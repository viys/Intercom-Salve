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

#ifndef _GPIO1_H_
#define _GPIO1_H_

// Auto generated (v1.0-38-g167fa99). Don't edit it manually!

#define NB_GPIO1 (8)

#define REG_GPIO1_BASE (0x41802000)

typedef volatile struct
{
    uint32_t gpio_oen_val;     // 0x00000000
    uint32_t gpio_oen_set_out; // 0x00000004
    uint32_t gpio_oen_set_in;  // 0x00000008
    uint32_t gpio_val;         // 0x0000000c
    uint32_t gpio_set;         // 0x00000010
    uint32_t gpio_clr;         // 0x00000014
    uint32_t gpint_ctrl_r;     // 0x00000018
    uint32_t gpint_ctrl_f;     // 0x0000001c
    uint32_t gpint_ctrl_mode;  // 0x00000020
    uint32_t int_clr;          // 0x00000024
    uint32_t int_status;       // 0x00000028
    struct                     // 0x0000002c
    {                          //
        uint32_t db_ctrl0;     // 0x00000000
    } db_ctrl[16];             //
    uint32_t gpint_ctrl;       // 0x0000006c
} HWP_GPIO1_T;

#define hwp_gpio1 ((HWP_GPIO1_T *)REG_ACCESS_ADDRESS(REG_GPIO1_BASE))

// gpio_oen_val
typedef union {
    uint32_t v;
    struct
    {
        uint32_t oen_val : 8; // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_GPIO1_GPIO_OEN_VAL_T;

// gpio_oen_set_out
typedef union {
    uint32_t v;
    struct
    {
        uint32_t oen_set_out : 8; // [7:0], write clear
        uint32_t __31_8 : 24;     // [31:8]
    } b;
} REG_GPIO1_GPIO_OEN_SET_OUT_T;

// gpio_oen_set_in
typedef union {
    uint32_t v;
    struct
    {
        uint32_t oen_set_in : 8; // [7:0], write set
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_GPIO1_GPIO_OEN_SET_IN_T;

// gpio_val
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpio_val : 8; // [7:0]
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_GPIO1_GPIO_VAL_T;

// gpio_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpio_set : 8; // [7:0], write set
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_GPIO1_GPIO_SET_T;

// gpio_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpio_clr : 8; // [7:0], write clear
        uint32_t __31_8 : 24;  // [31:8]
    } b;
} REG_GPIO1_GPIO_CLR_T;

// gpint_ctrl_r
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpint_r : 8; // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_GPIO1_GPINT_CTRL_R_T;

// gpint_ctrl_f
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpint_f : 8; // [7:0]
        uint32_t __31_8 : 24; // [31:8]
    } b;
} REG_GPIO1_GPINT_CTRL_F_T;

// gpint_ctrl_mode
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpint_mode : 8; // [7:0]
        uint32_t __31_8 : 24;    // [31:8]
    } b;
} REG_GPIO1_GPINT_CTRL_MODE_T;

// int_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpint_clr : 8; // [7:0], write clear
        uint32_t __31_8 : 24;   // [31:8]
    } b;
} REG_GPIO1_INT_CLR_T;

// int_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpint_status : 8; // [7:0], read only
        uint32_t __31_8 : 24;      // [31:8]
    } b;
} REG_GPIO1_INT_STATUS_T;

// db_ctrl0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t db_ctrl0 : 3; // [2:0]
        uint32_t __31_3 : 29;  // [31:3]
    } b;
} REG_GPIO1_DB_CTRL0_T;

// gpint_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t negedge_logic_en : 1; // [0]
        uint32_t __31_1 : 31;          // [31:1]
    } b;
} REG_GPIO1_GPINT_CTRL_T;

// gpio_oen_val
#define GPIO1_OEN_VAL(n) (((n)&0xff) << 0)
#define GPIO1_OEN_VAL_INPUT (1 << 0)
#define GPIO1_OEN_VAL_OUTPUT (0 << 0)

#define GPIO1_OEN_VAL_V_INPUT (1)
#define GPIO1_OEN_VAL_V_OUTPUT (0)

// gpio_oen_set_out
#define GPIO1_OEN_SET_OUT(n) (((n)&0xff) << 0)

// gpio_oen_set_in
#define GPIO1_OEN_SET_IN(n) (((n)&0xff) << 0)

// gpio_val
#define GPIO1_GPIO_VAL(n) (((n)&0xff) << 0)

// gpio_set
#define GPIO1_GPIO_SET(n) (((n)&0xff) << 0)

// gpio_clr
#define GPIO1_GPIO_CLR(n) (((n)&0xff) << 0)

// gpint_ctrl_r
#define GPIO1_GPINT_R(n) (((n)&0xff) << 0)

// gpint_ctrl_f
#define GPIO1_GPINT_F(n) (((n)&0xff) << 0)

// gpint_ctrl_mode
#define GPIO1_GPINT_MODE(n) (((n)&0xff) << 0)

// int_clr
#define GPIO1_GPINT_CLR(n) (((n)&0xff) << 0)

// int_status
#define GPIO1_GPINT_STATUS(n) (((n)&0xff) << 0)

// db_ctrl0
#define GPIO1_DB_CTRL0(n) (((n)&0x7) << 0)
#define GPIO1_DB_CTRL0_DEBOUNCE_0NS (0 << 0)
#define GPIO1_DB_CTRL0_DEBOUNCE_62_5NS (1 << 0)
#define GPIO1_DB_CTRL0_DEBOUNCE_125NS (2 << 0)
#define GPIO1_DB_CTRL0_DEBOUNCE_250NS (3 << 0)
#define GPIO1_DB_CTRL0_DEBOUNCE_500NS (4 << 0)
#define GPIO1_DB_CTRL0_DEBOUNCE_1000NS (5 << 0)
#define GPIO1_DB_CTRL0_DEBOUNCE_2000NS (6 << 0)
#define GPIO1_DB_CTRL0_DEBOUNCE_4000NS (7 << 0)

#define GPIO1_DB_CTRL0_V_DEBOUNCE_0NS (0)
#define GPIO1_DB_CTRL0_V_DEBOUNCE_62_5NS (1)
#define GPIO1_DB_CTRL0_V_DEBOUNCE_125NS (2)
#define GPIO1_DB_CTRL0_V_DEBOUNCE_250NS (3)
#define GPIO1_DB_CTRL0_V_DEBOUNCE_500NS (4)
#define GPIO1_DB_CTRL0_V_DEBOUNCE_1000NS (5)
#define GPIO1_DB_CTRL0_V_DEBOUNCE_2000NS (6)
#define GPIO1_DB_CTRL0_V_DEBOUNCE_4000NS (7)

// gpint_ctrl
#define GPIO1_NEGEDGE_LOGIC_EN (1 << 0)

#endif // _GPIO1_H_
