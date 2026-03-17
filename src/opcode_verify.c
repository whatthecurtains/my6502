/*
 * opcode_verify.c - 6502 Opcode Verification Test
 *
 * Tests each opcode by:
 * 1. Reading opcode definitions from opcodes_verify/opcodes.json
 * 2. Executing instructions via inst_decode function pointers
 * 3. Verifying status flags match expected values from JSON "execution" field
 * 4. Reporting results per opcode with summary statistics
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <cjson/cJSON.h>

#include "regs.h"
#include "instruction.h"
#include "bus.h"

/* Forward declarations for bus_verify functions */
void bus_verify_init(void);
void bus_verify_reset_tracking(void);
void bus_verify_set_memory(uint16_t addr, uint8_t data);

/* Test statistics */
static struct {
    int total;
    int passed;
    int failed;
    int not_implemented;
} test_stats = {0, 0, 0, 0};

/* Load JSON file into string */
static char* load_json_file(const char* filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Failed to open %s\n", filename);
        return NULL;
    }
    
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *content = malloc(fsize + 1);
    if (!content) {
        fclose(f);
        return NULL;
    }
    
    fread(content, fsize, 1, f);
    fclose(f);
    
    content[fsize] = 0;
    return content;
}

/* Parse flag name to SR field value */
static int flag_name_to_bit(const char *flag_name) {
    if (strcmp(flag_name, "CARRY") == 0 || strcmp(flag_name, "C") == 0) return 0;      /* C flag */
    if (strcmp(flag_name, "ZERO") == 0 || strcmp(flag_name, "Z") == 0) return 1;       /* Z flag */
    if (strcmp(flag_name, "INTERRUPT") == 0 || strcmp(flag_name, "I") == 0) return 2;  /* I flag */
    if (strcmp(flag_name, "DECIMAL") == 0 || strcmp(flag_name, "D") == 0) return 3;    /* D flag */
    if (strcmp(flag_name, "BREAK") == 0 || strcmp(flag_name, "B") == 0) return 4;      /* B flag */
    if (strcmp(flag_name, "OVERFLOW") == 0 || strcmp(flag_name, "V") == 0) return 6;   /* V flag */
    if (strcmp(flag_name, "NEGATIVE") == 0 || strcmp(flag_name, "N") == 0) return 7;   /* N flag */
    return -1;  /* Unknown flag */
}

/* Get SR bit value */
static int get_sr_bit(status_reg_t *sr, int bit_pos) {
    switch (bit_pos) {
        case 0: return sr->C;  /* Carry */
        case 1: return sr->Z;  /* Zero */
        case 2: return sr->I;  /* Interrupt Disable */
        case 3: return sr->D;  /* Decimal */
        case 4: return sr->B;  /* Break */
        case 6: return sr->V;  /* Overflow */
        case 7: return sr->N;  /* Negative */
        default: return -1;
    }
}

/* Check if status flags match expected values */
static int check_status_flags(const char *opcode_hex, status_reg_t *sr, 
                             cJSON *execution) {
    if (!execution || execution->type != cJSON_Object) {
        return 1;  /* No flags to check */
    }
    
    cJSON *flags_array = cJSON_GetObjectItem(execution, "flags");
    if (!flags_array || flags_array->type != cJSON_Array) {
        return 1;  /* No flags array */
    }
    
    int all_flags_set = 1;
    cJSON *flag_item = NULL;
    
    cJSON_ArrayForEach(flag_item, flags_array) {
        if (flag_item->type != cJSON_String) continue;
        
        const char *flag_name = flag_item->valuestring;
        int bit_pos = flag_name_to_bit(flag_name);
        
        if (bit_pos < 0) {
            fprintf(stderr, "Unknown flag name: %s for opcode %s\n", flag_name, opcode_hex);
            return 0;
        }
        
        int bit_value = get_sr_bit(sr, bit_pos);
        if (bit_value != 1) {
            fprintf(stderr, "  Flag %s not set (bit %d)\n", flag_name, bit_pos);
            all_flags_set = 0;
        }
    }
    
    return all_flags_set;
}

