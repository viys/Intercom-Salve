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

#ifndef _IOMUX1_H_
#define _IOMUX1_H_

// Auto generated (v1.0-38-g167fa99). Don't edit it manually!

#define REG_IOMUX1_BASE (0x41807000)

typedef volatile struct
{
    uint32_t pad_gpio_0_cfg; // 0x00000000
    uint32_t pad_gpio_1_cfg; // 0x00000004
    uint32_t pad_gpio_2_cfg; // 0x00000008
    uint32_t pad_gpio_3_cfg; // 0x0000000c
    uint32_t pad_gpio_4_cfg; // 0x00000010
    uint32_t pad_gpio_5_cfg; // 0x00000014
    uint32_t pad_gpio_6_cfg; // 0x00000018
    uint32_t pad_gpio_7_cfg; // 0x0000001c
} HWP_IOMUX1_T;

#define hwp_iomux1 ((HWP_IOMUX1_T *)REG_ACCESS_ADDRESS(REG_IOMUX1_BASE))

// pad_gpio_0_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_0_sel : 4;          // [3:0]
        uint32_t pad_gpio_0_out_reg : 1;      // [4]
        uint32_t pad_gpio_0_out_frc : 1;      // [5]
        uint32_t pad_gpio_0_oen_reg : 1;      // [6]
        uint32_t pad_gpio_0_oen_frc : 1;      // [7]
        uint32_t pad_gpio_0_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_0_pull_dn : 1;      // [10]
        uint32_t pad_gpio_0_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_gpio_0_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_gpio_0_ie : 1;           // [17]
        uint32_t pad_gpio_0_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX1_PAD_GPIO_0_CFG_T;

// pad_gpio_1_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_1_sel : 4;          // [3:0]
        uint32_t pad_gpio_1_out_reg : 1;      // [4]
        uint32_t pad_gpio_1_out_frc : 1;      // [5]
        uint32_t pad_gpio_1_oen_reg : 1;      // [6]
        uint32_t pad_gpio_1_oen_frc : 1;      // [7]
        uint32_t pad_gpio_1_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_1_pull_dn : 1;      // [10]
        uint32_t pad_gpio_1_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_gpio_1_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_gpio_1_ie : 1;           // [17]
        uint32_t pad_gpio_1_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX1_PAD_GPIO_1_CFG_T;

// pad_gpio_2_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_2_sel : 4;          // [3:0]
        uint32_t pad_gpio_2_out_reg : 1;      // [4]
        uint32_t pad_gpio_2_out_frc : 1;      // [5]
        uint32_t pad_gpio_2_oen_reg : 1;      // [6]
        uint32_t pad_gpio_2_oen_frc : 1;      // [7]
        uint32_t pad_gpio_2_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_2_pull_dn : 1;      // [10]
        uint32_t pad_gpio_2_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_gpio_2_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_gpio_2_ie : 1;           // [17]
        uint32_t pad_gpio_2_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX1_PAD_GPIO_2_CFG_T;

// pad_gpio_3_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_3_sel : 4;          // [3:0]
        uint32_t pad_gpio_3_out_reg : 1;      // [4]
        uint32_t pad_gpio_3_out_frc : 1;      // [5]
        uint32_t pad_gpio_3_oen_reg : 1;      // [6]
        uint32_t pad_gpio_3_oen_frc : 1;      // [7]
        uint32_t pad_gpio_3_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_3_pull_dn : 1;      // [10]
        uint32_t pad_gpio_3_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_gpio_3_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_gpio_3_ie : 1;           // [17]
        uint32_t pad_gpio_3_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX1_PAD_GPIO_3_CFG_T;

// pad_gpio_4_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_4_sel : 4;          // [3:0]
        uint32_t pad_gpio_4_out_reg : 1;      // [4]
        uint32_t pad_gpio_4_out_frc : 1;      // [5]
        uint32_t pad_gpio_4_oen_reg : 1;      // [6]
        uint32_t pad_gpio_4_oen_frc : 1;      // [7]
        uint32_t pad_gpio_4_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_4_pull_dn : 1;      // [10]
        uint32_t pad_gpio_4_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_gpio_4_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_gpio_4_ie : 1;           // [17]
        uint32_t pad_gpio_4_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX1_PAD_GPIO_4_CFG_T;

