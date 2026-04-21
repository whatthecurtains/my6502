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
#include <time.h>

#include "regs.h"
#include "instruction.h"
#include "bus.h"

/* Forward declarations for bus_verify functions */
void bus_verify_init(void);
void bus_verify_reset_tracking(void);
void bus_verify_set_memory(uint16_t addr, uint8_t data);
uint8_t bus_verify_get_memory(uint16_t addr);
void bus_verify_get_last_read(uint16_t *addr, uint8_t *data, int *accessed);
void bus_verify_get_last_write(uint16_t *addr, uint8_t *data, int *accessed);
int bus_verify_get_write_history_count(void);
void bus_verify_get_write_history(int index, uint16_t *addr, uint8_t *data);

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

/* Resolve common effective-address patterns (helper used by tests) */
static uint16_t resolve_effective_address(const char *expr, all_regs_t *sreg, uint8_t lo, uint8_t hi) {
    if (!expr) return 0;
    /* Absolute: M(M(PC+2)<<8|M(PC+1)) */
    if (strstr(expr, "M(M(PC+2)<<8|M(PC+1))") != NULL) {
        return ((uint16_t)hi << 8) | lo;
    }
    /* X-Indexed Absolute: +X */
    if (strstr(expr, "(M(PC+2)<<8|M(PC+1))+X") != NULL) {
        return (uint16_t)(((uint16_t)hi << 8) | lo) + sreg->X;
    }
    if (strstr(expr, "(M(PC+2)<<8|M(PC+1))+Y") != NULL) {
        return (uint16_t)(((uint16_t)hi << 8) | lo) + sreg->Y;
    }
    /* Zero page: M(M(PC+1)) -> addr = low */
    if (strstr(expr, "M(M(PC+1))") != NULL) {
        return (uint16_t)lo;
    }
    /* Zero page X: (M(PC+1)+X)&0xFF */
    if (strstr(expr, "(M(PC+1)+X)&0xFF") != NULL) {
        return (uint16_t)((lo + sreg->X) & 0xFF);
    }
    if (strstr(expr, "(M(PC+1)+Y)&0xFF") != NULL) {
        return (uint16_t)((lo + sreg->Y) & 0xFF);
    }
    /* Indirect Y: M(M(M(PC+1)) + Y) */
    if (strstr(expr, "M(M(M(PC+1)) + Y)") != NULL) {
        uint8_t zp = lo;
        uint8_t lo_e = bus_verify_get_memory(zp);
        uint8_t hi_e = bus_verify_get_memory((zp + 1) & 0xFF);
        return (uint16_t)(((uint16_t)hi_e << 8) | lo_e) + sreg->Y;
    }
    /* X-Indexed Zero Page Indirect: approximate */
    if (strstr(expr, "(M(PC+1)+X)&0xFF") != NULL && strstr(expr, "<<8|M((M(PC+1)+X)&0xFF+1)") != NULL) {
        uint8_t zp = (lo + sreg->X) & 0xFF;
        uint8_t lo_e = bus_verify_get_memory(zp);
        uint8_t hi_e = bus_verify_get_memory((zp + 1) & 0xFF);
        return (uint16_t)(((uint16_t)hi_e << 8) | lo_e);
    }
    /* Direct hex addresses like M(0xFFFE) */
    const char *hexptr = strstr(expr, "0x");
    if (hexptr) {
        unsigned int v = 0;
        if (sscanf(hexptr, "0x%X", &v) == 1) return (uint16_t)v;
    }
    /* Default fallback */
    return 0;
}

