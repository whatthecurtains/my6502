INSTR(BMI,rel,(2 + (reg->SR.N==1) ? 1 : 0 + ((next_pc&0xFF00)!=(reg->PC&0xFF00)) ? 1 : 0),0x30,2)