// pad_gpio_5_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_5_sel : 4;          // [3:0]
        uint32_t pad_gpio_5_out_reg : 1;      // [4]
        uint32_t pad_gpio_5_out_frc : 1;      // [5]
        uint32_t pad_gpio_5_oen_reg : 1;      // [6]
        uint32_t pad_gpio_5_oen_frc : 1;      // [7]
        uint32_t pad_gpio_5_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_5_pull_dn : 1;      // [10]
        uint32_t pad_gpio_5_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_gpio_5_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_gpio_5_ie : 1;           // [17]
        uint32_t pad_gpio_5_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX1_PAD_GPIO_5_CFG_T;

// pad_gpio_6_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_6_sel : 4;          // [3:0]
        uint32_t pad_gpio_6_out_reg : 1;      // [4]
        uint32_t pad_gpio_6_out_frc : 1;      // [5]
        uint32_t pad_gpio_6_oen_reg : 1;      // [6]
        uint32_t pad_gpio_6_oen_frc : 1;      // [7]
        uint32_t pad_gpio_6_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_6_pull_dn : 1;      // [10]
        uint32_t pad_gpio_6_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_gpio_6_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_gpio_6_ie : 1;           // [17]
        uint32_t pad_gpio_6_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX1_PAD_GPIO_6_CFG_T;

// pad_gpio_7_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_7_sel : 4;          // [3:0]
        uint32_t pad_gpio_7_out_reg : 1;      // [4]
        uint32_t pad_gpio_7_out_frc : 1;      // [5]
        uint32_t pad_gpio_7_oen_reg : 1;      // [6]
        uint32_t pad_gpio_7_oen_frc : 1;      // [7]
        uint32_t pad_gpio_7_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_7_pull_dn : 1;      // [10]
        uint32_t pad_gpio_7_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_gpio_7_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_gpio_7_ie : 1;           // [17]
        uint32_t pad_gpio_7_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX1_PAD_GPIO_7_CFG_T;

// pad_gpio_0_cfg
#define IOMUX1_PAD_GPIO_0_SEL(n) (((n)&0xf) << 0)
#define IOMUX1_PAD_GPIO_0_SEL_FUN_GPIO_0_SEL (0 << 0)
#define IOMUX1_PAD_GPIO_0_SEL_FUN_UART1_RXD_SEL (1 << 0)
#define IOMUX1_PAD_GPIO_0_OUT_REG (1 << 4)
#define IOMUX1_PAD_GPIO_0_OUT_FRC (1 << 5)
#define IOMUX1_PAD_GPIO_0_OEN_REG (1 << 6)
#define IOMUX1_PAD_GPIO_0_OEN_FRC (1 << 7)
#define IOMUX1_PAD_GPIO_0_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX1_PAD_GPIO_0_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX1_PAD_GPIO_0_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX1_PAD_GPIO_0_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX1_PAD_GPIO_0_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX1_PAD_GPIO_0_PULL_DN (1 << 10)
#define IOMUX1_PAD_GPIO_0_PULL_FRC (1 << 11)
#define IOMUX1_PAD_GPIO_0_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX1_PAD_GPIO_0_IE (1 << 17)
#define IOMUX1_PAD_GPIO_0_SE (1 << 18)

#define IOMUX1_PAD_GPIO_0_SEL_V_FUN_GPIO_0_SEL (0)
#define IOMUX1_PAD_GPIO_0_SEL_V_FUN_UART1_RXD_SEL (1)
#define IOMUX1_PAD_GPIO_0_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX1_PAD_GPIO_0_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX1_PAD_GPIO_0_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX1_PAD_GPIO_0_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_1_cfg
#define IOMUX1_PAD_GPIO_1_SEL(n) (((n)&0xf) << 0)
#define IOMUX1_PAD_GPIO_1_SEL_FUN_GPIO_1_SEL (0 << 0)
#define IOMUX1_PAD_GPIO_1_SEL_FUN_UART1_TXD_SEL (1 << 0)
#define IOMUX1_PAD_GPIO_1_SEL_FUN_GPT1_TI_0_SEL (3 << 0)
#define IOMUX1_PAD_GPIO_1_SEL_FUN_GPT1_PWM_0_SEL (4 << 0)
#define IOMUX1_PAD_GPIO_1_SEL_FUN_CLK_32K_SEL (6 << 0)
#define IOMUX1_PAD_GPIO_1_OUT_REG (1 << 4)
#define IOMUX1_PAD_GPIO_1_OUT_FRC (1 << 5)
#define IOMUX1_PAD_GPIO_1_OEN_REG (1 << 6)
#define IOMUX1_PAD_GPIO_1_OEN_FRC (1 << 7)
#define IOMUX1_PAD_GPIO_1_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX1_PAD_GPIO_1_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX1_PAD_GPIO_1_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX1_PAD_GPIO_1_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX1_PAD_GPIO_1_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX1_PAD_GPIO_1_PULL_DN (1 << 10)
#define IOMUX1_PAD_GPIO_1_PULL_FRC (1 << 11)
#define IOMUX1_PAD_GPIO_1_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX1_PAD_GPIO_1_IE (1 << 17)
#define IOMUX1_PAD_GPIO_1_SE (1 << 18)

