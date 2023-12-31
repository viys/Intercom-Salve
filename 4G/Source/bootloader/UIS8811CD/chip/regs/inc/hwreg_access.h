/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _HWREG_ACCESS_H_
#define _HWREG_ACCESS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXP2(n) (1 << (n))

#define REG_BIT(n) (1U << (n))
#define REG_BIT0 (1U << 0)
#define REG_BIT1 (1U << 1)
#define REG_BIT2 (1U << 2)
#define REG_BIT3 (1U << 3)
#define REG_BIT4 (1U << 4)
#define REG_BIT5 (1U << 5)
#define REG_BIT6 (1U << 6)
#define REG_BIT7 (1U << 7)
#define REG_BIT8 (1U << 8)
#define REG_BIT9 (1U << 9)
#define REG_BIT10 (1U << 10)
#define REG_BIT11 (1U << 11)
#define REG_BIT12 (1U << 12)
#define REG_BIT13 (1U << 13)
#define REG_BIT14 (1U << 14)
#define REG_BIT15 (1U << 15)
#define REG_BIT16 (1U << 16)
#define REG_BIT17 (1U << 17)
#define REG_BIT18 (1U << 18)
#define REG_BIT19 (1U << 19)
#define REG_BIT20 (1U << 20)
#define REG_BIT21 (1U << 21)
#define REG_BIT22 (1U << 22)
#define REG_BIT23 (1U << 23)
#define REG_BIT24 (1U << 24)
#define REG_BIT25 (1U << 25)
#define REG_BIT26 (1U << 26)
#define REG_BIT27 (1U << 27)
#define REG_BIT28 (1U << 28)
#define REG_BIT29 (1U << 29)
#define REG_BIT30 (1U << 30)
#define REG_BIT31 (1U << 31)

#define HAL_SYSIRQ_NUM(irq) (irq)
#define REG_ADDRESS_FOR_ARM
#define REG_ACCESS_ADDRESS(addr) (addr)


// Range in [start, end]
#define HWP_ADDRESS_RANGE(hwp) (uint32_t)(hwp), (uint32_t)(hwp) + sizeof(*(hwp)) - 1

/**
 * \brief write register fields by register type
 *
 * All other fields not listed are write to 0. Now, at most 9 fields are supported.
 * The return value is the value to be written to register.
 *
 * Example:
 * \code{.cpp}
 * unsigned val = REGT_FIELD_WRITE(hwp_sysCtrl->sel_clock, REG_SYS_CTRL_SEL_CLOCK_T,
 *                                 sys_sel_fast, 1, soft_sel_spiflash, 1);
 * \endcode
 */
#define REGT_FIELD_WRITE(reg, type, ...) __REGT_FIELD_WRITE(reg, type, ##__VA_ARGS__)

/**
 * \brief change register fields by register type
 *
 * Similar to \p REGT_FIELD_WRITE, just all other fields not listed will be kept.
 * Inside, it is read-modify-write.
 */
#define REGT_FIELD_CHANGE(reg, type, ...) __REGT_FIELD_CHANGE(reg, type, ##__VA_ARGS__)

/**
 * \brief write ADI register fields by register type
 *
 * Similar to \p REGT_FIELD_WRITE, just the register is ADI register.
 */
#define REGT_ADI_FIELD_WRITE(reg, type, ...) __REGT_ADI_FIELD_WRITE(reg, type, ##__VA_ARGS__)

/**
 * \brief change ADI register fields by register type
 *
 * Similar to \p REGT_FIELD_CHANGE, just the register is ADI register.
 */
#define REGT_ADI_FIELD_CHANGE(reg, type, ...) __REGT_ADI_FIELD_CHANGE(reg, type, ##__VA_ARGS__)

