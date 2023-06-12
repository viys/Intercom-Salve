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

#ifndef _BOOT_COMPILER_H_
#define _BOOT_COMPILER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// macros for alias, either strong or weak. It can't work with LTO.
#define BOOT_STRONG_ALIAS(alias, sym) __asm(".global " #alias "\n" #alias " = " #sym)
#define BOOT_WEAK_ALIAS(alias, sym) __asm(".weak " #alias "\n" #alias " = " #sym)

// macros for alias, either strong or weak,
#define BOOT_DECL_STRONG_ALIAS(oldname, newfunc) newfunc __attribute__((alias(#oldname)))
#define BOOT_DECL_WEAK_ALIAS(oldname, newfunc) newfunc __attribute__((weak, alias(#oldname)))

#define BOOT_STRINGFY(s) _BOOT_STRINGFY(s)
#define _BOOT_STRINGFY(s) #s

// macro for nop instruction
#define BOOT_NOP asm volatile("nop")

// macro for compiler memory access barrier
#define BOOT_BARRIER() asm volatile("" :: \
                                       : "memory")

// macro for alignment attribute
#define BOOT_CACHE_LINE_ALIGNED __attribute__((aligned(CONFIG_CACHE_LINE_SIZE)))
#define BOOT_ALIGNED(n) __attribute__((aligned(n)))

#define BOOT_ATTRIBUTE_ISR __attribute__((interrupt))
#define BOOT_ATTRIBUTE_USED __attribute__((used))
#define BOOT_ATTRIBUTE_OPTIMIZE(n) __attribute__((optimize(n)))

// macros for "known" sections
#define BOOT_SECTION(sect) __attribute__((section(#sect)))

// macros for attributes
#define BOOT_WEAK __attribute__((weak))
#define BOOT_USED __attribute__((used))
#define BOOT_UNUSED __attribute__((unused))
#define BOOT_NO_RETURN __attribute__((__noreturn__))
#define BOOT_NO_INLINE __attribute__((noinline))
#define BOOT_FORCE_INLINE __attribute__((always_inline)) inline

#if __mips__
#define BOOT_NO_MIPS16 __attribute__((nomips16))
#define BOOT_NAKED
#endif

#if __arm__
#define BOOT_NO_MIPS16
#define BOOT_NAKED __attribute__((naked))
#define BOOT_DMB() asm volatile("dmb 0xf" :: \
                                   : "memory")
#define BOOT_DSB() asm volatile("dsb 0xf" :: \
                                   : "memory")
#define BOOT_ISB() asm volatile("isb 0xf" :: \
                                   : "memory")
#endif

// macro maybe helpful for compiler optimization
#define BOOT_LIKELY(x) __builtin_expect(!!(x), 1)
#define BOOT_UNLIKELY(x) __builtin_expect(!!(x), 0)

// macros for MIPS KSEG0/1
#if __mips__
#define BOOT_KSEG0(addr) (((unsigned long)(addr)&0x1fffffff) | 0x80000000)
#define BOOT_KSEG1(addr) (((unsigned long)(addr)&0x1fffffff) | 0xa0000000)
#define BOOT_IS_KSEG0(addr) (((unsigned long)(addr)&0xe0000000) == 0x80000000)
#define BOOT_IS_KSEG1(addr) (((unsigned long)(addr)&0xe0000000) == 0xa0000000)
#define BOOT_KSEG01_PHY_ADDR(addr) ((unsigned long)(addr)&0x0FFFFFFF)
#endif

typedef struct
{
    uint32_t data[16 / 4];
} bootBits128_t, bootBytes16_t;

typedef struct
{
    uint32_t data[32 / 4];
} bootBits256_t, bootBytes32_t;

typedef struct
{
    uint32_t data[64 / 4];
} bootBits512_t, bootBytes64_t;

typedef struct
{
    uint32_t data[80 / 4];
} bootBits640_t, bootBytes80_t;

typedef struct
{
    uint32_t data[512 / 4];
} bootBits4096_t, bootBytes512_t;

/**
 * \brief tiny helper for buffer pointer and size
 */
typedef struct
{
    intptr_t ptr; ///< buffer pointer
    unsigned size; ///< buffer size
} bootBuffer_t;