#define IOMUX1_PAD_GPIO_1_SEL_V_FUN_GPIO_1_SEL (0)
#define IOMUX1_PAD_GPIO_1_SEL_V_FUN_UART1_TXD_SEL (1)
#define IOMUX1_PAD_GPIO_1_SEL_V_FUN_GPT1_TI_0_SEL (3)
#define IOMUX1_PAD_GPIO_1_SEL_V_FUN_GPT1_PWM_0_SEL (4)
#define IOMUX1_PAD_GPIO_1_SEL_V_FUN_CLK_32K_SEL (6)
#define IOMUX1_PAD_GPIO_1_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX1_PAD_GPIO_1_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX1_PAD_GPIO_1_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX1_PAD_GPIO_1_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_2_cfg
#define IOMUX1_PAD_GPIO_2_SEL(n) (((n)&0xf) << 0)
#define IOMUX1_PAD_GPIO_2_SEL_FUN_GPIO_2_SEL (0 << 0)
#define IOMUX1_PAD_GPIO_2_SEL_FUN_UART1_CTS_SEL (1 << 0)
#define IOMUX1_PAD_GPIO_2_SEL_FUN_I2C3_SCL_SEL (2 << 0)
#define IOMUX1_PAD_GPIO_2_SEL_FUN_GPT1_TI_0_SEL (3 << 0)
#define IOMUX1_PAD_GPIO_2_SEL_FUN_GPT1_PWM_0_SEL (4 << 0)
#define IOMUX1_PAD_GPIO_2_SEL_FUN_UART2_RXD_SEL (5 << 0)
#define IOMUX1_PAD_GPIO_2_SEL_FUN_CLK_32K_SEL (6 << 0)
#define IOMUX1_PAD_GPIO_2_OUT_REG (1 << 4)
#define IOMUX1_PAD_GPIO_2_OUT_FRC (1 << 5)
#define IOMUX1_PAD_GPIO_2_OEN_REG (1 << 6)
#define IOMUX1_PAD_GPIO_2_OEN_FRC (1 << 7)
#define IOMUX1_PAD_GPIO_2_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX1_PAD_GPIO_2_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX1_PAD_GPIO_2_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX1_PAD_GPIO_2_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX1_PAD_GPIO_2_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX1_PAD_GPIO_2_PULL_DN (1 << 10)
#define IOMUX1_PAD_GPIO_2_PULL_FRC (1 << 11)
#define IOMUX1_PAD_GPIO_2_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX1_PAD_GPIO_2_IE (1 << 17)
#define IOMUX1_PAD_GPIO_2_SE (1 << 18)

