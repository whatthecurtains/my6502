INSTR(BVS,rel,(2 + (reg->SR.V==1) ? 1 : 0 + ((next_pc&0xFF00)!=(reg->PC&0xFF00)) ? 1 : 0),0x70,2)