/**
 * \brief write register fields by a variable
 *
 * Similar to \p REGT_FIELD_WRITE, just the second parameter is a variable with
 * type of the register.
 *
 * Example:
 * \code{.cpp}
 * REG_SYS_CTRL_SEL_CLOCK_T sel_clock;
 * unsigned val = REGV_FIELD_WRITE(hwp_sysCtrl->sel_clock, sel_clock,
 *                                sys_sel_fast, 1, soft_sel_spiflash, 1);
 * \endcode
 */
#define REGV_FIELD_WRITE(reg, var, ...) __REGV_FIELD_WRITE(reg, var, ##__VA_ARGS__)

/**
 * \brief change register fields by a variable
 *
 * Similar to \p REGT_FIELD_CHANGE, just the second parameter is a variable with
 * type of the register.
 */
#define REGV_FIELD_CHANGE(reg, var, ...) __REGV_FIELD_CHANGE(reg, var, ##__VA_ARGS__)

/**
 * \brief write ADI register fields by a variable
 *
 * Similar to \p REGT_ADI_FIELD_WRITE, just the second parameter is a variable with
 * type of the register.
 */
#define REGV_ADI_FIELD_WRITE(reg, var, ...) __REGV_ADI_FIELD_WRITE(reg, var, ##__VA_ARGS__)

/**
 * \brief change ADI register fields by a variable
 *
 * Similar to \p REGT_ADI_FIELD_CHANGE, just the second parameter is a variable with
 * type of the register.
 */
#define REGV_ADI_FIELD_CHANGE(reg, var, ...) __REGV_ADI_FIELD_CHANGE(reg, var, ##__VA_ARGS__)

/**
 * \brief loop wait register field to meet a condition by register type
 *
 * Example:
 * \code{.cpp}
 * REGT_WAIT_FIELD_NEZ(hwp_sysCtrl->sel_clock, REG_SYS_CTRL_SEL_CLOCK_T, pll_locked);
 * \endcode
 */
#define REGT_WAIT_FIELD_EQ(reg, type, field, expected) _REGT_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_EQ)
#define REGT_WAIT_FIELD_NE(reg, type, field, expected) _REGT_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_NE)
#define REGT_WAIT_FIELD_GT(reg, type, field, expected) _REGT_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_GT)
#define REGT_WAIT_FIELD_GE(reg, type, field, expected) _REGT_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_GE)
#define REGT_WAIT_FIELD_LT(reg, type, field, expected) _REGT_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_LT)
#define REGT_WAIT_FIELD_LE(reg, type, field, expected) _REGT_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_LE)
#define REGT_WAIT_FIELD_EQZ(reg, type, field) REGT_WAIT_FIELD_EQ(reg, type, field, 0)
#define REGT_WAIT_FIELD_NEZ(reg, type, field) REGT_WAIT_FIELD_NE(reg, type, field, 0)

/**
 * \brief loop wait ADI register field to meet a condition by register type
 */
#define REGT_ADI_WAIT_FIELD_EQ(reg, type, field, expected) _REGT_ADI_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_EQ)
#define REGT_ADI_WAIT_FIELD_NE(reg, type, field, expected) _REGT_ADI_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_NE)
#define REGT_ADI_WAIT_FIELD_GT(reg, type, field, expected) _REGT_ADI_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_GT)
#define REGT_ADI_WAIT_FIELD_GE(reg, type, field, expected) _REGT_ADI_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_GE)
#define REGT_ADI_WAIT_FIELD_LT(reg, type, field, expected) _REGT_ADI_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_LT)
#define REGT_ADI_WAIT_FIELD_LE(reg, type, field, expected) _REGT_ADI_WAIT_FIELD(reg, type, field, expected, _REG_WAIT_OP_LE)
#define REGT_ADI_WAIT_FIELD_EQZ(reg, type, field) REGT_ADI_WAIT_FIELD_EQ(reg, type, field, 0)
#define REGT_ADI_WAIT_FIELD_NEZ(reg, type, field) REGT_ADI_WAIT_FIELD_NE(reg, type, field, 0)