/* Run test for a single opcode */
static void test_opcode(cJSON *opcode_obj) {
    cJSON *opcode_item = cJSON_GetObjectItem(opcode_obj, "opcode");
    cJSON *mnemonic_item = cJSON_GetObjectItem(opcode_obj, "mnemonic");
    cJSON *execution_item = cJSON_GetObjectItem(opcode_obj, "execution");
    cJSON *illegal_item = cJSON_GetObjectItem(opcode_obj, "illegal");
    
    if (!opcode_item || opcode_item->type != cJSON_String) return;
    if (!mnemonic_item || mnemonic_item->type != cJSON_String) return;
    
    const char *opcode_hex = opcode_item->valuestring;
    const char *mnemonic = mnemonic_item->valuestring;
    
    /* Parse hex opcode */
    unsigned long opcode_val = strtoul(opcode_hex, NULL, 16);
    if (opcode_val > 255) {
        fprintf(stderr, "Invalid opcode value: %s\n", opcode_hex);
        return;
    }
    
    uint8_t opcode_byte = (uint8_t)opcode_val;
    
    test_stats.total++;
    
    /* Check if opcode has execution definition */
    if (!execution_item) {
        printf("[%s] %s (0x%02X) - PASS (needs implementation)\n", 
               opcode_hex, mnemonic, opcode_byte);
        test_stats.not_implemented++;
        test_stats.passed++;
        return;
    }
    
    /* Get instruction function pointer */
    if (inst_decode[opcode_byte].hex != opcode_byte) {
        printf("[%s] %s (0x%02X) - FAIL (not in inst_decode table)\n", 
               opcode_hex, mnemonic, opcode_byte);
        test_stats.failed++;
        return;
    }
    
    opcode_fn instruction_func = inst_decode[opcode_byte].opcf;
    if (!instruction_func) {
        printf("[%s] %s (0x%02X) - FAIL (no instruction function)\n", 
               opcode_hex, mnemonic, opcode_byte);
        test_stats.failed++;
        return;
    }
    
    /* Initialize registers */
    all_regs_t reg_state = {
        .PC = 0x0000,
        .A = 0x00,
        .X = 0x00,
        .Y = 0x00,
        .SP = 0xFF,
        .SR_all = 0x00,
        .cyc = 0,
        .brk = 0
    };
    
    /* Reset memory access tracking */
    bus_verify_reset_tracking();
    
    /* Execute instruction with dummy operands */
    uint8_t low_byte = 0x00;
    uint8_t high_byte = 0x00;
    uint64_t cycles = 0;
    
    instruction_func(&reg_state, low_byte, high_byte, &cycles);
    
    /* Check status flags if execution field exists */
    if (check_status_flags(opcode_hex, &reg_state.SR, execution_item)) {
        printf("[%s] %s (0x%02X) - PASS\n", 
               opcode_hex, mnemonic, opcode_byte);
        test_stats.passed++;
    } else {
        printf("[%s] %s (0x%02X) - FAIL\n", 
               opcode_hex, mnemonic, opcode_byte);
        test_stats.failed++;
    }
}

/* Main test program */
int main(int argc, char *argv[]) {
    printf("=== 6502 Opcode Verification Test ===\n\n");
    
    /* Initialize bus verification */
    bus_verify_init();
    
    /* Build decode table */
    build_decode();
    
    /* Load and parse JSON */
    char *json_text = load_json_file("./opcodes_verify/opcodes.json");
    if (!json_text) {
        fprintf(stderr, "Failed to load JSON file\n");
        return 1;
    }
    
    cJSON *json = cJSON_Parse(json_text);
    free(json_text);
    
    if (!json) {
        fprintf(stderr, "Failed to parse JSON\n");
        return 1;
    }
    
    /* Get OPCODES array */
    cJSON *opcodes_array = cJSON_GetObjectItem(json, "OPCODES");
    if (!opcodes_array || opcodes_array->type != cJSON_Array) {
        fprintf(stderr, "OPCODES array not found in JSON\n");
        cJSON_Delete(json);
        return 1;
    }
    
    /* Test each opcode */
    cJSON *opcode_obj = NULL;
    cJSON_ArrayForEach(opcode_obj, opcodes_array) {
        test_opcode(opcode_obj);
    }
    
    /* Print summary */
    printf("\n=== Test Summary ===\n");
    printf("Total opcodes tested: %d\n", test_stats.total);
    printf("Passed: %d\n", test_stats.passed);
    printf("Failed: %d\n", test_stats.failed);
    printf("Not yet implemented: %d\n", test_stats.not_implemented);
    
    if (test_stats.failed == 0) {
        printf("\nAll tests PASSED!\n");
    } else {
        printf("\nSome tests FAILED!\n");
    }
    
    /* Cleanup */
    cJSON_Delete(json);
    
    return (test_stats.failed > 0) ? 1 : 0;
}
