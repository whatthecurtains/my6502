INSTR(BPL,rel,(2 + (reg->SR.N==0) ? 1 : 0 + ((next_pc&0xFF00)!=(reg->PC&0xFF00)) ? 1 : 0),0x10,2)