#define IOMUX1_PAD_GPIO_2_SEL_V_FUN_GPIO_2_SEL (0)
#define IOMUX1_PAD_GPIO_2_SEL_V_FUN_UART1_CTS_SEL (1)
#define IOMUX1_PAD_GPIO_2_SEL_V_FUN_I2C3_SCL_SEL (2)
#define IOMUX1_PAD_GPIO_2_SEL_V_FUN_GPT1_TI_0_SEL (3)
#define IOMUX1_PAD_GPIO_2_SEL_V_FUN_GPT1_PWM_0_SEL (4)
#define IOMUX1_PAD_GPIO_2_SEL_V_FUN_UART2_RXD_SEL (5)
#define IOMUX1_PAD_GPIO_2_SEL_V_FUN_CLK_32K_SEL (6)
#define IOMUX1_PAD_GPIO_2_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX1_PAD_GPIO_2_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX1_PAD_GPIO_2_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX1_PAD_GPIO_2_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_3_cfg
#define IOMUX1_PAD_GPIO_3_SEL(n) (((n)&0xf) << 0)
#define IOMUX1_PAD_GPIO_3_SEL_FUN_GPIO_3_SEL (0 << 0)
#define IOMUX1_PAD_GPIO_3_SEL_FUN_UART1_RTS_SEL (1 << 0)
#define IOMUX1_PAD_GPIO_3_SEL_FUN_I2C3_SDA_SEL (2 << 0)
#define IOMUX1_PAD_GPIO_3_SEL_FUN_GPT1_TI_1_SEL (3 << 0)
#define IOMUX1_PAD_GPIO_3_SEL_FUN_GPT1_PWM_1_SEL (4 << 0)
#define IOMUX1_PAD_GPIO_3_SEL_FUN_UART2_TXD_SEL (5 << 0)
#define IOMUX1_PAD_GPIO_3_SEL_FUN_CLK_32K_SEL (6 << 0)
#define IOMUX1_PAD_GPIO_3_OUT_REG (1 << 4)
#define IOMUX1_PAD_GPIO_3_OUT_FRC (1 << 5)
#define IOMUX1_PAD_GPIO_3_OEN_REG (1 << 6)
#define IOMUX1_PAD_GPIO_3_OEN_FRC (1 << 7)
#define IOMUX1_PAD_GPIO_3_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX1_PAD_GPIO_3_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX1_PAD_GPIO_3_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX1_PAD_GPIO_3_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX1_PAD_GPIO_3_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX1_PAD_GPIO_3_PULL_DN (1 << 10)
#define IOMUX1_PAD_GPIO_3_PULL_FRC (1 << 11)
#define IOMUX1_PAD_GPIO_3_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX1_PAD_GPIO_3_IE (1 << 17)
#define IOMUX1_PAD_GPIO_3_SE (1 << 18)

#define IOMUX1_PAD_GPIO_3_SEL_V_FUN_GPIO_3_SEL (0)
#define IOMUX1_PAD_GPIO_3_SEL_V_FUN_UART1_RTS_SEL (1)
#define IOMUX1_PAD_GPIO_3_SEL_V_FUN_I2C3_SDA_SEL (2)
#define IOMUX1_PAD_GPIO_3_SEL_V_FUN_GPT1_TI_1_SEL (3)
#define IOMUX1_PAD_GPIO_3_SEL_V_FUN_GPT1_PWM_1_SEL (4)
#define IOMUX1_PAD_GPIO_3_SEL_V_FUN_UART2_TXD_SEL (5)
#define IOMUX1_PAD_GPIO_3_SEL_V_FUN_CLK_32K_SEL (6)
#define IOMUX1_PAD_GPIO_3_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX1_PAD_GPIO_3_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX1_PAD_GPIO_3_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX1_PAD_GPIO_3_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_4_cfg
#define IOMUX1_PAD_GPIO_4_SEL(n) (((n)&0xf) << 0)
#define IOMUX1_PAD_GPIO_4_SEL_FUN_GPIO_4_SEL (0 << 0)
#define IOMUX1_PAD_GPIO_4_SEL_FUN_SPI2_CLK_SEL (1 << 0)
#define IOMUX1_PAD_GPIO_4_SEL_FUN_GPT1_PWM_0_SEL (4 << 0)
#define IOMUX1_PAD_GPIO_4_SEL_FUN_UART2_RXD_SEL (5 << 0)
#define IOMUX1_PAD_GPIO_4_SEL_FUN_CLK_32K_SEL (6 << 0)
#define IOMUX1_PAD_GPIO_4_OUT_REG (1 << 4)
#define IOMUX1_PAD_GPIO_4_OUT_FRC (1 << 5)
#define IOMUX1_PAD_GPIO_4_OEN_REG (1 << 6)
#define IOMUX1_PAD_GPIO_4_OEN_FRC (1 << 7)
#define IOMUX1_PAD_GPIO_4_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX1_PAD_GPIO_4_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX1_PAD_GPIO_4_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX1_PAD_GPIO_4_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX1_PAD_GPIO_4_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX1_PAD_GPIO_4_PULL_DN (1 << 10)
#define IOMUX1_PAD_GPIO_4_PULL_FRC (1 << 11)
#define IOMUX1_PAD_GPIO_4_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX1_PAD_GPIO_4_IE (1 << 17)
#define IOMUX1_PAD_GPIO_4_SE (1 << 18)

