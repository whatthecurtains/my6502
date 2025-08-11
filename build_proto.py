

opcode_list="ADC AND ASL BCC BCS BEQ BIT BMI BNE BPL BRK BVC BVS CLC CLD CLI CLV CMP CPX CPY DEC DEX DEY EOR INC INX INY JMP JSR LDA LDX LDY LSR NOP ORA PHA PHP PLA PLP ROL ROR RTI RTS SBC SEC SED SEI STA STX STY TAX TAY TSX TXA TXS TYA"

addr_mode_list="_imp _abs _abs_x _abs_y _imm _ind _x_ind _ind_y _rel _zpg _zpg_x _zpg_y"

opcodes=opcode_list.split(' ')
addr_modes=addr_mode_list.split(' ')

f=open("protos.txt","w")

for op in opcodes:
    for mode in addr_modes:
        f.write(f"opcode_fn {op}{mode};\n")
        
f.close()
        
        