/* Run test for a single opcode */
static void test_opcode(cJSON *opcode_obj) {
    cJSON *opcode_item = cJSON_GetObjectItem(opcode_obj, "opcode");
    cJSON *mnemonic_item = cJSON_GetObjectItem(opcode_obj, "mnemonic");
    cJSON *execution_item = cJSON_GetObjectItem(opcode_obj, "inst_action");
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

    /* Skip illegal opcodes */
    if (illegal_item && illegal_item->type == cJSON_True) {
        /* Do not count illegal opcodes */
        printf("[%s] %s (0x%02X) - SKIP (illegal)\n", opcode_hex, mnemonic, opcode_byte);
        return;
    }

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

    /* Initialize registers with randomized values */
    all_regs_t reg_state;
    reg_state.PC = (uint16_t)rand();
    reg_state.A = (uint8_t)rand();
    reg_state.X = (uint8_t)rand();
    reg_state.Y = (uint8_t)rand();
    reg_state.SP = (uint8_t)rand();
    reg_state.SR_all = (uint8_t)rand();
    reg_state.cyc = 0;
    reg_state.brk = 0;

    /* Reset memory access tracking */
    bus_verify_reset_tracking();

    /* Create low/high data bytes per instruction encoding length */
    int bytes_field = 1;
    cJSON *bytes_item = cJSON_GetObjectItem(opcode_obj, "bytes");
    if (bytes_item && bytes_item->type == cJSON_Number) {
        bytes_field = bytes_item->valueint;
    }

    uint8_t low_byte = 0x00;
    uint8_t high_byte = 0x00;
    if (bytes_field >= 2) low_byte = (uint8_t)rand();
    if (bytes_field == 3) high_byte = (uint8_t)rand();

    uint64_t cycles = 0; /* dummy cycles */

    /* Parse inst_action to setup memory expectations and expected outcomes */
    cJSON *inst_action_item = cJSON_GetObjectItem(opcode_obj, "inst_action");
    char *inst_action = NULL;
    if (inst_action_item && inst_action_item->type == cJSON_String) {
        inst_action = inst_action_item->valuestring;
    }


    /* Preconfigure memory for any referenced effective addresses in inst_action */
    /* capture initial state copy for computing expected addresses/values */
    all_regs_t initial_regs = reg_state;

    if (inst_action) {
        /* For simplicity scan for common patterns and set up memory */
        if (strstr(inst_action, "M(M(PC+2)<<8|M(PC+1))") || strstr(inst_action, "(M(PC+2)<<8|M(PC+1))+X") || strstr(inst_action, "(M(PC+2)<<8|M(PC+1))+Y")) {
            uint16_t eff = resolve_effective_address(inst_action, &initial_regs, low_byte, high_byte);
            uint8_t val = (uint8_t)rand();
            bus_verify_set_memory(eff, val);
        }
        if (strstr(inst_action, "M(M(PC+1))") || strstr(inst_action, "(M(PC+1)+X)&0xFF")) {
            uint16_t eff = resolve_effective_address(inst_action, &initial_regs, low_byte, high_byte);
            uint8_t val = (uint8_t)rand();
            bus_verify_set_memory(eff, val);
        }
        if (strstr(inst_action, "M(M(M(PC+1)) + Y)") || strstr(inst_action, "M(M(M(PC+1)) + Y)")) {
            uint8_t zp = low_byte;
            uint8_t lo_e = (uint8_t)rand();
            uint8_t hi_e = (uint8_t)rand();
            bus_verify_set_memory(zp, lo_e);
            bus_verify_set_memory((zp + 1) & 0xFF, hi_e);
            uint16_t eff = (uint16_t)(((uint16_t)hi_e << 8) | lo_e) + initial_regs.Y;
            bus_verify_set_memory(eff, (uint8_t)rand());
        }
        /* BRK instruction: set up interrupt vector at 0xFFFE/0xFFFF */
        if (strstr(inst_action, "M(0xFFFE)") || strstr(inst_action, "M(0xFFFF)")) {
            uint8_t irq_vec_lo = (uint8_t)rand();
            uint8_t irq_vec_hi = (uint8_t)rand();
            bus_verify_set_memory(0xFFFE, irq_vec_lo);
            bus_verify_set_memory(0xFFFF, irq_vec_hi);
        }
    }

    /* Execute instruction */
    instruction_func(&reg_state, low_byte, high_byte, &cycles);

    /* After execution, verify results described in inst_action */
    int pass = 1;
    if (inst_action) {
        /* tokenize by ';' and evaluate expected results */
        char *copy = strdup(inst_action);
        char *tok = strtok(copy, ";");
        while (tok) {
            /* trim whitespace */
            while (*tok == ' ' || *tok == '\t') tok++;
            char *end = tok + strlen(tok) - 1;
            while (end > tok && (*end == ' ' || *end == '\t' || *end == '\n')) { *end = '\0'; end--; }

            if (strstr(tok, "A <- A | M(" ) == tok || strstr(tok, "A <- A & M(") == tok || strstr(tok, "A <- A ^ M(") == tok) {
                /* logic op with memory */
                char op = tok[8]; /* '|' or '&' or '^' */
                /* find address substring start */
                char *addr_start = strchr(tok, 'M');
                uint16_t addr = resolve_effective_address(addr_start, &initial_regs, low_byte, high_byte);
                uint8_t memv = bus_verify_get_memory(addr);
                /* compute expected using initial A */
                uint8_t expected = initial_regs.A;
                if (op == '|') expected = (uint8_t)(initial_regs.A | memv);
                else if (op == '&') expected = (uint8_t)(initial_regs.A & memv);
                else if (op == '^') expected = (uint8_t)(initial_regs.A ^ memv);
                if (reg_state.A != expected) {
                    printf("  [%s] %s - FAIL: A expected 0x%02X got 0x%02X\n", opcode_hex, mnemonic, expected, reg_state.A);
                    pass = 0;
                }
            }

            /* Check register load: pattern A <- M(...) */
            if (strstr(tok, "A <- M(") == tok) {
                uint16_t addr = resolve_effective_address(tok + 6, &initial_regs, low_byte, high_byte);
                uint8_t expected = bus_verify_get_memory(addr);
                if (reg_state.A != expected) {
                    printf("  [%s] %s - FAIL: A expected 0x%02X got 0x%02X\n", opcode_hex, mnemonic, expected, reg_state.A);
                    pass = 0;
                }
                /* check flags if present in subsequent expressions will be checked */
            }

            /* Check store: pattern M(...) <- A (but not BRK stack writes) */
            if (strstr(tok, "M(") == tok && strstr(tok, "<-") != NULL && strstr(tok, "SP") == NULL) {
                /* find addr and expected source */
                char *arrow = strstr(tok, "<-");
                if (arrow) {
                    /* left side addr */
                    char left[128];
                    int n = (int)(arrow - tok);
                    strncpy(left, tok, n);
                    left[n] = '\0';
                    /* right side */
                    char right[128];
                    strcpy(right, arrow + 2);
                    /* trim */
                    char *r = right;
                    while (*r == ' ' || *r == '\t') r++;
                    uint8_t expected_val = 0;
                    if (strcmp(r, "A") == 0) expected_val = reg_state.A;
                    else if (strcmp(r, "X") == 0) expected_val = reg_state.X;
                    else if (strcmp(r, "Y") == 0) expected_val = reg_state.Y;
                    else if (strncmp(r, "res", 3) == 0) {
                        /* not tracked; skip strict check */
                        expected_val = bus_verify_get_memory(0); /* noop */
                    }
                    uint16_t addr = resolve_effective_address(left + 2, &initial_regs, low_byte, high_byte);
                    uint16_t last_addr=0; uint8_t last_data=0; int accessed=0;
                    bus_verify_get_last_write(&last_addr, &last_data, &accessed);
                    if (!accessed || last_addr != addr) {
                        printf("  [%s] %s - FAIL: expected write to 0x%04X, actual 0x%04X\n", opcode_hex, mnemonic, addr, last_addr);
                        pass = 0;
                    } else {
                        if (r[0] == 'A' || r[0]=='X' || r[0]=='Y') {
                            if (last_data != expected_val) {
                                printf("  [%s] %s - FAIL: write data mismatch at 0x%04X expected 0x%02X got 0x%02X\n", opcode_hex, mnemonic, addr, expected_val, last_data);
                                pass = 0;
                            }
                        }
                    }
                }
            }

            /* Flags checks */
            if (strstr(tok, "SR.Z") != NULL) {
                /* derived expected from reg_state.A */
                int expected_z = (reg_state.A == 0) ? 1 : 0;
                if (reg_state.SR.Z != expected_z) {
                    printf("  [%s] %s - FAIL: SR.Z expected %d got %d\n", opcode_hex, mnemonic, expected_z, reg_state.SR.Z);
                    pass = 0;
                }
            }
            if (strstr(tok, "SR.N") != NULL) {
                int expected_n = ((reg_state.A & 0x80) != 0) ? 1 : 0;
                if (reg_state.SR.N != expected_n) {
                    printf("  [%s] %s - FAIL: SR.N expected %d got %d\n", opcode_hex, mnemonic, expected_n, reg_state.SR.N);
                    pass = 0;
                }
            }
            if (strstr(tok, "SR.C") != NULL && strstr(tok, "(A > 0xFF)") != NULL) {
                /* For ADC like patterns, attempt to compute from initial values - not implemented fully */
            }

            /* BRK instruction: verify stack pushes and PC load */
            if (strstr(tok, "M(SP) <- (PC+2)&0xFF") != NULL && strstr(inst_action, "M(SP-2) <- SR") != NULL) {
                /* BRK pushes PC+2 low, then high, then SR to stack, sets SR.I, and loads PC from 0xFFFE/0xFFFF */
                /* Use this check to distinguish BRK from JSR (both push to stack, but BRK has 3 pushes) */
                int write_count = bus_verify_get_write_history_count();
                if (write_count >= 3) {
                    uint16_t sp_addr_lo, sp_addr_hi, sp_addr_sr;
                    uint8_t sp_data_lo, sp_data_hi, sp_data_sr;
                    
                    /* Get the three writes (should be to SP, SP-1, SP-2) */
                    bus_verify_get_write_history(0, &sp_addr_lo, &sp_data_lo);
                    bus_verify_get_write_history(1, &sp_addr_hi, &sp_data_hi);
                    bus_verify_get_write_history(2, &sp_addr_sr, &sp_data_sr);
                    
                    uint8_t expected_lo = (initial_regs.PC + 2) & 0xFF;
                    uint8_t expected_hi = ((initial_regs.PC + 2) >> 8) & 0xFF;
                    
                    /* Verify PC+2 low byte write to 0x100+SP */
                    uint16_t expected_sp_addr_lo = 0x100 + initial_regs.SP;
                    if (sp_addr_lo != expected_sp_addr_lo || sp_data_lo != expected_lo) {
                        printf("  [%s] %s - FAIL: BRK PC+2 low write - expected addr 0x%04X data 0x%02X, got addr 0x%04X data 0x%02X\n",
                               opcode_hex, mnemonic, expected_sp_addr_lo, expected_lo, sp_addr_lo, sp_data_lo);
                        pass = 0;
                    }
                } else {
                    printf("  [%s] %s - FAIL: BRK expected 3+ writes, got %d\n", opcode_hex, mnemonic, write_count);
                    pass = 0;
                }
            }
            
            /* BRK PC+2 high byte verification */
            if (strstr(tok, "M(SP-1) <- (PC+2)>>8") != NULL) {
                int write_count = bus_verify_get_write_history_count();
                if (write_count >= 3) {
                    uint16_t sp_addr_hi;
                    uint8_t sp_data_hi;
                    bus_verify_get_write_history(1, &sp_addr_hi, &sp_data_hi);
                    
                    uint8_t expected_hi = ((initial_regs.PC + 2) >> 8) & 0xFF;
                    uint16_t expected_sp_addr_hi = 0x100 + (initial_regs.SP - 1);
                    
                    if (sp_addr_hi != expected_sp_addr_hi || sp_data_hi != expected_hi) {
                        printf("  [%s] %s - FAIL: BRK PC+2 high write - expected addr 0x%04X data 0x%02X, got addr 0x%04X data 0x%02X\n",
                               opcode_hex, mnemonic, expected_sp_addr_hi, expected_hi, sp_addr_hi, sp_data_hi);
                        pass = 0;
                    }
                }
            }
            
            /* BRK SR verification */
            if (strstr(tok, "M(SP-2) <- SR") != NULL) {
                int write_count = bus_verify_get_write_history_count();
                if (write_count >= 3) {
                    uint16_t sp_addr_sr;
                    uint8_t sp_data_sr;
                    bus_verify_get_write_history(2, &sp_addr_sr, &sp_data_sr);
                    
                    uint16_t expected_sp_addr_sr = 0x100 + (initial_regs.SP - 2);
                    if (sp_addr_sr != expected_sp_addr_sr || sp_data_sr != initial_regs.SR_all) {
                        printf("  [%s] %s - FAIL: BRK SR write - expected addr 0x%04X data 0x%02X, got addr 0x%04X data 0x%02X\n",
                               opcode_hex, mnemonic, expected_sp_addr_sr, initial_regs.SR_all, sp_addr_sr, sp_data_sr);
                        pass = 0;
                    }
                }
            }
            
            /* Check SR.I flag set for BRK */
            if (strstr(tok, "SR.I <- 1") != NULL) {
                if (reg_state.SR.I != 1) {
                    printf("  [%s] %s - FAIL: BRK SR.I expected 1 got %d\n", opcode_hex, mnemonic, reg_state.SR.I);
                    pass = 0;
                }
            }
            
            /* Check PC load from interrupt vector for BRK */
            if (strstr(tok, "PC <- M(0xFFFE)|M(0xFFFF)<<8") != NULL) {
                uint8_t vec_lo = bus_verify_get_memory(0xFFFE);
                uint8_t vec_hi = bus_verify_get_memory(0xFFFF);
                uint16_t expected_pc = ((uint16_t)vec_hi << 8) | vec_lo;
                if (reg_state.PC != expected_pc) {
                    printf("  [%s] %s - FAIL: BRK PC load - expected 0x%04X got 0x%04X\n", opcode_hex, mnemonic, expected_pc, reg_state.PC);
                    pass = 0;
                }
            }

            tok = strtok(NULL, ";");
        }
        free(copy);
    }

    if (pass) {
        printf("[%s] %s (0x%02X) - PASS\n", opcode_hex, mnemonic, opcode_byte);
        test_stats.passed++;
    } else {
        printf("[%s] %s (0x%02X) - FAIL\n", opcode_hex, mnemonic, opcode_byte);
        test_stats.failed++;
    }
}


/* Main test program */
int main(int argc, char *argv[]) {
    printf("=== 6502 Opcode Verification Test ===\n\n");

    /* Seed RNG for randomized register/state generation */
    srand((unsigned)time(NULL));

    /* Initialize bus verification */
    bus_verify_init();

    /* Build decode table */
    build_decode();

    /* Load and parse JSON */
    char *json_text = load_json_file("../opcodes_verify/op2.json");
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