#define IOMUX1_PAD_GPIO_4_SEL_V_FUN_GPIO_4_SEL (0)
#define IOMUX1_PAD_GPIO_4_SEL_V_FUN_SPI2_CLK_SEL (1)
#define IOMUX1_PAD_GPIO_4_SEL_V_FUN_GPT1_PWM_0_SEL (4)
#define IOMUX1_PAD_GPIO_4_SEL_V_FUN_UART2_RXD_SEL (5)
#define IOMUX1_PAD_GPIO_4_SEL_V_FUN_CLK_32K_SEL (6)
#define IOMUX1_PAD_GPIO_4_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX1_PAD_GPIO_4_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX1_PAD_GPIO_4_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX1_PAD_GPIO_4_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_5_cfg
#define IOMUX1_PAD_GPIO_5_SEL(n) (((n)&0xf) << 0)
#define IOMUX1_PAD_GPIO_5_SEL_FUN_GPIO_5_SEL (0 << 0)
#define IOMUX1_PAD_GPIO_5_SEL_FUN_SPI2_CS_SEL (1 << 0)
#define IOMUX1_PAD_GPIO_5_SEL_FUN_GPT1_PWM_1_SEL (4 << 0)
#define IOMUX1_PAD_GPIO_5_SEL_FUN_UART2_TXD_SEL (5 << 0)
#define IOMUX1_PAD_GPIO_5_SEL_FUN_CLK_32K_SEL (6 << 0)
#define IOMUX1_PAD_GPIO_5_OUT_REG (1 << 4)
#define IOMUX1_PAD_GPIO_5_OUT_FRC (1 << 5)
#define IOMUX1_PAD_GPIO_5_OEN_REG (1 << 6)
#define IOMUX1_PAD_GPIO_5_OEN_FRC (1 << 7)
#define IOMUX1_PAD_GPIO_5_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX1_PAD_GPIO_5_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX1_PAD_GPIO_5_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX1_PAD_GPIO_5_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX1_PAD_GPIO_5_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX1_PAD_GPIO_5_PULL_DN (1 << 10)
#define IOMUX1_PAD_GPIO_5_PULL_FRC (1 << 11)
#define IOMUX1_PAD_GPIO_5_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX1_PAD_GPIO_5_IE (1 << 17)
#define IOMUX1_PAD_GPIO_5_SE (1 << 18)

#define IOMUX1_PAD_GPIO_5_SEL_V_FUN_GPIO_5_SEL (0)
#define IOMUX1_PAD_GPIO_5_SEL_V_FUN_SPI2_CS_SEL (1)
#define IOMUX1_PAD_GPIO_5_SEL_V_FUN_GPT1_PWM_1_SEL (4)
#define IOMUX1_PAD_GPIO_5_SEL_V_FUN_UART2_TXD_SEL (5)
#define IOMUX1_PAD_GPIO_5_SEL_V_FUN_CLK_32K_SEL (6)
#define IOMUX1_PAD_GPIO_5_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX1_PAD_GPIO_5_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX1_PAD_GPIO_5_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX1_PAD_GPIO_5_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_6_cfg
#define IOMUX1_PAD_GPIO_6_SEL(n) (((n)&0xf) << 0)
#define IOMUX1_PAD_GPIO_6_SEL_FUN_GPIO_6_SEL (0 << 0)
#define IOMUX1_PAD_GPIO_6_SEL_FUN_SPI2_DI_0_SEL (1 << 0)
#define IOMUX1_PAD_GPIO_6_SEL_FUN_I2C3_SCL_SEL (2 << 0)
#define IOMUX1_PAD_GPIO_6_SEL_FUN_GPT1_TI_0_SEL (3 << 0)
#define IOMUX1_PAD_GPIO_6_SEL_FUN_GPT1_PWM_0_SEL (4 << 0)
#define IOMUX1_PAD_GPIO_6_SEL_FUN_UART2_CTS_SEL (5 << 0)
#define IOMUX1_PAD_GPIO_6_SEL_FUN_CLK_32K_SEL (6 << 0)
#define IOMUX1_PAD_GPIO_6_OUT_REG (1 << 4)
#define IOMUX1_PAD_GPIO_6_OUT_FRC (1 << 5)
#define IOMUX1_PAD_GPIO_6_OEN_REG (1 << 6)
#define IOMUX1_PAD_GPIO_6_OEN_FRC (1 << 7)
#define IOMUX1_PAD_GPIO_6_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX1_PAD_GPIO_6_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX1_PAD_GPIO_6_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX1_PAD_GPIO_6_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX1_PAD_GPIO_6_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX1_PAD_GPIO_6_PULL_DN (1 << 10)
#define IOMUX1_PAD_GPIO_6_PULL_FRC (1 << 11)
#define IOMUX1_PAD_GPIO_6_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX1_PAD_GPIO_6_IE (1 << 17)
#define IOMUX1_PAD_GPIO_6_SE (1 << 18)

