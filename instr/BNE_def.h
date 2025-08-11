INSTR(BNE,rel,(2 + (reg->SR.Z==0) ? 1 : 0 + ((next_pc&0xFF00)!=(reg->PC&0xFF00)) ? 1 : 0),0xd0,2)