/**
 * \brief contiguous buffer, with size as first word
 */
typedef struct
{
    uint32_t size;    ///< size the following data
    uint32_t data[0]; ///< data
} bootBufSize32_t;

/**
 * \brief contiguous buffer, with size as first short
 */
typedef struct
{
    uint16_t size;    ///< size the following data
    uint16_t data[0]; ///< data
} bootBufSize16_t;

/**
 * \brief contiguous buffer, with size as first byte
 */
typedef struct
{
    uint8_t size;    ///< size the following data
    uint8_t data[0]; ///< data
} bootBufSize8_t;

/**
 * data structure to define an unsigned integer range
 */
typedef struct
{
    uint32_t minval; ///< minimal value
    uint32_t maxval; ///< maximum value
} bootUintRange_t;

/**
 * data structure to define a signed integer range
 */
typedef struct
{
    int minval; ///< minimal value
    int maxval; ///< maximum value
} bootIntRange_t;

/**
 * data structure to define a unsigned 64bits integer range
 */
typedef struct
{
    uint64_t minval; ///< minimal value
    uint64_t maxval; ///< maximum value
} bootUint64Range_t;

/**
 * data structure to define a signed 64bits integer range
 */
typedef struct
{
    int64_t minval; ///< minimal value
    int64_t maxval; ///< maximum value
} bootInt64Range_t;

// do { ... } while (0) is common trick to avoid if/else error
#define BOOT_DO_WHILE0(expr) \
    do                      \
    {                       \
        expr                \
    } while (0)

#define BOOT_OFFSETOF(type, member) __builtin_offsetof(type, member)

// just a dead loop
#define BOOT_DEAD_LOOP BOOT_DO_WHILE0(for (;;);)

// Busy loop wait until condition is true
#define BOOT_LOOP_WAIT(cond) BOOT_DO_WHILE0(while (!(cond));)

// Busy loop wait util condition is true. When polling peripherals, it is
// needed to avoid read peripheral registers without delay, especially
// when the peripheral is connected to a slow bus. This may cause the bus
// is busy to react CPU register read, and other operations are affected.
#define BOOT_POLL_WAIT(cond) BOOT_DO_WHILE0(while (!(cond)) { BOOT_NOP; BOOT_NOP; BOOT_NOP; BOOT_NOP; })

// Loop wait. Return true if cont_true is meet, return false if cond_false is meet.
#define BOOT_LOOP_WAIT_IF(cond_true, cond_false) \
    ({                                          \
        bool _waited;                           \
        for (;;)                                \
        {                                       \
            if (cond_true)                      \
            {                                   \
                _waited = true;                 \
                break;                          \
            }                                   \
            if (cond_false)                     \
            {                                   \
                _waited = false;                \
                break;                          \
            }                                   \
        }                                       \
        _waited;                                \
    })

// Loop wait with timeout
#define BOOT_LOOP_WAIT_TIMEOUT_US(cond, us)            \
    ({                                                \
        bool _waited = false;                         \
        unsigned _us = (us);                          \
        uint32_t _timeout;                   \
        bootElapsedTimerStart(&_timeout);              \
        do                                            \
        {                                             \
            if (cond)                                 \
            {                                         \
                _waited = true;                       \
                break;                                \
            }                                         \
        } while (bootElapsedTimeUS(&_timeout) <= _us); \
        _waited;                                      \
    })

// Loop wait with timeout, also post will be executed for each loop
#define BOOT_LOOP_WAIT_POST_TIMEOUT_US(cond, us, post) \
    ({                                                \
        bool _waited = false;                         \
        unsigned _us = (us);                          \
        uint32_t _timeout;                   \
        bootElapsedTimerStart(&_timeout);              \
        for (;;)                                      \
        {                                             \
            if (cond)                                 \
            {                                         \
                _waited = true;                       \
                break;                                \
            }                                         \
            if (bootElapsedTimeUS(&_timeout) > _us)    \
            {                                         \
                _waited = false;                      \
                break;                                \
            }                                         \
            (void)(post);                             \
        }                                             \
        _waited;                                      \
    })

// macro for fourcc tag
#define BOOT_MAKE_TAG(a, b, c, d) ((unsigned)(a) | ((unsigned)(b) << 8) | ((unsigned)(c) << 16) | ((unsigned)(d) << 24))

