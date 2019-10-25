#ifndef PSPMACRO_H_
#define PSPMACRO_H_

#include <pspsdk.h>

// MIPS patching utilities
#define MIPS_J(a) (0x08000000 | (((u32)(f)&0x0ffffffc) >> 2))
#define MAKE_CALL(a, f) _sw(0x0C000000 | (((u32)(f) >> 2) & 0x03ffffff), a)
#define MAKE_JUMP(a, f) _sw(0x08000000 | (((u32)(f)&0x0ffffffc) >> 2), a)
#define REDIRECT_FUNCTION(a, f)                                                                    \
    {                                                                                              \
        u32 address = a;                                                                           \
        _sw(0x08000000 | (((u32)(f) >> 2) & 0x03ffffff), address);                                 \
        _sw(0, address + 4);                                                                       \
    }
#define MAKE_RELATIVE_ADDR(a, f, t) _sh(((((f - a) >> 2) - 1) & 0xFFFF), a + t)
#define MAKE_RELATIVE_BRANCH(a, f, t) _sw((0x10000000 | ((((f - a) >> 2) - 1) & 0xFFFF)), a + t)
#define MAKE_FUNCTION_RETURN0(a)                                                                   \
    _sw(0x03E00008, a);                                                                            \
    _sw(0x00001021, a + 4)

#define MAKE_FUNCTION_RETURN(a, v)                                                                 \
    _sw(0x03E00008, a);                                                                            \
    _sw(LI(GPREG_V0, 1), a + 4)

#define EXTRACT_J_ADDR(x) (((((u32)_lw((u32)x)) & ~0xFC000000) << 2) | 0x80000000)

#define KERNEL_HIJACK_FUNCTION(a, f, ptr)                                                          \
    {                                                                                              \
        static u32 patch_buffer[3];                                                                \
        _sw(_lw(a + 0x00), (u32)patch_buffer + 0x00);                                              \
        _sw(_lw(a + 0x04), (u32)patch_buffer + 0x08);                                              \
        MAKE_JUMP((u32)patch_buffer + 0x04, a + 0x08);                                             \
        REDIRECT_FUNCTION(a, f);                                                                   \
        ptr = (void*)patch_buffer;                                                                 \
    }

#define MIPS_ITYPE(op, rs, rt, imm)                                                                \
    (((op & 0x3F) << 26) | ((rs & 0x1F) << 21) | ((rt & 0x1F) << 16) | (imm & 0xFFFF))

#define LUI(rt, imm) MIPS_ITYPE(0x0F, 0, rt, imm)
#define ORI(rt, rs, imm) MIPS_ITYPE(0x0D, rs, rt, imm)
#define LW(rt, rs, imm) MIPS_ITYPE(0x23, rs, rt, imm)
#define SW(rt, rs, imm) MIPS_ITYPE(0x2B, rs, rt, imm)
#define ADDIU(rt, rs, imm) MIPS_ITYPE(0x09, rs, rt, imm)
#define LI(rt, imm) ADDIU(rt, GPREG_ZR, imm)
#define EXTRACT_IMM(imm) (imm & 0xFFFF)
#define STR(x) #x

#define GPREG_ZR 0
#define GPREG_AT 1
#define GPREG_V0 2
#define GPREG_V1 3
#define GPREG_A0 4
#define GPREG_A1 5
#define GPREG_A2 6
#define GPREG_A3 7
#define GPREG_T0 8
#define GPREG_T1 9
#define GPREG_T2 10
#define GPREG_T3 11
#define GPREG_T4 12
#define GPREG_T5 13
#define GPREG_T6 14
#define GPREG_T7 15
#define GPREG_S0 16
#define GPREG_S1 17
#define GPREG_S2 18
#define GPREG_S3 19
#define GPREG_S4 20
#define GPREG_S5 21
#define GPREG_S6 22
#define GPREG_S7 23
#define GPREG_T8 24
#define GPREG_T9 25
#define GPREG_K0 26
#define GPREG_K1 27
#define GPREG_GP 28
#define GPREG_SP 29
#define GPREG_FP 30
#define GPREG_RA 31

#define _GET_GPREG(r)                                                                              \
    ({                                                                                             \
        int res;                                                                                   \
        __asm__ __volatile__(".set push"                                                           \
                             "\n"                                                                  \
                             ".set noreorder"                                                      \
                             "\n"                                                                  \
                             ".frame $sp, 0, $31"                                                  \
                             "\n"                                                                  \
                             "move %0,$%1"                                                         \
                             "\n"                                                                  \
                             ".set pop"                                                            \
                             "\n"                                                                  \
                             : "=r"(res)                                                           \
                             : "i"(r)                                                              \
                             : "memory");                                                          \
        res;                                                                                       \
    })

#define _SET_GPREG(reg, val)                                                                       \
    ({                                                                                             \
        __asm__ __volatile__(".set push"                                                           \
                             "\n"                                                                  \
                             ".set noreorder"                                                      \
                             "\n"                                                                  \
                             ".frame $sp, 0, $31"                                                  \
                             "\n"                                                                  \
                             "move $" STR(reg) ", %0"                                              \
                                               "\n"                                                \
                                               ".set pop"                                          \
                                               "\n"                                                \
                             :                                                                     \
                             : "r"(val));                                                          \
    })

#define SET_K1_SLL11                                                                               \
    u32 old_k1 = _GET_GPREG(27);                                                                   \
    u32 k1 = old_k1 << 11;                                                                         \
    _SET_GPREG(27, k1);
#define RESET_K1 _SET_GPREG(27, old_k1);

#define REG32(ADDR) (*(vu32*)(ADDR))

// PSP model types
#define PSP_MODEL_PHAT (0)
#define PSP_MODEL_SLIM (1)
#define PSP_MODEL_BRITE (2)
#define PSP_MODEL_BRITE4G (3)
#define PSP_MODEL_PSPGO (4)
#define PSP_MODEL_BRITE7G (6)
#define PSP_MODEL_BRITE9G (8)
#define PSP_MODEL_STREET (10)

#endif // PSPMACRO_H_