#define IOMUX1_PAD_GPIO_6_SEL_V_FUN_GPIO_6_SEL (0)
#define IOMUX1_PAD_GPIO_6_SEL_V_FUN_SPI2_DI_0_SEL (1)
#define IOMUX1_PAD_GPIO_6_SEL_V_FUN_I2C3_SCL_SEL (2)
#define IOMUX1_PAD_GPIO_6_SEL_V_FUN_GPT1_TI_0_SEL (3)
#define IOMUX1_PAD_GPIO_6_SEL_V_FUN_GPT1_PWM_0_SEL (4)
#define IOMUX1_PAD_GPIO_6_SEL_V_FUN_UART2_CTS_SEL (5)
#define IOMUX1_PAD_GPIO_6_SEL_V_FUN_CLK_32K_SEL (6)
#define IOMUX1_PAD_GPIO_6_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX1_PAD_GPIO_6_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX1_PAD_GPIO_6_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX1_PAD_GPIO_6_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_7_cfg
#define IOMUX1_PAD_GPIO_7_SEL(n) (((n)&0xf) << 0)
#define IOMUX1_PAD_GPIO_7_SEL_FUN_GPIO_7_SEL (0 << 0)
#define IOMUX1_PAD_GPIO_7_SEL_FUN_SPI2_DI_1_SEL (1 << 0)
#define IOMUX1_PAD_GPIO_7_SEL_FUN_I2C3_SDA_SEL (2 << 0)
#define IOMUX1_PAD_GPIO_7_SEL_FUN_GPT1_TI_1_SEL (3 << 0)
#define IOMUX1_PAD_GPIO_7_SEL_FUN_GPT1_PWM_1_SEL (4 << 0)
#define IOMUX1_PAD_GPIO_7_SEL_FUN_UART2_RTS_SEL (5 << 0)
#define IOMUX1_PAD_GPIO_7_SEL_FUN_CLK_32K_SEL (6 << 0)
#define IOMUX1_PAD_GPIO_7_OUT_REG (1 << 4)
#define IOMUX1_PAD_GPIO_7_OUT_FRC (1 << 5)
#define IOMUX1_PAD_GPIO_7_OEN_REG (1 << 6)
#define IOMUX1_PAD_GPIO_7_OEN_FRC (1 << 7)
#define IOMUX1_PAD_GPIO_7_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX1_PAD_GPIO_7_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX1_PAD_GPIO_7_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX1_PAD_GPIO_7_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX1_PAD_GPIO_7_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX1_PAD_GPIO_7_PULL_DN (1 << 10)
#define IOMUX1_PAD_GPIO_7_PULL_FRC (1 << 11)
#define IOMUX1_PAD_GPIO_7_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX1_PAD_GPIO_7_IE (1 << 17)
#define IOMUX1_PAD_GPIO_7_SE (1 << 18)

#define IOMUX1_PAD_GPIO_7_SEL_V_FUN_GPIO_7_SEL (0)
#define IOMUX1_PAD_GPIO_7_SEL_V_FUN_SPI2_DI_1_SEL (1)
#define IOMUX1_PAD_GPIO_7_SEL_V_FUN_I2C3_SDA_SEL (2)
#define IOMUX1_PAD_GPIO_7_SEL_V_FUN_GPT1_TI_1_SEL (3)
#define IOMUX1_PAD_GPIO_7_SEL_V_FUN_GPT1_PWM_1_SEL (4)
#define IOMUX1_PAD_GPIO_7_SEL_V_FUN_UART2_RTS_SEL (5)
#define IOMUX1_PAD_GPIO_7_SEL_V_FUN_CLK_32K_SEL (6)
#define IOMUX1_PAD_GPIO_7_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX1_PAD_GPIO_7_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX1_PAD_GPIO_7_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX1_PAD_GPIO_7_PULL_UP_V_PULL_UP_1_8K (3)

#endif // _IOMUX1_H_
