#define ADC(mode,cc) \
    all_regs_t* ADC_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define AND(mode,cc) \
    all_regs_t* AND_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define ASL(mode,cc) \
    all_regs_t* ASL_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define BCC(mode,cc) \
    all_regs_t* BCC_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define BCS(mode,cc) \
    all_regs_t* BCS_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define BEQ(mode,cc) \
    all_regs_t* BEQ_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define BIT(mode,cc) \
    all_regs_t* BIT_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define BMI(mode,cc) \
    all_regs_t* BMI_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define BNE(mode,cc) \
    all_regs_t* BNE_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define BPL(mode,cc) \
    all_regs_t* BPL_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define BRK(mode,cc) \
    all_regs_t* BRK_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define BVC(mode,cc) \
    all_regs_t* BVC_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define BVS(mode,cc) \
    all_regs_t* BVS_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define CLC(mode,cc) \
    all_regs_t* CLC_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define CLD(mode,cc) \
    all_regs_t* CLD_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define CLI(mode,cc) \
    all_regs_t* CLI_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define CLV(mode,cc) \
    all_regs_t* CLV_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define CMP(mode,cc) \
    all_regs_t* CMP_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define CPX(mode,cc) \
    all_regs_t* CPX_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define CPY(mode,cc) \
    all_regs_t* CPY_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define DCP(mode,cc) \
    all_regs_t* DCP_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define DEC(mode,cc) \
    all_regs_t* DEC_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define DEX(mode,cc) \
    all_regs_t* DEX_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define DEY(mode,cc) \
    all_regs_t* DEY_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define EOR(mode,cc) \
    all_regs_t* EOR_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define INC(mode,cc) \
    all_regs_t* INC_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define INX(mode,cc) \
    all_regs_t* INX_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define INY(mode,cc) \
    all_regs_t* INY_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define JMP(mode,cc) \
    all_regs_t* JMP_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define JSR(mode,cc) \
    all_regs_t* JSR_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define LDA(mode,cc) \
    all_regs_t* LDA_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define LDX(mode,cc) \
    all_regs_t* LDX_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define LDY(mode,cc) \
    all_regs_t* LDY_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define LSR(mode,cc) \
    all_regs_t* LSR_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define NOP(mode,cc) \
    all_regs_t* NOP_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define ORA(mode,cc) \
    all_regs_t* ORA_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define PHA(mode,cc) \
    all_regs_t* PHA_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define PHP(mode,cc) \
    all_regs_t* PHP_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define PLA(mode,cc) \
    all_regs_t* PLA_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define PLP(mode,cc) \
    all_regs_t* PLP_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define ROL(mode,cc) \
    all_regs_t* ROL_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define ROR(mode,cc) \
    all_regs_t* ROR_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define RTI(mode,cc) \
    all_regs_t* RTI_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define RTS(mode,cc) \
    all_regs_t* RTS_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define SBC(mode,cc) \
    all_regs_t* SBC_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define SEC(mode,cc) \
    all_regs_t* SEC_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define SED(mode,cc) \
    all_regs_t* SED_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define SEI(mode,cc) \
    all_regs_t* SEI_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define SHY(mode,cc) \
    all_regs_t* SHY_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define STA(mode,cc) \
    all_regs_t* STA_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define STX(mode,cc) \
    all_regs_t* STX_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define STY(mode,cc) \
    all_regs_t* STY_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define TAX(mode,cc) \
    all_regs_t* TAX_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define TAY(mode,cc) \
    all_regs_t* TAY_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define TSX(mode,cc) \
    all_regs_t* TSX_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define TXA(mode,cc) \
    all_regs_t* TXA_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define TXS(mode,cc) \
    all_regs_t* TXS_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);
#define TYA(mode,cc) \
    all_regs_t* TYA_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);


#define INSTR(inst,mode,cc,hex,bytes)  \
    inst(mode,cc)

#include "all_instr.c"
