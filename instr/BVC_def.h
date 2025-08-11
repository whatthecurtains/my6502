INSTR(BVC,rel,(2 + (reg->SR.V==0) ? 1 : 0 + ((next_pc&0xFF00)!=(reg->PC&0xFF00)) ? 1 : 0),0x50,2)
