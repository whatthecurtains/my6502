#!/usr/bin/env python3
import json
import re

# Read the opcodes.json file
with open('./opcodes.json', 'r') as f:
    data = json.load(f)

def get_effective_address(addressing_mode):
    """Generate the effective address notation based on addressing mode"""
    mode_map = {
        "Implied": None,  # No address
        "Accumulator": None,  # Operates on accumulator
        "Immediate": "M(PC+1)",  # Operand is at PC+1
        "Absolute": "M(PC+2)<<8|M(PC+1)",  # Address at PC+1,PC+2 (little-endian)
        "X-Indexed Absolute": "(M(PC+2)<<8|M(PC+1))+X",  # Absolute + X
        "Y-Indexed Absolute": "(M(PC+2)<<8|M(PC+1))+Y",  # Absolute + Y
        "Absolute Indirect": "M(M(PC+2)<<8|M(PC+1))|M(M(PC+2)<<8|M(PC+1)+1)<<8",  # JMP indirect
        "Zero Page": "M(PC+1)",  # Address at PC+1 (zero page)
        "X-Indexed Zero Page": "(M(PC+1)+X)&0xFF",  # Zero page + X
        "Y-Indexed Zero Page": "(M(PC+1)+Y)&0xFF",  # Zero page + Y
        "X-Indexed Zero Page Indirect": "M(M((M(PC+1)+X)&0xFF))|M(M((M(PC+1)+X)&0xFF)+1)<<8",  # ($nn,X)
        "Zero Page Indirect Y-Indexed": "M(M(M(PC+1))|(M(M(PC+1)+1)<<8)+Y)",  # ($nn),Y
        "Relative": "PC+M(PC+1)",  # PC + signed offset
    }
    return mode_map.get(addressing_mode, None)