/**
 * \brief loop wait register field to meet a condition by a variable
 *
 * Example:
 * \code{.cpp}
 * REG_SYS_CTRL_SEL_CLOCK_T sel_clock;
 * REG_WAIT_FIELD_NEZ(sel_clock, hwp_sysCtrl->sel_clock, pll_locked);
 * \endcode
 */
#define REGV_WAIT_FIELD_EQ(reg, var, field, expected) _REGV_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_EQ)
#define REGV_WAIT_FIELD_NE(reg, var, field, expected) _REGV_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_NE)
#define REGV_WAIT_FIELD_GT(reg, var, field, expected) _REGV_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_GT)
#define REGV_WAIT_FIELD_GE(reg, var, field, expected) _REGV_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_GE)
#define REGV_WAIT_FIELD_LT(reg, var, field, expected) _REGV_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_LT)
#define REGV_WAIT_FIELD_LE(reg, var, field, expected) _REGV_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_LE)
#define REGV_WAIT_FIELD_EQZ(reg, var, field) REGV_WAIT_FIELD_EQ(reg, var, field, 0)
#define REGV_WAIT_FIELD_NEZ(reg, var, field) REGV_WAIT_FIELD_NE(reg, var, field, 0)

/**
 * \brief loop wait ADI register field to meet a condition by a variable
 */
#define REGV_ADI_WAIT_FIELD_EQ(reg, var, field, expected) _REGV_ADI_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_EQ)
#define REGV_ADI_WAIT_FIELD_NE(reg, var, field, expected) _REGV_ADI_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_NE)
#define REGV_ADI_WAIT_FIELD_GT(reg, var, field, expected) _REGV_ADI_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_GT)
#define REGV_ADI_WAIT_FIELD_GE(reg, var, field, expected) _REGV_ADI_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_GE)
#define REGV_ADI_WAIT_FIELD_LT(reg, var, field, expected) _REGV_ADI_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_LT)
#define REGV_ADI_WAIT_FIELD_LE(reg, var, field, expected) _REGV_ADI_WAIT_FIELD(reg, var, field, expected, _REG_WAIT_OP_LE)
#define REGV_ADI_WAIT_FIELD_EQZ(reg, var, field) REGV_ADI_WAIT_FIELD_EQ(reg, var, field, 0)
#define REGV_ADI_WAIT_FIELD_NEZ(reg, var, field) REGV_ADI_WAIT_FIELD_NE(reg, var, field, 0)

// ============================================================================
// Implementation
// ============================================================================

#define _REGT_WAIT_FIELD(reg, type, field, expected, op) \
    do                                                   \
    {                                                    \
        type var;                                        \
        for (;;)                                         \
        {                                                \
            (var).v = (reg);                             \
            if (op((var).b.field, (expected)))           \
                break;                                   \
        }                                                \
    } while (0)

#define _REGT_ADI_WAIT_FIELD(reg, type, field, expected, op) \
    do                                                       \
    {                                                        \
        type var;                                            \
        for (;;)                                             \
        {                                                    \
            (var).v = bootAdiBusRead(&(reg));                 \
            if (op((var).b.field, (expected)))               \
                break;                                       \
        }                                                    \
    } while (0)

#define _REGV_WAIT_FIELD(reg, var, field, expected, op) \
    do                                                  \
    {                                                   \
        for (;;)                                        \
        {                                               \
            (var).v = (reg);                            \
            if (op((var).b.field, (expected)))          \
                break;                                  \
        }                                               \
    } while (0)

#define _REGV_ADI_WAIT_FIELD(reg, var, field, expected, op) \
    do                                                      \
    {                                                       \
        for (;;)                                            \
        {                                                   \
            (var).v = bootAdiBusRead(&(reg));                \
            if (op((var).b.field, (expected)))              \
                break;                                      \
        }                                                   \
    } while (0)

