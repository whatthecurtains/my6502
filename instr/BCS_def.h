INSTR(BCS,rel,(2 + (reg->SR.C==0) ? 1 : 0 + ((next_pc&0xFF00)!=(reg->PC&0xFF00)) ? 1 : 0),0xb0,2)