def generate_inst_action(mnemonic, addressing_mode, description):
    """Generate inst_action based on mnemonic and addressing mode"""
    
    # Special case: NOP with no effects
    if mnemonic == "NOP" or mnemonic == "JAM":
        return ""
    
    actions = []
    addr = get_effective_address(addressing_mode)
    
    # Logical operations (update A and flags)
    if mnemonic in ["ORA", "AND", "EOR"]:
        actions.append("A")
        actions.append("STATUS.Z, STATUS.N")
    
    # Shift/Rotate left (result to A or memory, updates C and N,Z)
    elif mnemonic in ["ASL"]:
        if addressing_mode == "Accumulator":
            actions.append("A, STATUS.C, STATUS.Z, STATUS.N")
        else:
            actions.append(f"M({addr}), STATUS.C, STATUS.Z, STATUS.N")
    
    # Shift/Rotate right (result to A or memory, updates C and N,Z)
    elif mnemonic in ["LSR"]:
        if addressing_mode == "Accumulator":
            actions.append("A, STATUS.C, STATUS.Z, STATUS.N")
        else:
            actions.append(f"M({addr}), STATUS.C, STATUS.Z, STATUS.N")
    
    # Rotate left through carry
    elif mnemonic in ["ROL"]:
        if addressing_mode == "Accumulator":
            actions.append("A, STATUS.C, STATUS.Z, STATUS.N")
        else:
            actions.append(f"M({addr}), STATUS.C, STATUS.Z, STATUS.N")
    
    # Rotate right through carry
    elif mnemonic in ["ROR"]:
        if addressing_mode == "Accumulator":
            actions.append("A, STATUS.C, STATUS.Z, STATUS.N")
        else:
            actions.append(f"M({addr}), STATUS.C, STATUS.Z, STATUS.N")
    
    # Add with carry (A <= A + M, updates flags)
    elif mnemonic == "ADC":
        actions.append("A, STATUS.C, STATUS.Z, STATUS.V, STATUS.N")
    
    # Subtract with carry (A <= A - M, updates flags)
    elif mnemonic == "SBC":
        actions.append("A, STATUS.C, STATUS.Z, STATUS.V, STATUS.N")
    
    # Compare (updates flags only)
    elif mnemonic in ["CMP", "CPX", "CPY"]:
        actions.append("STATUS.Z, STATUS.C, STATUS.N")
    
    # Bit test (updates flags)
    elif mnemonic == "BIT":
        actions.append("STATUS.Z, STATUS.V, STATUS.N")
    
    # Load accumulator
    elif mnemonic == "LDA":
        actions.append("A, STATUS.Z, STATUS.N")
    
    # Load X register
    elif mnemonic == "LDX":
        actions.append("X, STATUS.Z, STATUS.N")
    
    # Load Y register
    elif mnemonic == "LDY":
        actions.append("Y, STATUS.Z, STATUS.N")
    
    # Store accumulator
    elif mnemonic == "STA":
        actions.append(f"M({addr})")
    
    # Store X register
    elif mnemonic == "STX":
        actions.append(f"M({addr})")
    
    # Store Y register
    elif mnemonic == "STY":
        actions.append(f"M({addr})")
    
    # Transfers
    elif mnemonic == "TAX":
        actions.append("X, STATUS.Z, STATUS.N")
    elif mnemonic == "TAY":
        actions.append("Y, STATUS.Z, STATUS.N")
    elif mnemonic == "TXA":
        actions.append("A, STATUS.Z, STATUS.N")
    elif mnemonic == "TYA":
        actions.append("A, STATUS.Z, STATUS.N")
    elif mnemonic == "TSX":
        actions.append("X, STATUS.Z, STATUS.N")
    elif mnemonic == "TXS":
        actions.append("SP")
    
    # Increment/Decrement
    elif mnemonic == "INX":
        actions.append("X, STATUS.Z, STATUS.N")
    elif mnemonic == "INY":
        actions.append("Y, STATUS.Z, STATUS.N")
    elif mnemonic == "DEX":
        actions.append("X, STATUS.Z, STATUS.N")
    elif mnemonic == "DEY":
        actions.append("Y, STATUS.Z, STATUS.N")
    elif mnemonic == "INC":
        actions.append(f"M({addr}), STATUS.Z, STATUS.N")
    elif mnemonic == "DEC":
        actions.append(f"M({addr}), STATUS.Z, STATUS.N")
    
    # Flag operations
    elif mnemonic == "CLC":
        actions.append("STATUS.C<=0")
    elif mnemonic == "CLD":
        actions.append("STATUS.D<=0")
    elif mnemonic == "CLI":
        actions.append("STATUS.I<=0")
    elif mnemonic == "CLV":
        actions.append("STATUS.V<=0")
    elif mnemonic == "SEC":
        actions.append("STATUS.C<=1")
    elif mnemonic == "SED":
        actions.append("STATUS.D<=1")
    elif mnemonic == "SEI":
        actions.append("STATUS.I<=1")
    
    # Stack operations
    elif mnemonic == "PHA":
        actions.append("M(SP)")
    elif mnemonic == "PHP":
        actions.append("M(SP)")
    elif mnemonic == "PLA":
        actions.append("A, STATUS.Z, STATUS.N")
    elif mnemonic == "PLP":
        actions.append("STATUS")
    
    # Jump and subroutine
    elif mnemonic == "JMP":
        actions.append(f"PC<=M({addr})")
    elif mnemonic == "JSR":
        actions.append(f"M(SP)<=(PC+2)&0xFF, M(SP-1)<=(PC+2)>>8, PC<=M({addr})")
    
    # Return from subroutine
    elif mnemonic == "RTS":
        actions.append(f"PC<=M(SP+1)|M(SP+2)<<8")
    
    # Return from interrupt
    elif mnemonic == "RTI":
        actions.append(f"STATUS<=M(SP+1), PC<=M(SP+2)|M(SP+3)<<8")
    
    # Break
    elif mnemonic == "BRK":
        actions.append(f"M(SP)<=(PC+2)&0xFF, M(SP-1)<=(PC+2)>>8, M(SP-2)<=SR, STATUS.I<=1, PC<=M(0xFFFE)|M(0xFFFF)<<8")
    
    # Branch instructions
    elif mnemonic in ["BPL", "BMI", "BVC", "BVS", "BCC", "BCS", "BNE", "BEQ"]:
        actions.append(f"PC<=PC+M(PC+1)")
    
    # Undocumented opcodes - SLO (shift left then OR)
    elif mnemonic == "SLO":
        if addressing_mode == "Accumulator":
            actions.append("A, STATUS.C, STATUS.Z, STATUS.N")
        else:
            actions.append(f"M({addr}), A, STATUS.C, STATUS.Z, STATUS.N")
    
    # Undocumented opcodes - ANC (AND then move bit 7 to carry)
    elif mnemonic == "ANC":
        actions.append("A, STATUS.C, STATUS.Z, STATUS.N")
    
    # Default: if we don't recognize it, leave empty
    return ", ".join(actions) if actions else ""

# Process all opcodes
for opcode in data['OPCODES']:
    mnemonic = opcode.get('mnemonic', '')
    addressing_mode = opcode.get('addressing mode', '')
    description = opcode.get('description', '')
    
    inst_action = generate_inst_action(mnemonic, addressing_mode, description)
    opcode['inst_action'] = inst_action

# Write to op1.json
with open('./op1.json', 'w') as f:
    json.dump(data, f, indent=2)

print(f"Successfully created op1.json with {len(data['OPCODES'])} opcodes")
print("\nSample entries:")
for i, opcode in enumerate(data['OPCODES'][:10]):
    print(f"{opcode['opcode']}: {opcode['mnemonic']:4s} ({opcode['addressing mode']:30s}) -> {opcode['inst_action']}")