#define _REG_WAIT_OP_EQ(a, b) ((a) == (b))
#define _REG_WAIT_OP_NE(a, b) ((a) != (b))
#define _REG_WAIT_OP_GT(a, b) ((a) > (b))
#define _REG_WAIT_OP_GE(a, b) ((a) >= (b))
#define _REG_WAIT_OP_LT(a, b) ((a) < (b))
#define _REG_WAIT_OP_LE(a, b) ((a) <= (b))

#define __REGT_FIELD_WRITE(reg, type, ...) ({ \
    type _val = {};                           \
    __REGV_SET(_val, __VA_ARGS__);            \
    reg = _val.v;                             \
    _val.v;                                   \
})

#define __REGT_FIELD_CHANGE(reg, type, ...) ({ \
    type _val = {.v = reg};                    \
    __REGV_SET(_val, __VA_ARGS__);             \
    reg = _val.v;                              \
    _val.v;                                    \
})

#define __REGT_ADI_FIELD_WRITE(reg, type, ...) ({ \
    type _val = {};                               \
    __REGV_SET(_val, __VA_ARGS__);                \
    bootAdiBusWrite(&(reg), _val.v);               \
    _val.v;                                       \
})

#define __REGT_ADI_FIELD_CHANGE(reg, type, ...) ({ \
    type _val = {};                                \
    __REGV_SET(_val, __VA_ARGS__);                 \
    type _maskoff = {0xffffffff};                  \
    __REGV_MASKOFF(_maskoff, __VA_ARGS__);         \
    bootAdiBusChange(&(reg), ~_maskoff.v, _val.v);  \
})

#define __REGV_FIELD_WRITE(reg, var, ...) ({ \
    var.v = 0;                               \
    __REGV_SET(var, __VA_ARGS__);            \
    reg = var.v;                             \
    var.v;                                   \
})

#define __REGV_FIELD_CHANGE(reg, var, ...) ({ \
    var.v = reg;                              \
    __REGV_SET(var, __VA_ARGS__);             \
    reg = var.v;                              \
    var.v;                                    \
})

#define __REGV_ADI_FIELD_WRITE(reg, var, ...) ({ \
    var.v = 0;                                   \
    __REGV_SET(var, __VA_ARGS__);                \
    bootAdiBusWrite(&(reg), var.v);               \
    var.v;                                       \
})

#define __REGV_ADI_FIELD_CHANGE(reg, var, ...) ({ \
    var.v = 0xffffffff;                           \
    __REGV_MASKOFF(var, __VA_ARGS__);             \
    unsigned _maskoff = var.v;                    \
    var.v = 0;                                    \
    __REGV_SET(var, __VA_ARGS__);                 \
    unsigned _val = var.v;                        \
    bootAdiBusChange(&(reg), ~_maskoff, _val);     \
})

#define __REGV_SET_IMP2(count, ...) __REGV_SET_##count(__VA_ARGS__)
#define __REGV_SET_IMP1(count, ...) __REGV_SET_IMP2(count, __VA_ARGS__)
#define __REGV_SET(...) __REGV_SET_IMP1(BOOT_VA_NARGS(__VA_ARGS__), __VA_ARGS__)

#define __REGV_MASKOFF_IMP2(count, ...) __REGV_MASKOFF_##count(__VA_ARGS__)
#define __REGV_MASKOFF_IMP1(count, ...) __REGV_MASKOFF_IMP2(count, __VA_ARGS__)
#define __REGV_MASKOFF(...) __REGV_MASKOFF_IMP1(BOOT_VA_NARGS(__VA_ARGS__), __VA_ARGS__)

#define __REGV_SET_3(v, f1, v1) ({ v.b.f1 = v1; })
#define __REGV_SET_5(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_3(v, __VA_ARGS__); })
#define __REGV_SET_7(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_5(v, __VA_ARGS__); })
#define __REGV_SET_9(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_7(v, __VA_ARGS__); })
#define __REGV_SET_11(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_9(v, __VA_ARGS__); })
#define __REGV_SET_13(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_11(v, __VA_ARGS__); })
#define __REGV_SET_15(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_13(v, __VA_ARGS__); })
#define __REGV_SET_17(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_15(v, __VA_ARGS__); })
#define __REGV_SET_19(v, f1, v1, ...) ({ v.b.f1 = v1; __REGV_SET_17(v, __VA_ARGS__); })

