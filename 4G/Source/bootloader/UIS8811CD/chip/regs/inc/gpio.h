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

#ifndef _GPIO_H_
#define _GPIO_H_

// Auto generated (v1.0-38-g167fa99). Don't edit it manually!

#define IDX_GPIO_DCON (0)
#define IDX_GPO_CHG (0)
#define NB_GPIO (64)
#define NB_GPIO_INT (64)
#define NB_GPO (10)

#define REG_GPIO2_BASE (0x41a10000)

typedef volatile struct
{
    uint32_t gpio_oen_val_l;     // 0x00000000
    uint32_t gpio_oen_set_out_l; // 0x00000004
    uint32_t gpio_oen_set_in_l;  // 0x00000008
    uint32_t gpio_val_l;         // 0x0000000c
    uint32_t gpio_set_l;         // 0x00000010
    uint32_t gpio_clr_l;         // 0x00000014
    uint32_t gpint_r_set_l;      // 0x00000018
    uint32_t gpint_r_set_h;      // 0x0000001c
    uint32_t int_clr_l;          // 0x00000020
    uint32_t int_status_l;       // 0x00000024
    uint32_t chg_ctrl;           // 0x00000028
    uint32_t chg_cmd;            // 0x0000002c
    uint32_t gpo_set;            // 0x00000030
    uint32_t gpo_clr;            // 0x00000034
    uint32_t gpio_oen_val_h;     // 0x00000038
    uint32_t gpio_oen_set_out_h; // 0x0000003c
    uint32_t gpio_oen_set_in_h;  // 0x00000040
    uint32_t gpio_val_h;         // 0x00000044
    uint32_t gpio_set_h;         // 0x00000048
    uint32_t gpio_clr_h;         // 0x0000004c
    uint32_t gpint_r_clr_l;      // 0x00000050
    uint32_t gpint_r_clr_h;      // 0x00000054
    uint32_t gpint_f_set_l;      // 0x00000058
    uint32_t gpint_f_set_h;      // 0x0000005c
    uint32_t gpint_f_clr_l;      // 0x00000060
    uint32_t gpint_f_clr_h;      // 0x00000064
    uint32_t gpint_dbn_en_set_l; // 0x00000068
    uint32_t gpint_dbn_en_set_h; // 0x0000006c
    uint32_t gpint_dbn_en_clr_l; // 0x00000070
    uint32_t gpint_dbn_en_clr_h; // 0x00000074
    uint32_t gpint_mode_set_l;   // 0x00000078
    uint32_t gpint_mode_set_h;   // 0x0000007c
    uint32_t gpint_mode_clr_l;   // 0x00000080
    uint32_t gpint_mode_clr_h;   // 0x00000084
    uint32_t int_status_h;       // 0x00000088
    uint32_t int_clr_h;          // 0x0000008c
} HWP_GPIO_T;

#define hwp_gpio2 ((HWP_GPIO_T *)REG_ACCESS_ADDRESS(REG_GPIO2_BASE))

// chg_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t out_time : 4;  // [3:0]
        uint32_t wait_time : 6; // [9:4]
        uint32_t __15_10 : 6;   // [15:10]
        uint32_t int_mode : 2;  // [17:16]
        uint32_t __31_18 : 14;  // [31:18]
    } b;
} REG_GPIO_CHG_CTRL_T;

// chg_cmd
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dcon_mode_set : 1; // [0], write set
        uint32_t __3_1 : 3;         // [3:1]
        uint32_t chg_mode_set : 1;  // [4], write set
        uint32_t __7_5 : 3;         // [7:5]
        uint32_t dcon_mode_clr : 1; // [8], write clear
        uint32_t __11_9 : 3;        // [11:9]
        uint32_t chg_mode_clr : 1;  // [12], write clear
        uint32_t __23_13 : 11;      // [23:13]
        uint32_t chg_down : 1;      // [24], write set
        uint32_t __31_25 : 7;       // [31:25]
    } b;
} REG_GPIO_CHG_CMD_T;

// gpo_set
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpo_set : 10; // [9:0], write set
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_GPIO_GPO_SET_T;

// gpo_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpo_clr : 10; // [9:0], write clear
        uint32_t __31_10 : 22; // [31:10]
    } b;
} REG_GPIO_GPO_CLR_T;

// gpio_oen_val_l
#define GPIO_OEN_VAL_L(n) (((n)&0xffffffff) << 0)
#define GPIO_OEN_VAL_L_INPUT (1 << 0)
#define GPIO_OEN_VAL_L_OUTPUT (0 << 0)

#define GPIO_OEN_VAL_L_V_INPUT (1)
#define GPIO_OEN_VAL_L_V_OUTPUT (0)

// gpio_oen_set_out_l
#define GPIO_OEN_SET_OUT_L(n) (((n)&0xffffffff) << 0)

// gpio_oen_set_in_l
#define GPIO_OEN_SET_IN_L(n) (((n)&0xffffffff) << 0)

// gpio_val_l
#define GPIO_GPIO_VAL_L(n) (((n)&0xffffffff) << 0)