// macro for array dimension
#define BOOT_ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

// macros for 2^n, and 2^n alignment
#define BOOT_IS_POW2(v) (((v) & ((v)-1)) == 0)
#define BOOT_IS_ALIGNED(v, n) (((unsigned long)(v) & ((n)-1)) == 0)
#define BOOT_ALIGN_UP(v, n) (((unsigned long)(v) + (n)-1) & ~((n)-1))
#define BOOT_ALIGN_DOWN(v, n) ((unsigned long)(v) & ~((n)-1))
#define BOOT_DIV_ROUND(m, n) (((m) + ((n) >> 1)) / (n))
#define BOOT_DIV_ROUND_UP(n, m) (((n) + (m)-1) / (m))

// macro for compare two chars ignoring case
#define BOOT_CHAR_CASE_EQU(a, b) (((a) | 0x20) == ((b) | 0x20))

// macro to increase the pointer, and return the original pointer
#define BOOT_PTR_INCR_POST(p, n) ({uintptr_t _orig = (p); (p) += (n); _orig; })

// pointer (signed) diff, either can be any pointer type
#define BOOT_PTR_DIFF(a, b) ((intptr_t)(a) - (intptr_t)(b))

// Macro for variadic argument count
#define BOOT_VA_NARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, N, ...) N
#define BOOT_VA_NARGS(...) BOOT_VA_NARGS_IMPL(0, ##__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

// macros for declaration and definition
#define BOOT_DEF_CONST_VAR(decl, ...) static const decl = __VA_ARGS__
#define BOOT_DEF_GLOBAL_VAR(decl, ...) decl = __VA_ARGS__
#define BOOT_DECL_GLOBAL_VAR(decl, ...) extern decl

// macros to convert CPU endian to little/big endian
#define BOOT_TO_LE16(v) (v)
#define BOOT_TO_LE32(v) (v)
#define BOOT_TO_BE16(v) __builtin_bswap16(v)
#define BOOT_TO_BE32(v) __builtin_bswap32(v)

// macros to convert CPU endian from little/big endian
#define BOOT_FROM_LE16(v) (v)
#define BOOT_FROM_LE32(v) (v)
#define BOOT_FROM_BE16(v) __builtin_bswap16(v)
#define BOOT_FROM_BE32(v) __builtin_bswap32(v)

// macro for 32bits/16bits/8bits register read and write
#define BOOT_REG32_WRITE(address, value) *(volatile uint32_t *)(address) = (value)
#define BOOT_REG32_READ(address) (*(volatile uint32_t *)(address))
#define BOOT_REG16_WRITE(address, value) *(volatile uint16_t *)(address) = (value)
#define BOOT_REG16_READ(address) (*(volatile uint16_t *)(address))
#define BOOT_REG8_WRITE(address, value) *(volatile uint8_t *)(address) = (value)
#define BOOT_REG8_READ(address) (*(volatile uint8_t *)(address))

// macros for easier writing
#define BOOT_KB(n) ((unsigned)(n) * (unsigned)(1024))
#define BOOT_MB(n) ((unsigned)(n) * (unsigned)(1024 * 1024))
#define BOOT_GB(n) ((unsigned)(n) * (unsigned)(1024 * 1024 * 1024))
#define BOOT_MHZ(n) ((unsigned)(n) * (unsigned)(1000 * 1000))

// macros for min, max. the variable will be accessed only once
#define BOOT_MIN(type, a, b) ({ type _a = (type)(a); type _b = (type)(b); _a < _b? _a : _b; })
#define BOOT_MAX(type, a, b) ({ type _a = (type)(a); type _b = (type)(b); _a > _b? _a : _b; })