#define __REGV_MASKOFF_3(v, f1, v1) ({ v.b.f1 = 0; })
#define __REGV_MASKOFF_5(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_3(v, __VA_ARGS__); })
#define __REGV_MASKOFF_7(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_5(v, __VA_ARGS__); })
#define __REGV_MASKOFF_9(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_7(v, __VA_ARGS__); })
#define __REGV_MASKOFF_11(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_9(v, __VA_ARGS__); })
#define __REGV_MASKOFF_13(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_11(v, __VA_ARGS__); })
#define __REGV_MASKOFF_15(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_13(v, __VA_ARGS__); })
#define __REGV_MASKOFF_17(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_15(v, __VA_ARGS__); })
#define __REGV_MASKOFF_19(v, f1, v1, ...) ({ v.b.f1 = 0; __REGV_MASKOFF_17(v, __VA_ARGS__); })

// ============================================================================
// Oboleted
// ============================================================================

// hardware registers are defined as union. The template of union is
//      union {
//          uint32_t v;
//          struct {
//              uint32_t f1 : 1;
//              uint32_t f2 : 2;
//              uint32_t f3 : 3;
//              ......
//          } b;
//      };
//
// The following macros are helpers for bit field operations:
// * REG_FIELD_GET: read the register, and return the value of specified field.
// * REG_WAIT_FIELD_EQ/NE/GT/GE/LT/LE/EQZ/NEZ: wait until the register fields
//   meet the condition.
// * REG_FIELD_MASK: return a mask (1 for the specified fields) of one or more
//   fields. In the above example, mask for f1 and f3 is 0x00000039
// * REG_FIELD_VALUE: return a value with specified fields, other fields are 0.
// * REG_FIELD_MASKVAL: return "mask, value"
// * REG_FIELD_CHANGE: change the specified fields, and other fields are
//   untouched. The changed value is returned
// * REG_FIELD_WRITE: write register with specified fields, and other fields
//   are 0, return the value to be written to register

#define REGTYPE_FIELD_GET(type, var, f1) ({ type _var = {(var)}; _var.b.f1; })
#define REGTYPE_FIELD_VAL(type, f1, v1) ({ type _var = {.b={.f1=(v1)}}; _var.v; })
#define REGTYPE_FIELD_MASK(type, f1) ({ type _var = {0xffffffff}; _var.b.f1 = 0; ~_var.v; })

#define REG_FIELD_GET(r, var, f) ({ var.v = r; var.b.f; })

#define REG_WAIT_FIELD_EQ(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_EQ)
#define REG_WAIT_FIELD_NE(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_NE)
#define REG_WAIT_FIELD_GT(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_GT)
#define REG_WAIT_FIELD_GE(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_GE)
#define REG_WAIT_FIELD_LT(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_LT)
#define REG_WAIT_FIELD_LE(var, reg, field, expected) _REG_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_LE)
#define REG_WAIT_FIELD_EQZ(var, reg, field) REG_WAIT_FIELD_EQ(var, reg, field, 0)
#define REG_WAIT_FIELD_NEZ(var, reg, field) REG_WAIT_FIELD_NE(var, reg, field, 0)

#define REG_WAIT_COND(cond) \
    do                      \
    {                       \
        while (!(cond))     \
            ;               \
    } while (0)

