#ifndef __PSP_PATCH_H__
#define __PSP_PATCH_H__

#define REG32(ADDR) (*(vu32*)(ADDR))

#define MIPS_LUI(R, IMM) 0x3c000000 | (R << 16) | ((unsigned int)(IMM)&0xffff)

#define MIPS_ADDI(RT, RS, IMM) (0x20000000 | (RS << 21) | (RT << 16) | ((unsigned int)(IMM)&0xffff))
#define MIPS_ADDIU(RT, RS, IMM)                                                                    \
    (0x24000000 | (RS << 21) | (RT << 16) | ((unsigned int)(IMM)&0xffff))
#define MIPS_ANDI(RT, RS, IMM) (0x30000000 | (RS << 21) | (RT << 16) | ((unsigned int)(IMM)&0xffff))
#define MIPS_ORI(RT, RS, IMM) (0x34000000 | (RS << 21) | (RT << 16) | ((unsigned int)(IMM)&0xffff))
#define MIPS_NOP 0x00000000
#define MIPS_J(ADDR) (0x08000000 + ((((unsigned int)(ADDR)) & 0x0ffffffc) >> 2))
#define MIPS_JR(R) (0x00000008 + ((R) << 21))
#define MIPS_JAL(ADDR) (0x0c000000 + (((unsigned int)(ADDR) >> 2) & 0x03ffffff))
#define MIPS_SYSCALL(NUM) (0x0000000C + ((NUM) << 6))

#define MIPS_AND(RD, RS, RT) (0x00000024 | (RD << 11) | (RT << 16) | (RS << 21))
#define MIPS_ADD(RD, RS, RT) (0x00000020 | (RD << 11) | (RT << 16) | (RS << 21))
#define MIPS_ADDU(RD, RS, RT) (0x00000021 | (RD << 11) | (RT << 16) | (RS << 21))

#define MIPS_SW(RT, BASE, OFFSET) (0xac000000 | (BASE << 21) | (RT << 16) | (OFFSET & 0xFFFF))
#define MIPS_SH(RT, BASE, OFFSET) (0xa4000000 | (BASE << 21) | (RT << 16) | (OFFSET & 0xFFFF))

#define SYNC() asm(" sync; nop" ::)

#endif