// Range [start, end) and region [start, start+size) macros
#define BOOT_IS_IN_RANGE(type, a, start, end) ({type _a = (type)(a); type _start = (type)(start); type _end = (type)(end); _a >= _start && _a < _end; })
#define BOOT_IS_IN_REGION(type, a, start, size) ({type _a = (type)(a); type _start = (type)(start); type _end = _start + (type)(size); _a >= _start && _a < _end; })
#define BOOT_RANGE_INSIDE(type, start1, end1, start2, end2) ({type _s1 = (type)(start1), _e1 = (type)(end1), _s2 = (type)(start2), _e2 = (type)(end2);  _s1 >= _s2 && _e1 <= _e2; })
#define BOOT_REGION_INSIDE(type, start1, size1, start2, size2) ({type _s1 = (type)(start1), _e1 = _s1 + (type)(size1), _s2 = (type)(start2), _e2 = _s2 + (type)(size2);  _s1 >= _s2 && _e1 <= _e2; })

// abs(a - b) <= threshold, a little trick to avoid abs calculation or if-else
#define BOOT_ABS_DELTA_LE(a, b, threshold) ({ unsigned _t = (threshold); (unsigned)((a) - (b) + _t) <= 2 * _t; })

// macro to swap 2 variables
#define BOOT_SWAP(type, a, b) ({ type _t = (a); (a) = (b); (b) = _t; })

// macro for offsetof and container_of
#define BOOT_OFFSETOF(type, member) __builtin_offsetof(type, member)
#define BOOT_CONTAINER_OF(ptr, type, member) ((type *)((char *)(ptr)-BOOT_OFFSETOF(type, member)))

// macro for load section, symbol naming style matches linker script
#define BOOT_LOAD_SECTION(name)                 \
    do                                         \
    {                                          \
        extern uint32_t __##name##_start;      \
        extern uint32_t __##name##_end;        \
        extern uint32_t __##name##_load_start; \
        uint32_t *p;                           \
        uint32_t *l;                           \
        for (p = &__##name##_start,            \
            l = &__##name##_load_start;        \
             p < &__##name##_end;)             \
            *p++ = *l++;                       \
    } while (0)

// macro for clear section, symbol naming style matches linker script
#define BOOT_CLEAR_SECTION(name)           \
    do                                    \
    {                                     \
        extern uint32_t __##name##_start; \
        extern uint32_t __##name##_end;   \
        uint32_t *p;                      \
        for (p = &__##name##_start;       \
             p < &__##name##_end;)        \
            *p++ = 0;                     \
    } while (0)

// macros for "known" sections
#define BOOT_SECTION_SRAM_BOOT_TEXT BOOT_SECTION(.sramboottext)
#define BOOT_SECTION_SRAM_TEXT BOOT_SECTION(.sramtext)
#define BOOT_SECTION_SRAM_DATA BOOT_SECTION(.sramdata)
#define BOOT_SECTION_SRAM_BSS BOOT_SECTION(.srambss)
#define BOOT_SECTION_SRAM_UNINIT BOOT_SECTION(.sramuninit)
#define BOOT_SECTION_SRAM_UC_DATA BOOT_SECTION(.sramucdata)
#define BOOT_SECTION_SRAM_UC_BSS BOOT_SECTION(.sramucbss)
#define BOOT_SECTION_SRAM_UC_UNINIT BOOT_SECTION(.sramucuninit)
#define BOOT_SECTION_RAM_TEXT BOOT_SECTION(.ramtext)
#define BOOT_SECTION_RAM_DATA BOOT_SECTION(.ramdata)
#define BOOT_SECTION_RAM_BSS BOOT_SECTION(.rambss)
#define BOOT_SECTION_RAM_UNINIT BOOT_SECTION(.ramuninit)
#define BOOT_SECTION_RAM_UC_DATA BOOT_SECTION(.ramucdata)
#define BOOT_SECTION_RAM_UC_BSS BOOT_SECTION(.ramucbss)
#define BOOT_SECTION_RAM_UC_UNINIT BOOT_SECTION(.ramucuninit)
#define BOOT_SECTION_BOOT_TEXT BOOT_SECTION(.boottext)
#define BOOT_SECTION_RO_KEEP __attribute__((used, section(".rokeep")))
#define BOOT_SECTION_RW_KEEP __attribute__((used, section(".rwkeep")))


#ifdef __cplusplus
#define BOOT_EXTERN_C extern "C"
#define BOOT_EXTERN_C_BEGIN extern "C" {
#define BOOT_EXTERN_C_END }
#else
#define BOOT_EXTERN_C
#define BOOT_EXTERN_C_BEGIN
#define BOOT_EXTERN_C_END
#endif

#endif