#define REG_FIELD_MASK1(var, f1) ({ var.v = 0xffffffff; var.b.f1 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK2(var, f1, f2) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK3(var, f1, f2, f3) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.b.f3 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK4(var, f1, f2, f3, f4) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.b.f3 = 0; var.b.f4 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK5(var, f1, f2, f3, f4, f5) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.b.f3 = 0; var.b.f4 = 0; var.b.f5 = 0; var.v = ~var.v; var.v; })
#define REG_FIELD_MASK6(var, f1, f2, f3, f4, f5, f6) ({ var.v = 0xffffffff; var.b.f1 = 0; var.b.f2 = 0; var.b.f3 = 0; var.b.f4 = 0; var.b.f5 = 0; var.b.f6 = 0; var.v = ~var.v; var.v; })

#define REG_FIELD_VALUE1(var, f1, v1) ({ var.v = 0; var.b.f1 = v1; var.v; })
#define REG_FIELD_VALUE2(var, f1, v1, f2, v2) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.v; })
#define REG_FIELD_VALUE3(var, f1, v1, f2, v2, f3, v3) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.v; })
#define REG_FIELD_VALUE4(var, f1, v1, f2, v2, f3, v3, f4, v4) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.v; })
#define REG_FIELD_VALUE5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; var.v; })
#define REG_FIELD_VALUE6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; var.b.f6 = v6; var.v; })

#define REG_FIELD_MASKVAL1(var, f1, v1) REG_FIELD_MASK1(var, f1), REG_FIELD_VALUE1(var, f1, v1)
#define REG_FIELD_MASKVAL2(var, f1, v1, f2, v2) REG_FIELD_MASK2(var, f1, f2), REG_FIELD_VALUE2(var, f1, v1, f2, v2)
#define REG_FIELD_MASKVAL3(var, f1, v1, f2, v2, f3, v3) REG_FIELD_MASK3(var, f1, f2, f3), REG_FIELD_VALUE3(var, f1, v1, f2, v2, f3, v3)
#define REG_FIELD_MASKVAL4(var, f1, v1, f2, v2, f3, v3, f4, v4) REG_FIELD_MASK4(var, f1, f2, f3, f4), REG_FIELD_VALUE4(var, f1, v1, f2, v2, f3, v3, f4, v4)
#define REG_FIELD_MASKVAL5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) REG_FIELD_MASK5(var, f1, f2, f3, f4, f5), REG_FIELD_VALUE5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5)
#define REG_FIELD_MASKVAL6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) REG_FIELD_MASK6(var, f1, f2, f3, f4, f5, f6), REG_FIELD_VALUE6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6)

#define REG_FIELD_CHANGE1(r, var, f1, v1) ({ var.v = r; var.b.f1 = v1; r = var.v; var.v; })
#define REG_FIELD_CHANGE2(r, var, f1, v1, f2, v2) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; r = var.v; var.v; })
#define REG_FIELD_CHANGE3(r, var, f1, v1, f2, v2, f3, v3) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; r = var.v; var.v; })
#define REG_FIELD_CHANGE4(r, var, f1, v1, f2, v2, f3, v3, f4, v4) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; r = var.v; var.v; })
#define REG_FIELD_CHANGE5(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; r = var.v; var.v; })
#define REG_FIELD_CHANGE6(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) ({ var.v = r; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; var.b.f6 = v6; r = var.v; var.v; })

#define REG_FIELD_WRITE1(r, var, f1, v1) ({ var.v = 0; var.b.f1 = v1; r = var.v; var.v; })
#define REG_FIELD_WRITE2(r, var, f1, v1, f2, v2) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; r = var.v; var.v; })
#define REG_FIELD_WRITE3(r, var, f1, v1, f2, v2, f3, v3) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; r = var.v; var.v; })
#define REG_FIELD_WRITE4(r, var, f1, v1, f2, v2, f3, v3, f4, v4) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; r = var.v; var.v; })
#define REG_FIELD_WRITE5(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; r = var.v; var.v; })
#define REG_FIELD_WRITE6(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) ({ var.v = 0; var.b.f1 = v1; var.b.f2 = v2; var.b.f3 = v3; var.b.f4 = v4; var.b.f5 = v5; var.b.f6 = v6; r = var.v; var.v; })