// gpio_set_l
#define GPIO_GPIO_SET_L(n) (((n)&0xffffffff) << 0)

// gpio_clr_l
#define GPIO_GPIO_CLR_L(n) (((n)&0xffffffff) << 0)

// gpint_r_set_l
#define GPIO_GPINT_R_SET_L(n) (((n)&0xffffffff) << 0)

// gpint_r_set_h
#define GPIO_GPINT_R_SET_H(n) (((n)&0xffffffff) << 0)

// int_clr_l
#define GPIO_GPINT_CLR_L(n) (((n)&0xffffffff) << 0)

// int_status_l
#define GPIO_GPINT_STATUS_L(n) (((n)&0xffffffff) << 0)

// chg_ctrl
#define GPIO_OUT_TIME(n) (((n)&0xf) << 0)
#define GPIO_WAIT_TIME(n) (((n)&0x3f) << 4)
#define GPIO_INT_MODE(n) (((n)&0x3) << 16)
#define GPIO_INT_MODE_L2H (0 << 16)
#define GPIO_INT_MODE_H2L (1 << 16)
#define GPIO_INT_MODE_RR (3 << 16)

#define GPIO_INT_MODE_V_L2H (0)
#define GPIO_INT_MODE_V_H2L (1)
#define GPIO_INT_MODE_V_RR (3)

// chg_cmd
#define GPIO_DCON_MODE_SET (1 << 0)
#define GPIO_CHG_MODE_SET (1 << 4)
#define GPIO_DCON_MODE_CLR (1 << 8)
#define GPIO_CHG_MODE_CLR (1 << 12)
#define GPIO_CHG_DOWN (1 << 24)

// gpo_set
#define GPIO_GPO_SET(n) (((n)&0x3ff) << 0)

// gpo_clr
#define GPIO_GPO_CLR(n) (((n)&0x3ff) << 0)

// gpio_oen_val_h
#define GPIO_OEN_VAL_H(n) (((n)&0xffffffff) << 0)
#define GPIO_OEN_VAL_H_INPUT (1 << 0)
#define GPIO_OEN_VAL_H_OUTPUT (0 << 0)

#define GPIO_OEN_VAL_H_V_INPUT (1)
#define GPIO_OEN_VAL_H_V_OUTPUT (0)

// gpio_oen_set_out_h
#define GPIO_OEN_SET_OUT_H(n) (((n)&0xffffffff) << 0)

// gpio_oen_set_in_h
#define GPIO_OEN_SET_IN_H(n) (((n)&0xffffffff) << 0)

// gpio_val_h
#define GPIO_GPIO_VAL_H(n) (((n)&0xffffffff) << 0)

// gpio_set_h
#define GPIO_GPIO_SET_H(n) (((n)&0xffffffff) << 0)

// gpio_clr_h
#define GPIO_GPIO_CLR_H(n) (((n)&0xffffffff) << 0)

// gpint_r_clr_l
#define GPIO_GPINT_R_CLR_L(n) (((n)&0xffffffff) << 0)

// gpint_r_clr_h
#define GPIO_GPINT_R_CLR_H(n) (((n)&0xffffffff) << 0)

// gpint_f_set_l
#define GPIO_GPINT_F_SET_L(n) (((n)&0xffffffff) << 0)

// gpint_f_set_h
#define GPIO_GPINT_F_SET_H(n) (((n)&0xffffffff) << 0)

// gpint_f_clr_l
#define GPIO_GPINT_F_CLR_L(n) (((n)&0xffffffff) << 0)

// gpint_f_clr_h
#define GPIO_GPINT_F_CLR_H(n) (((n)&0xffffffff) << 0)

// gpint_dbn_en_set_l
#define GPIO_DBN_EN_SET_L(n) (((n)&0xffffffff) << 0)

// gpint_dbn_en_set_h
#define GPIO_DBN_EN_SET_H(n) (((n)&0xffffffff) << 0)

// gpint_dbn_en_clr_l
#define GPIO_DBN_EN_CLR_L(n) (((n)&0xffffffff) << 0)

// gpint_dbn_en_clr_h
#define GPIO_DBN_EN_CLR_H(n) (((n)&0xffffffff) << 0)

// gpint_mode_set_l
#define GPIO_GPINT_MODE_SET_L(n) (((n)&0xffffffff) << 0)

// gpint_mode_set_h
#define GPIO_GPINT_MODE_SET_H(n) (((n)&0xffffffff) << 0)

// gpint_mode_clr_l
#define GPIO_GPINT_MODE_CLR_L(n) (((n)&0xffffffff) << 0)

// gpint_mode_clr_h
#define GPIO_GPINT_MODE_CLR_H(n) (((n)&0xffffffff) << 0)

// int_status_h
#define GPIO_GPINT_STATUS_H(n) (((n)&0xffffffff) << 0)

// int_clr_h
#define GPIO_GPINT_CLR_H(n) (((n)&0xffffffff) << 0)

#endif // _GPIO_H_
