#ifndef __ADDRESS_MODE_H__
#define __ADDRESS_MODE_H__


#define PUSH(x) memwr(0x100|reg->SP--,(x))
#define POP     memrd(0x100|(++reg->SP))


#define imp(reg,low,high,rd)                            \
    uint8_t __attribute((unused)) OP;

#define acc(reg,low,high,rd)                            \
    uint8_t OP=reg->A;

#define imm(reg,low,high,rd)                            \
    uint8_t OP=low;

#define abs(reg,low,high,rd)                            \
    uint8_t __attribute__((unused)) OP;                 \
    uint16_t addr = (uint16_t)high<<8 | low;            \
    if (rd) OP=memrd(addr);

#define absjmp(reg,low,high,rd)                         \
    uint16_t tgt = (uint16_t)high<<8 | low;

#define abs_x(reg,low,high,rd) \
    uint8_t __attribute__((unused)) OP;                 \
    uint16_t addr = (uint16_t)(high<<8|low + reg->X);   \
    if (rd) OP = memrd(addr);

#define abs_y(reg,low,high,rd)                          \
    uint8_t __attribute__((unused)) OP;                 \
    uint16_t addr = (uint16_t)(high<<8|low + reg->Y);   \
    if (rd) OP = memrd( addr );

#define ind(reg,low,high,rd)                            \
    uint8_t __attribute__((unused)) OP;                 \
    if (rd) OP=memrd((uint16_t)high<<8 | low);

#define zpg(reg,low,high,rd)                            \
    uint8_t __attribute__((unused)) OP;                 \
    uint16_t addr = low;                                \
    if (rd) OP=memrd(addr);

#define zpg_y(reg,low,high,rd)                          \
    uint8_t __attribute__((unused)) OP;                 \
    uint16_t addr = reg->Y + low;                       \
    if (rd) OP=memrd(addr);

#define zpg_x(reg,low,high,rd)                          \
    uint8_t __attribute__((unused)) OP;                 \
    uint16_t addr=(low+reg->X)&0xFF;                    \
    if (rd) OP=memrd(addr);

#define x_ind(reg,low,high,rd)                          \
    uint8_t __attribute__((unused)) OP;                 \
    uint16_t addr =  (low + reg->X) &0xFF;              \
    if (rd) OP = memrd(addr);

#define ind_y(reg,low,high,rd)                          \
    uint8_t __attribute__((unused)) OP;                 \
    uint16_t addr = memrd( low ) | memrd( low +1 );     \
    if (rd) OP = memrd( addr + reg->Y );

#define rel(reg,low,high,rd)                            \
    int16_t OP = (int16_t)((int8_t)low);

#define absind(reg,low,high,rd)                         \
    uint16_t tgt=reg->PC + (int8_t)low;



#endif