#define REG_ADI_CHANGE1(r, var, f1, v1) bootAdiBusChange(&(r), REG_FIELD_MASKVAL1(var, f1, v1))
#define REG_ADI_CHANGE2(r, var, f1, v1, f2, v2) bootAdiBusChange(&(r), REG_FIELD_MASKVAL2(var, f1, v1, f2, v2))
#define REG_ADI_CHANGE3(r, var, f1, v1, f2, v2, f3, v3) bootAdiBusChange(&(r), REG_FIELD_MASKVAL3(var, f1, v1, f2, v2, f3, v3))
#define REG_ADI_CHANGE4(r, var, f1, v1, f2, v2, f3, v3, f4, v4) bootAdiBusChange(&(r), REG_FIELD_MASKVAL4(var, f1, v1, f2, v2, f3, v3, f4, v4))
#define REG_ADI_CHANGE5(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) bootAdiBusChange(&(r), REG_FIELD_MASKVAL5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5))
#define REG_ADI_CHANGE6(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) bootAdiBusChange(&(r), REG_FIELD_MASKVAL6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6))

#define REG_ADI_WRITE1(r, var, f1, v1) bootAdiBusWrite(&(r), REG_FIELD_VALUE1(var, f1, v1))
#define REG_ADI_WRITE2(r, var, f1, v1, f2, v2) bootAdiBusWrite(&(r), REG_FIELD_VALUE2(var, f1, v1, f2, v2))
#define REG_ADI_WRITE3(r, var, f1, v1, f2, v2, f3, v3) bootAdiBusWrite(&(r), REG_FIELD_VALUE3(var, f1, v1, f2, v2, f3, v3))
#define REG_ADI_WRITE4(r, var, f1, v1, f2, v2, f3, v3, f4, v4) bootAdiBusWrite(&(r), REG_FIELD_VALUE4(var, f1, v1, f2, v2, f3, v3, f4, v4))
#define REG_ADI_WRITE5(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5) bootAdiBusWrite(&(r), REG_FIELD_VALUE5(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5))
#define REG_ADI_WRITE6(r, var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6) bootAdiBusWrite(&(r), REG_FIELD_VALUE6(var, f1, v1, f2, v2, f3, v3, f4, v4, f5, v5, f6, v6))

#define REG_ADI_WAIT_FIELD_EQ(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_EQ)
#define REG_ADI_WAIT_FIELD_NE(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_NE)
#define REG_ADI_WAIT_FIELD_GT(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_GT)
#define REG_ADI_WAIT_FIELD_GE(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_GE)
#define REG_ADI_WAIT_FIELD_LT(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_LT)
#define REG_ADI_WAIT_FIELD_LE(var, reg, field, expected) _REG_ADI_WAIT_FIELD(var, reg, field, expected, _REG_WAIT_OP_LE)
#define REG_ADI_WAIT_FIELD_EQZ(var, reg, field) REG_ADI_WAIT_FIELD_EQ(var, reg, field, 0)
#define REG_ADI_WAIT_FIELD_NEZ(var, reg, field) REG_ADI_WAIT_FIELD_NE(var, reg, field, 0)

#define _REG_WAIT_FIELD(var, reg, field, expected, op) \
    do                                                 \
    {                                                  \
        for (;;)                                       \
        {                                              \
            (var).v = (reg);                           \
            if (op((var).b.field, (expected)))         \
                break;                                 \
        }                                              \
    } while (0)

#define _REG_ADI_WAIT_FIELD(var, reg, field, expected, op) \
    do                                                     \
    {                                                      \
        for (;;)                                           \
        {                                                  \
            (var).v = bootAdiBusRead(&(reg));               \
            if (op((var).b.field, (expected)))             \
                break;                                     \
        }                                                  \
    } while (0)

#ifdef __cplusplus
}
#endif
#endif
