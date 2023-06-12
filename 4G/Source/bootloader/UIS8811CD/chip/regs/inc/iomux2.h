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

#ifndef _IOMUX2_H_
#define _IOMUX2_H_

// Auto generated (v1.0-38-g167fa99). Don't edit it manually!

#define REG_IOMUX2_BASE (0x41808000)

typedef volatile struct
{
    uint32_t pad_m_dq_0_cfg;    // 0x00000000
    uint32_t pad_m_dq_1_cfg;    // 0x00000004
    uint32_t pad_m_dq_2_cfg;    // 0x00000008
    uint32_t pad_m_dq_3_cfg;    // 0x0000000c
    uint32_t pad_m_dq_4_cfg;    // 0x00000010
    uint32_t pad_m_dq_5_cfg;    // 0x00000014
    uint32_t pad_m_dq_6_cfg;    // 0x00000018
    uint32_t pad_m_dq_7_cfg;    // 0x0000001c
    uint32_t pad_m_cs_cfg;      // 0x00000020
    uint32_t pad_m_dm_cfg;      // 0x00000024
    uint32_t pad_m_clk_cfg;     // 0x00000028
    uint32_t pad_m_clkb_cfg;    // 0x0000002c
    uint32_t pad_m_dqs_cfg;     // 0x00000030
    uint32_t pad_m_spi_clk_cfg; // 0x00000034
    uint32_t pad_m_spi_cs_cfg;  // 0x00000038
    uint32_t pad_m_spi_d_0_cfg; // 0x0000003c
    uint32_t pad_m_spi_d_1_cfg; // 0x00000040
    uint32_t pad_m_spi_d_2_cfg; // 0x00000044
    uint32_t pad_m_spi_d_3_cfg; // 0x00000048
    uint32_t pad_gpio_8_cfg;    // 0x0000004c
    uint32_t pad_gpio_9_cfg;    // 0x00000050
    uint32_t pad_gpio_10_cfg;   // 0x00000054
    uint32_t pad_gpio_11_cfg;   // 0x00000058
    uint32_t pad_gpio_12_cfg;   // 0x0000005c
    uint32_t pad_gpio_13_cfg;   // 0x00000060
    uint32_t pad_gpio_14_cfg;   // 0x00000064
    uint32_t pad_gpio_15_cfg;   // 0x00000068
    uint32_t pad_gpio_16_cfg;   // 0x0000006c
    uint32_t pad_gpio_17_cfg;   // 0x00000070
    uint32_t pad_gpio_18_cfg;   // 0x00000074
    uint32_t pad_gpio_19_cfg;   // 0x00000078
    uint32_t pad_gpio_20_cfg;   // 0x0000007c
    uint32_t pad_gpio_21_cfg;   // 0x00000080
    uint32_t pad_gpio_22_cfg;   // 0x00000084
    uint32_t pad_gpio_23_cfg;   // 0x00000088
    uint32_t pad_gpio_24_cfg;   // 0x0000008c
    uint32_t pad_gpio_25_cfg;   // 0x00000090
    uint32_t pad_gpio_26_cfg;   // 0x00000094
    uint32_t pad_gpio_27_cfg;   // 0x00000098
    uint32_t pad_gpio_28_cfg;   // 0x0000009c
    uint32_t pad_gpio_29_cfg;   // 0x000000a0
    uint32_t pad_gpio_30_cfg;   // 0x000000a4
    uint32_t pad_gpio_31_cfg;   // 0x000000a8
    uint32_t pad_gpio_32_cfg;   // 0x000000ac
    uint32_t pad_gpio_33_cfg;   // 0x000000b0
    uint32_t pad_swclk_cfg;     // 0x000000b4
    uint32_t pad_swdio_cfg;     // 0x000000b8
    uint32_t pad_sim_clk_0_cfg; // 0x000000bc
    uint32_t pad_sim_rst_0_cfg; // 0x000000c0
    uint32_t pad_sim_dio_0_cfg; // 0x000000c4
    uint32_t pad_sim_clk_1_cfg; // 0x000000c8
    uint32_t pad_sim_rst_1_cfg; // 0x000000cc
    uint32_t pad_sim_dio_1_cfg; // 0x000000d0
} HWP_IOMUX2_T;

#define hwp_iomux2 ((HWP_IOMUX2_T *)REG_ACCESS_ADDRESS(REG_IOMUX2_BASE))

// pad_m_dq_0_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_dq_0_sel : 4;          // [3:0]
        uint32_t pad_m_dq_0_out_reg : 1;      // [4]
        uint32_t pad_m_dq_0_out_frc : 1;      // [5]
        uint32_t pad_m_dq_0_oen_reg : 1;      // [6]
        uint32_t pad_m_dq_0_oen_frc : 1;      // [7]
        uint32_t pad_m_dq_0_pull_up : 2;      // [9:8]
        uint32_t pad_m_dq_0_pull_dn : 1;      // [10]
        uint32_t pad_m_dq_0_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_m_dq_0_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_m_dq_0_ie : 1;           // [17]
        uint32_t pad_m_dq_0_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX2_PAD_M_DQ_0_CFG_T;

// pad_m_dq_1_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_dq_1_sel : 4;          // [3:0]
        uint32_t pad_m_dq_1_out_reg : 1;      // [4]
        uint32_t pad_m_dq_1_out_frc : 1;      // [5]
        uint32_t pad_m_dq_1_oen_reg : 1;      // [6]
        uint32_t pad_m_dq_1_oen_frc : 1;      // [7]
        uint32_t pad_m_dq_1_pull_up : 2;      // [9:8]
        uint32_t pad_m_dq_1_pull_dn : 1;      // [10]
        uint32_t pad_m_dq_1_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_m_dq_1_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_m_dq_1_ie : 1;           // [17]
        uint32_t pad_m_dq_1_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX2_PAD_M_DQ_1_CFG_T;

// pad_m_dq_2_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_dq_2_sel : 4;          // [3:0]
        uint32_t pad_m_dq_2_out_reg : 1;      // [4]
        uint32_t pad_m_dq_2_out_frc : 1;      // [5]
        uint32_t pad_m_dq_2_oen_reg : 1;      // [6]
        uint32_t pad_m_dq_2_oen_frc : 1;      // [7]
        uint32_t pad_m_dq_2_pull_up : 2;      // [9:8]
        uint32_t pad_m_dq_2_pull_dn : 1;      // [10]
        uint32_t pad_m_dq_2_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_m_dq_2_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_m_dq_2_ie : 1;           // [17]
        uint32_t pad_m_dq_2_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX2_PAD_M_DQ_2_CFG_T;

// pad_m_dq_3_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_dq_3_sel : 4;          // [3:0]
        uint32_t pad_m_dq_3_out_reg : 1;      // [4]
        uint32_t pad_m_dq_3_out_frc : 1;      // [5]
        uint32_t pad_m_dq_3_oen_reg : 1;      // [6]
        uint32_t pad_m_dq_3_oen_frc : 1;      // [7]
        uint32_t pad_m_dq_3_pull_up : 2;      // [9:8]
        uint32_t pad_m_dq_3_pull_dn : 1;      // [10]
        uint32_t pad_m_dq_3_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_m_dq_3_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_m_dq_3_ie : 1;           // [17]
        uint32_t pad_m_dq_3_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX2_PAD_M_DQ_3_CFG_T;

// pad_m_dq_4_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_dq_4_sel : 4;          // [3:0]
        uint32_t pad_m_dq_4_out_reg : 1;      // [4]
        uint32_t pad_m_dq_4_out_frc : 1;      // [5]
        uint32_t pad_m_dq_4_oen_reg : 1;      // [6]
        uint32_t pad_m_dq_4_oen_frc : 1;      // [7]
        uint32_t pad_m_dq_4_pull_up : 2;      // [9:8]
        uint32_t pad_m_dq_4_pull_dn : 1;      // [10]
        uint32_t pad_m_dq_4_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_m_dq_4_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_m_dq_4_ie : 1;           // [17]
        uint32_t pad_m_dq_4_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX2_PAD_M_DQ_4_CFG_T;

// pad_m_dq_5_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_dq_5_sel : 4;          // [3:0]
        uint32_t pad_m_dq_5_out_reg : 1;      // [4]
        uint32_t pad_m_dq_5_out_frc : 1;      // [5]
        uint32_t pad_m_dq_5_oen_reg : 1;      // [6]
        uint32_t pad_m_dq_5_oen_frc : 1;      // [7]
        uint32_t pad_m_dq_5_pull_up : 2;      // [9:8]
        uint32_t pad_m_dq_5_pull_dn : 1;      // [10]
        uint32_t pad_m_dq_5_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_m_dq_5_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_m_dq_5_ie : 1;           // [17]
        uint32_t pad_m_dq_5_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX2_PAD_M_DQ_5_CFG_T;

// pad_m_dq_6_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_dq_6_sel : 4;          // [3:0]
        uint32_t pad_m_dq_6_out_reg : 1;      // [4]
        uint32_t pad_m_dq_6_out_frc : 1;      // [5]
        uint32_t pad_m_dq_6_oen_reg : 1;      // [6]
        uint32_t pad_m_dq_6_oen_frc : 1;      // [7]
        uint32_t pad_m_dq_6_pull_up : 2;      // [9:8]
        uint32_t pad_m_dq_6_pull_dn : 1;      // [10]
        uint32_t pad_m_dq_6_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_m_dq_6_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_m_dq_6_ie : 1;           // [17]
        uint32_t pad_m_dq_6_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX2_PAD_M_DQ_6_CFG_T;

// pad_m_dq_7_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_dq_7_sel : 4;          // [3:0]
        uint32_t pad_m_dq_7_out_reg : 1;      // [4]
        uint32_t pad_m_dq_7_out_frc : 1;      // [5]
        uint32_t pad_m_dq_7_oen_reg : 1;      // [6]
        uint32_t pad_m_dq_7_oen_frc : 1;      // [7]
        uint32_t pad_m_dq_7_pull_up : 2;      // [9:8]
        uint32_t pad_m_dq_7_pull_dn : 1;      // [10]
        uint32_t pad_m_dq_7_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_m_dq_7_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_m_dq_7_ie : 1;           // [17]
        uint32_t pad_m_dq_7_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX2_PAD_M_DQ_7_CFG_T;

// pad_m_cs_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_cs_sel : 4;          // [3:0]
        uint32_t pad_m_cs_out_reg : 1;      // [4]
        uint32_t pad_m_cs_out_frc : 1;      // [5]
        uint32_t pad_m_cs_oen_reg : 1;      // [6]
        uint32_t pad_m_cs_oen_frc : 1;      // [7]
        uint32_t pad_m_cs_pull_up : 2;      // [9:8]
        uint32_t pad_m_cs_pull_dn : 1;      // [10]
        uint32_t pad_m_cs_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;               // [13:12]
        uint32_t pad_m_cs_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;               // [16]
        uint32_t pad_m_cs_ie : 1;           // [17]
        uint32_t pad_m_cs_se : 1;           // [18]
        uint32_t __31_19 : 13;              // [31:19]
    } b;
} REG_IOMUX2_PAD_M_CS_CFG_T;

// pad_m_dm_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_dm_sel : 4;          // [3:0]
        uint32_t pad_m_dm_out_reg : 1;      // [4]
        uint32_t pad_m_dm_out_frc : 1;      // [5]
        uint32_t pad_m_dm_oen_reg : 1;      // [6]
        uint32_t pad_m_dm_oen_frc : 1;      // [7]
        uint32_t pad_m_dm_pull_up : 2;      // [9:8]
        uint32_t pad_m_dm_pull_dn : 1;      // [10]
        uint32_t pad_m_dm_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;               // [13:12]
        uint32_t pad_m_dm_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;               // [16]
        uint32_t pad_m_dm_ie : 1;           // [17]
        uint32_t pad_m_dm_se : 1;           // [18]
        uint32_t __31_19 : 13;              // [31:19]
    } b;
} REG_IOMUX2_PAD_M_DM_CFG_T;

// pad_m_clk_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_clk_sel : 4;          // [3:0]
        uint32_t pad_m_clk_out_reg : 1;      // [4]
        uint32_t pad_m_clk_out_frc : 1;      // [5]
        uint32_t pad_m_clk_oen_reg : 1;      // [6]
        uint32_t pad_m_clk_oen_frc : 1;      // [7]
        uint32_t pad_m_clk_pull_up : 2;      // [9:8]
        uint32_t pad_m_clk_pull_dn : 1;      // [10]
        uint32_t pad_m_clk_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                // [13:12]
        uint32_t pad_m_clk_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                // [16]
        uint32_t pad_m_clk_ie : 1;           // [17]
        uint32_t pad_m_clk_se : 1;           // [18]
        uint32_t __31_19 : 13;               // [31:19]
    } b;
} REG_IOMUX2_PAD_M_CLK_CFG_T;

// pad_m_clkb_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_clkb_sel : 4;          // [3:0]
        uint32_t pad_m_clkb_out_reg : 1;      // [4]
        uint32_t pad_m_clkb_out_frc : 1;      // [5]
        uint32_t pad_m_clkb_oen_reg : 1;      // [6]
        uint32_t pad_m_clkb_oen_frc : 1;      // [7]
        uint32_t pad_m_clkb_pull_up : 2;      // [9:8]
        uint32_t pad_m_clkb_pull_dn : 1;      // [10]
        uint32_t pad_m_clkb_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_m_clkb_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_m_clkb_ie : 1;           // [17]
        uint32_t pad_m_clkb_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX2_PAD_M_CLKB_CFG_T;

// pad_m_dqs_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_dqs_sel : 4;          // [3:0]
        uint32_t pad_m_dqs_out_reg : 1;      // [4]
        uint32_t pad_m_dqs_out_frc : 1;      // [5]
        uint32_t pad_m_dqs_oen_reg : 1;      // [6]
        uint32_t pad_m_dqs_oen_frc : 1;      // [7]
        uint32_t pad_m_dqs_pull_up : 2;      // [9:8]
        uint32_t pad_m_dqs_pull_dn : 1;      // [10]
        uint32_t pad_m_dqs_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                // [13:12]
        uint32_t pad_m_dqs_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                // [16]
        uint32_t pad_m_dqs_ie : 1;           // [17]
        uint32_t pad_m_dqs_se : 1;           // [18]
        uint32_t __31_19 : 13;               // [31:19]
    } b;
} REG_IOMUX2_PAD_M_DQS_CFG_T;

// pad_m_spi_clk_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_spi_clk_sel : 4;          // [3:0]
        uint32_t pad_m_spi_clk_out_reg : 1;      // [4]
        uint32_t pad_m_spi_clk_out_frc : 1;      // [5]
        uint32_t pad_m_spi_clk_oen_reg : 1;      // [6]
        uint32_t pad_m_spi_clk_oen_frc : 1;      // [7]
        uint32_t pad_m_spi_clk_pull_up : 2;      // [9:8]
        uint32_t pad_m_spi_clk_pull_dn : 1;      // [10]
        uint32_t pad_m_spi_clk_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                    // [13:12]
        uint32_t pad_m_spi_clk_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                    // [16]
        uint32_t pad_m_spi_clk_ie : 1;           // [17]
        uint32_t pad_m_spi_clk_se : 1;           // [18]
        uint32_t __31_19 : 13;                   // [31:19]
    } b;
} REG_IOMUX2_PAD_M_SPI_CLK_CFG_T;

// pad_m_spi_cs_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_spi_cs_sel : 4;          // [3:0]
        uint32_t pad_m_spi_cs_out_reg : 1;      // [4]
        uint32_t pad_m_spi_cs_out_frc : 1;      // [5]
        uint32_t pad_m_spi_cs_oen_reg : 1;      // [6]
        uint32_t pad_m_spi_cs_oen_frc : 1;      // [7]
        uint32_t pad_m_spi_cs_pull_up : 2;      // [9:8]
        uint32_t pad_m_spi_cs_pull_dn : 1;      // [10]
        uint32_t pad_m_spi_cs_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                   // [13:12]
        uint32_t pad_m_spi_cs_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                   // [16]
        uint32_t pad_m_spi_cs_ie : 1;           // [17]
        uint32_t pad_m_spi_cs_se : 1;           // [18]
        uint32_t __31_19 : 13;                  // [31:19]
    } b;
} REG_IOMUX2_PAD_M_SPI_CS_CFG_T;

// pad_m_spi_d_0_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_spi_d_0_sel : 4;          // [3:0]
        uint32_t pad_m_spi_d_0_out_reg : 1;      // [4]
        uint32_t pad_m_spi_d_0_out_frc : 1;      // [5]
        uint32_t pad_m_spi_d_0_oen_reg : 1;      // [6]
        uint32_t pad_m_spi_d_0_oen_frc : 1;      // [7]
        uint32_t pad_m_spi_d_0_pull_up : 2;      // [9:8]
        uint32_t pad_m_spi_d_0_pull_dn : 1;      // [10]
        uint32_t pad_m_spi_d_0_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                    // [13:12]
        uint32_t pad_m_spi_d_0_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                    // [16]
        uint32_t pad_m_spi_d_0_ie : 1;           // [17]
        uint32_t pad_m_spi_d_0_se : 1;           // [18]
        uint32_t __31_19 : 13;                   // [31:19]
    } b;
} REG_IOMUX2_PAD_M_SPI_D_0_CFG_T;

// pad_m_spi_d_1_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_spi_d_1_sel : 4;          // [3:0]
        uint32_t pad_m_spi_d_1_out_reg : 1;      // [4]
        uint32_t pad_m_spi_d_1_out_frc : 1;      // [5]
        uint32_t pad_m_spi_d_1_oen_reg : 1;      // [6]
        uint32_t pad_m_spi_d_1_oen_frc : 1;      // [7]
        uint32_t pad_m_spi_d_1_pull_up : 2;      // [9:8]
        uint32_t pad_m_spi_d_1_pull_dn : 1;      // [10]
        uint32_t pad_m_spi_d_1_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                    // [13:12]
        uint32_t pad_m_spi_d_1_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                    // [16]
        uint32_t pad_m_spi_d_1_ie : 1;           // [17]
        uint32_t pad_m_spi_d_1_se : 1;           // [18]
        uint32_t __31_19 : 13;                   // [31:19]
    } b;
} REG_IOMUX2_PAD_M_SPI_D_1_CFG_T;

// pad_m_spi_d_2_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_spi_d_2_sel : 4;          // [3:0]
        uint32_t pad_m_spi_d_2_out_reg : 1;      // [4]
        uint32_t pad_m_spi_d_2_out_frc : 1;      // [5]
        uint32_t pad_m_spi_d_2_oen_reg : 1;      // [6]
        uint32_t pad_m_spi_d_2_oen_frc : 1;      // [7]
        uint32_t pad_m_spi_d_2_pull_up : 2;      // [9:8]
        uint32_t pad_m_spi_d_2_pull_dn : 1;      // [10]
        uint32_t pad_m_spi_d_2_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                    // [13:12]
        uint32_t pad_m_spi_d_2_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                    // [16]
        uint32_t pad_m_spi_d_2_ie : 1;           // [17]
        uint32_t pad_m_spi_d_2_se : 1;           // [18]
        uint32_t __31_19 : 13;                   // [31:19]
    } b;
} REG_IOMUX2_PAD_M_SPI_D_2_CFG_T;

// pad_m_spi_d_3_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_m_spi_d_3_sel : 4;          // [3:0]
        uint32_t pad_m_spi_d_3_out_reg : 1;      // [4]
        uint32_t pad_m_spi_d_3_out_frc : 1;      // [5]
        uint32_t pad_m_spi_d_3_oen_reg : 1;      // [6]
        uint32_t pad_m_spi_d_3_oen_frc : 1;      // [7]
        uint32_t pad_m_spi_d_3_pull_up : 2;      // [9:8]
        uint32_t pad_m_spi_d_3_pull_dn : 1;      // [10]
        uint32_t pad_m_spi_d_3_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                    // [13:12]
        uint32_t pad_m_spi_d_3_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                    // [16]
        uint32_t pad_m_spi_d_3_ie : 1;           // [17]
        uint32_t pad_m_spi_d_3_se : 1;           // [18]
        uint32_t __31_19 : 13;                   // [31:19]
    } b;
} REG_IOMUX2_PAD_M_SPI_D_3_CFG_T;

// pad_gpio_8_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_8_sel : 4;          // [3:0]
        uint32_t pad_gpio_8_out_reg : 1;      // [4]
        uint32_t pad_gpio_8_out_frc : 1;      // [5]
        uint32_t pad_gpio_8_oen_reg : 1;      // [6]
        uint32_t pad_gpio_8_oen_frc : 1;      // [7]
        uint32_t pad_gpio_8_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_8_pull_dn : 1;      // [10]
        uint32_t pad_gpio_8_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_gpio_8_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_gpio_8_ie : 1;           // [17]
        uint32_t pad_gpio_8_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_8_CFG_T;

// pad_gpio_9_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_9_sel : 4;          // [3:0]
        uint32_t pad_gpio_9_out_reg : 1;      // [4]
        uint32_t pad_gpio_9_out_frc : 1;      // [5]
        uint32_t pad_gpio_9_oen_reg : 1;      // [6]
        uint32_t pad_gpio_9_oen_frc : 1;      // [7]
        uint32_t pad_gpio_9_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_9_pull_dn : 1;      // [10]
        uint32_t pad_gpio_9_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                 // [13:12]
        uint32_t pad_gpio_9_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                 // [16]
        uint32_t pad_gpio_9_ie : 1;           // [17]
        uint32_t pad_gpio_9_se : 1;           // [18]
        uint32_t __31_19 : 13;                // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_9_CFG_T;

// pad_gpio_10_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_10_sel : 4;          // [3:0]
        uint32_t pad_gpio_10_out_reg : 1;      // [4]
        uint32_t pad_gpio_10_out_frc : 1;      // [5]
        uint32_t pad_gpio_10_oen_reg : 1;      // [6]
        uint32_t pad_gpio_10_oen_frc : 1;      // [7]
        uint32_t pad_gpio_10_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_10_pull_dn : 1;      // [10]
        uint32_t pad_gpio_10_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_10_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_10_ie : 1;           // [17]
        uint32_t pad_gpio_10_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_10_CFG_T;

// pad_gpio_11_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_11_sel : 4;          // [3:0]
        uint32_t pad_gpio_11_out_reg : 1;      // [4]
        uint32_t pad_gpio_11_out_frc : 1;      // [5]
        uint32_t pad_gpio_11_oen_reg : 1;      // [6]
        uint32_t pad_gpio_11_oen_frc : 1;      // [7]
        uint32_t pad_gpio_11_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_11_pull_dn : 1;      // [10]
        uint32_t pad_gpio_11_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_11_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_11_ie : 1;           // [17]
        uint32_t pad_gpio_11_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_11_CFG_T;

// pad_gpio_12_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_12_sel : 4;          // [3:0]
        uint32_t pad_gpio_12_out_reg : 1;      // [4]
        uint32_t pad_gpio_12_out_frc : 1;      // [5]
        uint32_t pad_gpio_12_oen_reg : 1;      // [6]
        uint32_t pad_gpio_12_oen_frc : 1;      // [7]
        uint32_t pad_gpio_12_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_12_pull_dn : 1;      // [10]
        uint32_t pad_gpio_12_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_12_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_12_ie : 1;           // [17]
        uint32_t pad_gpio_12_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_12_CFG_T;

// pad_gpio_13_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_13_sel : 4;          // [3:0]
        uint32_t pad_gpio_13_out_reg : 1;      // [4]
        uint32_t pad_gpio_13_out_frc : 1;      // [5]
        uint32_t pad_gpio_13_oen_reg : 1;      // [6]
        uint32_t pad_gpio_13_oen_frc : 1;      // [7]
        uint32_t pad_gpio_13_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_13_pull_dn : 1;      // [10]
        uint32_t pad_gpio_13_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_13_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_13_ie : 1;           // [17]
        uint32_t pad_gpio_13_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_13_CFG_T;

// pad_gpio_14_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_14_sel : 4;          // [3:0]
        uint32_t pad_gpio_14_out_reg : 1;      // [4]
        uint32_t pad_gpio_14_out_frc : 1;      // [5]
        uint32_t pad_gpio_14_oen_reg : 1;      // [6]
        uint32_t pad_gpio_14_oen_frc : 1;      // [7]
        uint32_t pad_gpio_14_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_14_pull_dn : 1;      // [10]
        uint32_t pad_gpio_14_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_14_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_14_ie : 1;           // [17]
        uint32_t pad_gpio_14_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_14_CFG_T;

// pad_gpio_15_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_15_sel : 4;          // [3:0]
        uint32_t pad_gpio_15_out_reg : 1;      // [4]
        uint32_t pad_gpio_15_out_frc : 1;      // [5]
        uint32_t pad_gpio_15_oen_reg : 1;      // [6]
        uint32_t pad_gpio_15_oen_frc : 1;      // [7]
        uint32_t pad_gpio_15_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_15_pull_dn : 1;      // [10]
        uint32_t pad_gpio_15_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_15_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_15_ie : 1;           // [17]
        uint32_t pad_gpio_15_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_15_CFG_T;

// pad_gpio_16_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_16_sel : 4;          // [3:0]
        uint32_t pad_gpio_16_out_reg : 1;      // [4]
        uint32_t pad_gpio_16_out_frc : 1;      // [5]
        uint32_t pad_gpio_16_oen_reg : 1;      // [6]
        uint32_t pad_gpio_16_oen_frc : 1;      // [7]
        uint32_t pad_gpio_16_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_16_pull_dn : 1;      // [10]
        uint32_t pad_gpio_16_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_16_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_16_ie : 1;           // [17]
        uint32_t pad_gpio_16_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_16_CFG_T;

// pad_gpio_17_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_17_sel : 4;          // [3:0]
        uint32_t pad_gpio_17_out_reg : 1;      // [4]
        uint32_t pad_gpio_17_out_frc : 1;      // [5]
        uint32_t pad_gpio_17_oen_reg : 1;      // [6]
        uint32_t pad_gpio_17_oen_frc : 1;      // [7]
        uint32_t pad_gpio_17_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_17_pull_dn : 1;      // [10]
        uint32_t pad_gpio_17_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_17_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_17_ie : 1;           // [17]
        uint32_t pad_gpio_17_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_17_CFG_T;

// pad_gpio_18_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_18_sel : 4;          // [3:0]
        uint32_t pad_gpio_18_out_reg : 1;      // [4]
        uint32_t pad_gpio_18_out_frc : 1;      // [5]
        uint32_t pad_gpio_18_oen_reg : 1;      // [6]
        uint32_t pad_gpio_18_oen_frc : 1;      // [7]
        uint32_t pad_gpio_18_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_18_pull_dn : 1;      // [10]
        uint32_t pad_gpio_18_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_18_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_18_ie : 1;           // [17]
        uint32_t pad_gpio_18_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_18_CFG_T;

// pad_gpio_19_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_19_sel : 4;          // [3:0]
        uint32_t pad_gpio_19_out_reg : 1;      // [4]
        uint32_t pad_gpio_19_out_frc : 1;      // [5]
        uint32_t pad_gpio_19_oen_reg : 1;      // [6]
        uint32_t pad_gpio_19_oen_frc : 1;      // [7]
        uint32_t pad_gpio_19_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_19_pull_dn : 1;      // [10]
        uint32_t pad_gpio_19_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_19_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_19_ie : 1;           // [17]
        uint32_t pad_gpio_19_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_19_CFG_T;

// pad_gpio_20_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_20_sel : 4;          // [3:0]
        uint32_t pad_gpio_20_out_reg : 1;      // [4]
        uint32_t pad_gpio_20_out_frc : 1;      // [5]
        uint32_t pad_gpio_20_oen_reg : 1;      // [6]
        uint32_t pad_gpio_20_oen_frc : 1;      // [7]
        uint32_t pad_gpio_20_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_20_pull_dn : 1;      // [10]
        uint32_t pad_gpio_20_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_20_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_20_ie : 1;           // [17]
        uint32_t pad_gpio_20_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_20_CFG_T;

// pad_gpio_21_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_21_sel : 4;          // [3:0]
        uint32_t pad_gpio_21_out_reg : 1;      // [4]
        uint32_t pad_gpio_21_out_frc : 1;      // [5]
        uint32_t pad_gpio_21_oen_reg : 1;      // [6]
        uint32_t pad_gpio_21_oen_frc : 1;      // [7]
        uint32_t pad_gpio_21_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_21_pull_dn : 1;      // [10]
        uint32_t pad_gpio_21_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_21_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_21_ie : 1;           // [17]
        uint32_t pad_gpio_21_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_21_CFG_T;

// pad_gpio_22_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_22_sel : 4;          // [3:0]
        uint32_t pad_gpio_22_out_reg : 1;      // [4]
        uint32_t pad_gpio_22_out_frc : 1;      // [5]
        uint32_t pad_gpio_22_oen_reg : 1;      // [6]
        uint32_t pad_gpio_22_oen_frc : 1;      // [7]
        uint32_t pad_gpio_22_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_22_pull_dn : 1;      // [10]
        uint32_t pad_gpio_22_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_22_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_22_ie : 1;           // [17]
        uint32_t pad_gpio_22_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_22_CFG_T;

// pad_gpio_23_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_23_sel : 4;          // [3:0]
        uint32_t pad_gpio_23_out_reg : 1;      // [4]
        uint32_t pad_gpio_23_out_frc : 1;      // [5]
        uint32_t pad_gpio_23_oen_reg : 1;      // [6]
        uint32_t pad_gpio_23_oen_frc : 1;      // [7]
        uint32_t pad_gpio_23_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_23_pull_dn : 1;      // [10]
        uint32_t pad_gpio_23_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_23_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_23_ie : 1;           // [17]
        uint32_t pad_gpio_23_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_23_CFG_T;

// pad_gpio_24_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_24_sel : 4;          // [3:0]
        uint32_t pad_gpio_24_out_reg : 1;      // [4]
        uint32_t pad_gpio_24_out_frc : 1;      // [5]
        uint32_t pad_gpio_24_oen_reg : 1;      // [6]
        uint32_t pad_gpio_24_oen_frc : 1;      // [7]
        uint32_t pad_gpio_24_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_24_pull_dn : 1;      // [10]
        uint32_t pad_gpio_24_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_24_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_24_ie : 1;           // [17]
        uint32_t pad_gpio_24_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_24_CFG_T;

// pad_gpio_25_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_25_sel : 4;          // [3:0]
        uint32_t pad_gpio_25_out_reg : 1;      // [4]
        uint32_t pad_gpio_25_out_frc : 1;      // [5]
        uint32_t pad_gpio_25_oen_reg : 1;      // [6]
        uint32_t pad_gpio_25_oen_frc : 1;      // [7]
        uint32_t pad_gpio_25_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_25_pull_dn : 1;      // [10]
        uint32_t pad_gpio_25_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_25_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_25_ie : 1;           // [17]
        uint32_t pad_gpio_25_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_25_CFG_T;

// pad_gpio_26_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_26_sel : 4;          // [3:0]
        uint32_t pad_gpio_26_out_reg : 1;      // [4]
        uint32_t pad_gpio_26_out_frc : 1;      // [5]
        uint32_t pad_gpio_26_oen_reg : 1;      // [6]
        uint32_t pad_gpio_26_oen_frc : 1;      // [7]
        uint32_t pad_gpio_26_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_26_pull_dn : 1;      // [10]
        uint32_t pad_gpio_26_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_26_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_26_ie : 1;           // [17]
        uint32_t pad_gpio_26_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_26_CFG_T;

// pad_gpio_27_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_27_sel : 4;          // [3:0]
        uint32_t pad_gpio_27_out_reg : 1;      // [4]
        uint32_t pad_gpio_27_out_frc : 1;      // [5]
        uint32_t pad_gpio_27_oen_reg : 1;      // [6]
        uint32_t pad_gpio_27_oen_frc : 1;      // [7]
        uint32_t pad_gpio_27_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_27_pull_dn : 1;      // [10]
        uint32_t pad_gpio_27_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_27_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_27_ie : 1;           // [17]
        uint32_t pad_gpio_27_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_27_CFG_T;

// pad_gpio_28_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_28_sel : 4;          // [3:0]
        uint32_t pad_gpio_28_out_reg : 1;      // [4]
        uint32_t pad_gpio_28_out_frc : 1;      // [5]
        uint32_t pad_gpio_28_oen_reg : 1;      // [6]
        uint32_t pad_gpio_28_oen_frc : 1;      // [7]
        uint32_t pad_gpio_28_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_28_pull_dn : 1;      // [10]
        uint32_t pad_gpio_28_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_28_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_28_ie : 1;           // [17]
        uint32_t pad_gpio_28_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_28_CFG_T;

// pad_gpio_29_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_29_sel : 4;          // [3:0]
        uint32_t pad_gpio_29_out_reg : 1;      // [4]
        uint32_t pad_gpio_29_out_frc : 1;      // [5]
        uint32_t pad_gpio_29_oen_reg : 1;      // [6]
        uint32_t pad_gpio_29_oen_frc : 1;      // [7]
        uint32_t pad_gpio_29_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_29_pull_dn : 1;      // [10]
        uint32_t pad_gpio_29_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_29_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_29_ie : 1;           // [17]
        uint32_t pad_gpio_29_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_29_CFG_T;

// pad_gpio_30_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_30_sel : 4;          // [3:0]
        uint32_t pad_gpio_30_out_reg : 1;      // [4]
        uint32_t pad_gpio_30_out_frc : 1;      // [5]
        uint32_t pad_gpio_30_oen_reg : 1;      // [6]
        uint32_t pad_gpio_30_oen_frc : 1;      // [7]
        uint32_t pad_gpio_30_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_30_pull_dn : 1;      // [10]
        uint32_t pad_gpio_30_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_30_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_30_ie : 1;           // [17]
        uint32_t pad_gpio_30_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_30_CFG_T;

// pad_gpio_31_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_31_sel : 4;          // [3:0]
        uint32_t pad_gpio_31_out_reg : 1;      // [4]
        uint32_t pad_gpio_31_out_frc : 1;      // [5]
        uint32_t pad_gpio_31_oen_reg : 1;      // [6]
        uint32_t pad_gpio_31_oen_frc : 1;      // [7]
        uint32_t pad_gpio_31_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_31_pull_dn : 1;      // [10]
        uint32_t pad_gpio_31_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_31_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_31_ie : 1;           // [17]
        uint32_t pad_gpio_31_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_31_CFG_T;

// pad_gpio_32_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_32_sel : 4;          // [3:0]
        uint32_t pad_gpio_32_out_reg : 1;      // [4]
        uint32_t pad_gpio_32_out_frc : 1;      // [5]
        uint32_t pad_gpio_32_oen_reg : 1;      // [6]
        uint32_t pad_gpio_32_oen_frc : 1;      // [7]
        uint32_t pad_gpio_32_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_32_pull_dn : 1;      // [10]
        uint32_t pad_gpio_32_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_32_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_32_ie : 1;           // [17]
        uint32_t pad_gpio_32_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_32_CFG_T;

// pad_gpio_33_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_gpio_33_sel : 4;          // [3:0]
        uint32_t pad_gpio_33_out_reg : 1;      // [4]
        uint32_t pad_gpio_33_out_frc : 1;      // [5]
        uint32_t pad_gpio_33_oen_reg : 1;      // [6]
        uint32_t pad_gpio_33_oen_frc : 1;      // [7]
        uint32_t pad_gpio_33_pull_up : 2;      // [9:8]
        uint32_t pad_gpio_33_pull_dn : 1;      // [10]
        uint32_t pad_gpio_33_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                  // [13:12]
        uint32_t pad_gpio_33_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                  // [16]
        uint32_t pad_gpio_33_ie : 1;           // [17]
        uint32_t pad_gpio_33_se : 1;           // [18]
        uint32_t __31_19 : 13;                 // [31:19]
    } b;
} REG_IOMUX2_PAD_GPIO_33_CFG_T;

// pad_swclk_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_swclk_sel : 4;          // [3:0]
        uint32_t pad_swclk_out_reg : 1;      // [4]
        uint32_t pad_swclk_out_frc : 1;      // [5]
        uint32_t pad_swclk_oen_reg : 1;      // [6]
        uint32_t pad_swclk_oen_frc : 1;      // [7]
        uint32_t pad_swclk_pull_up : 2;      // [9:8]
        uint32_t pad_swclk_pull_dn : 1;      // [10]
        uint32_t pad_swclk_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                // [13:12]
        uint32_t pad_swclk_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                // [16]
        uint32_t pad_swclk_ie : 1;           // [17]
        uint32_t pad_swclk_se : 1;           // [18]
        uint32_t __31_19 : 13;               // [31:19]
    } b;
} REG_IOMUX2_PAD_SWCLK_CFG_T;

// pad_swdio_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_swdio_sel : 4;          // [3:0]
        uint32_t pad_swdio_out_reg : 1;      // [4]
        uint32_t pad_swdio_out_frc : 1;      // [5]
        uint32_t pad_swdio_oen_reg : 1;      // [6]
        uint32_t pad_swdio_oen_frc : 1;      // [7]
        uint32_t pad_swdio_pull_up : 2;      // [9:8]
        uint32_t pad_swdio_pull_dn : 1;      // [10]
        uint32_t pad_swdio_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                // [13:12]
        uint32_t pad_swdio_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                // [16]
        uint32_t pad_swdio_ie : 1;           // [17]
        uint32_t pad_swdio_se : 1;           // [18]
        uint32_t __31_19 : 13;               // [31:19]
    } b;
} REG_IOMUX2_PAD_SWDIO_CFG_T;

// pad_sim_clk_0_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_sim_clk_0_sel : 4;          // [3:0]
        uint32_t pad_sim_clk_0_out_reg : 1;      // [4]
        uint32_t pad_sim_clk_0_out_frc : 1;      // [5]
        uint32_t pad_sim_clk_0_oen_reg : 1;      // [6]
        uint32_t pad_sim_clk_0_oen_frc : 1;      // [7]
        uint32_t pad_sim_clk_0_pull_up : 2;      // [9:8]
        uint32_t pad_sim_clk_0_pull_dn : 1;      // [10]
        uint32_t pad_sim_clk_0_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                    // [13:12]
        uint32_t pad_sim_clk_0_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                    // [16]
        uint32_t pad_sim_clk_0_ie : 1;           // [17]
        uint32_t pad_sim_clk_0_se : 1;           // [18]
        uint32_t __31_19 : 13;                   // [31:19]
    } b;
} REG_IOMUX2_PAD_SIM_CLK_0_CFG_T;

// pad_sim_rst_0_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_sim_rst_0_sel : 4;          // [3:0]
        uint32_t pad_sim_rst_0_out_reg : 1;      // [4]
        uint32_t pad_sim_rst_0_out_frc : 1;      // [5]
        uint32_t pad_sim_rst_0_oen_reg : 1;      // [6]
        uint32_t pad_sim_rst_0_oen_frc : 1;      // [7]
        uint32_t pad_sim_rst_0_pull_up : 2;      // [9:8]
        uint32_t pad_sim_rst_0_pull_dn : 1;      // [10]
        uint32_t pad_sim_rst_0_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                    // [13:12]
        uint32_t pad_sim_rst_0_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                    // [16]
        uint32_t pad_sim_rst_0_ie : 1;           // [17]
        uint32_t pad_sim_rst_0_se : 1;           // [18]
        uint32_t __31_19 : 13;                   // [31:19]
    } b;
} REG_IOMUX2_PAD_SIM_RST_0_CFG_T;

// pad_sim_dio_0_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_sim_dio_0_sel : 4;          // [3:0]
        uint32_t pad_sim_dio_0_out_reg : 1;      // [4]
        uint32_t pad_sim_dio_0_out_frc : 1;      // [5]
        uint32_t pad_sim_dio_0_oen_reg : 1;      // [6]
        uint32_t pad_sim_dio_0_oen_frc : 1;      // [7]
        uint32_t pad_sim_dio_0_pull_up : 2;      // [9:8]
        uint32_t pad_sim_dio_0_pull_dn : 1;      // [10]
        uint32_t pad_sim_dio_0_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                    // [13:12]
        uint32_t pad_sim_dio_0_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                    // [16]
        uint32_t pad_sim_dio_0_ie : 1;           // [17]
        uint32_t pad_sim_dio_0_se : 1;           // [18]
        uint32_t __31_19 : 13;                   // [31:19]
    } b;
} REG_IOMUX2_PAD_SIM_DIO_0_CFG_T;

// pad_sim_clk_1_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_sim_clk_1_sel : 4;          // [3:0]
        uint32_t pad_sim_clk_1_out_reg : 1;      // [4]
        uint32_t pad_sim_clk_1_out_frc : 1;      // [5]
        uint32_t pad_sim_clk_1_oen_reg : 1;      // [6]
        uint32_t pad_sim_clk_1_oen_frc : 1;      // [7]
        uint32_t pad_sim_clk_1_pull_up : 2;      // [9:8]
        uint32_t pad_sim_clk_1_pull_dn : 1;      // [10]
        uint32_t pad_sim_clk_1_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                    // [13:12]
        uint32_t pad_sim_clk_1_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                    // [16]
        uint32_t pad_sim_clk_1_ie : 1;           // [17]
        uint32_t pad_sim_clk_1_se : 1;           // [18]
        uint32_t __31_19 : 13;                   // [31:19]
    } b;
} REG_IOMUX2_PAD_SIM_CLK_1_CFG_T;

// pad_sim_rst_1_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_sim_rst_1_sel : 4;          // [3:0]
        uint32_t pad_sim_rst_1_out_reg : 1;      // [4]
        uint32_t pad_sim_rst_1_out_frc : 1;      // [5]
        uint32_t pad_sim_rst_1_oen_reg : 1;      // [6]
        uint32_t pad_sim_rst_1_oen_frc : 1;      // [7]
        uint32_t pad_sim_rst_1_pull_up : 2;      // [9:8]
        uint32_t pad_sim_rst_1_pull_dn : 1;      // [10]
        uint32_t pad_sim_rst_1_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                    // [13:12]
        uint32_t pad_sim_rst_1_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                    // [16]
        uint32_t pad_sim_rst_1_ie : 1;           // [17]
        uint32_t pad_sim_rst_1_se : 1;           // [18]
        uint32_t __31_19 : 13;                   // [31:19]
    } b;
} REG_IOMUX2_PAD_SIM_RST_1_CFG_T;

// pad_sim_dio_1_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pad_sim_dio_1_sel : 4;          // [3:0]
        uint32_t pad_sim_dio_1_out_reg : 1;      // [4]
        uint32_t pad_sim_dio_1_out_frc : 1;      // [5]
        uint32_t pad_sim_dio_1_oen_reg : 1;      // [6]
        uint32_t pad_sim_dio_1_oen_frc : 1;      // [7]
        uint32_t pad_sim_dio_1_pull_up : 2;      // [9:8]
        uint32_t pad_sim_dio_1_pull_dn : 1;      // [10]
        uint32_t pad_sim_dio_1_pull_frc : 1;     // [11]
        uint32_t __13_12 : 2;                    // [13:12]
        uint32_t pad_sim_dio_1_drv_strength : 2; // [15:14]
        uint32_t __16_16 : 1;                    // [16]
        uint32_t pad_sim_dio_1_ie : 1;           // [17]
        uint32_t pad_sim_dio_1_se : 1;           // [18]
        uint32_t __31_19 : 13;                   // [31:19]
    } b;
} REG_IOMUX2_PAD_SIM_DIO_1_CFG_T;

// pad_m_dq_0_cfg
#define IOMUX2_PAD_M_DQ_0_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_DQ_0_SEL_FUN_M_DQ_0_SEL (0 << 0)
#define IOMUX2_PAD_M_DQ_0_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_DQ_0_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_DQ_0_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_DQ_0_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_DQ_0_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_DQ_0_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_DQ_0_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_DQ_0_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_DQ_0_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_DQ_0_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_DQ_0_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_DQ_0_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_DQ_0_IE (1 << 17)
#define IOMUX2_PAD_M_DQ_0_SE (1 << 18)

#define IOMUX2_PAD_M_DQ_0_SEL_V_FUN_M_DQ_0_SEL (0)
#define IOMUX2_PAD_M_DQ_0_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_DQ_0_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_DQ_0_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_DQ_0_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_dq_1_cfg
#define IOMUX2_PAD_M_DQ_1_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_DQ_1_SEL_FUN_M_DQ_1_SEL (0 << 0)
#define IOMUX2_PAD_M_DQ_1_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_DQ_1_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_DQ_1_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_DQ_1_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_DQ_1_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_DQ_1_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_DQ_1_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_DQ_1_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_DQ_1_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_DQ_1_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_DQ_1_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_DQ_1_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_DQ_1_IE (1 << 17)
#define IOMUX2_PAD_M_DQ_1_SE (1 << 18)

#define IOMUX2_PAD_M_DQ_1_SEL_V_FUN_M_DQ_1_SEL (0)
#define IOMUX2_PAD_M_DQ_1_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_DQ_1_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_DQ_1_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_DQ_1_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_dq_2_cfg
#define IOMUX2_PAD_M_DQ_2_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_DQ_2_SEL_FUN_M_DQ_2_SEL (0 << 0)
#define IOMUX2_PAD_M_DQ_2_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_DQ_2_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_DQ_2_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_DQ_2_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_DQ_2_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_DQ_2_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_DQ_2_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_DQ_2_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_DQ_2_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_DQ_2_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_DQ_2_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_DQ_2_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_DQ_2_IE (1 << 17)
#define IOMUX2_PAD_M_DQ_2_SE (1 << 18)

#define IOMUX2_PAD_M_DQ_2_SEL_V_FUN_M_DQ_2_SEL (0)
#define IOMUX2_PAD_M_DQ_2_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_DQ_2_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_DQ_2_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_DQ_2_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_dq_3_cfg
#define IOMUX2_PAD_M_DQ_3_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_DQ_3_SEL_FUN_M_DQ_3_SEL (0 << 0)
#define IOMUX2_PAD_M_DQ_3_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_DQ_3_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_DQ_3_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_DQ_3_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_DQ_3_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_DQ_3_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_DQ_3_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_DQ_3_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_DQ_3_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_DQ_3_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_DQ_3_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_DQ_3_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_DQ_3_IE (1 << 17)
#define IOMUX2_PAD_M_DQ_3_SE (1 << 18)

#define IOMUX2_PAD_M_DQ_3_SEL_V_FUN_M_DQ_3_SEL (0)
#define IOMUX2_PAD_M_DQ_3_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_DQ_3_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_DQ_3_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_DQ_3_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_dq_4_cfg
#define IOMUX2_PAD_M_DQ_4_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_DQ_4_SEL_FUN_M_DQ_4_SEL (0 << 0)
#define IOMUX2_PAD_M_DQ_4_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_DQ_4_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_DQ_4_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_DQ_4_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_DQ_4_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_DQ_4_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_DQ_4_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_DQ_4_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_DQ_4_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_DQ_4_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_DQ_4_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_DQ_4_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_DQ_4_IE (1 << 17)
#define IOMUX2_PAD_M_DQ_4_SE (1 << 18)

#define IOMUX2_PAD_M_DQ_4_SEL_V_FUN_M_DQ_4_SEL (0)
#define IOMUX2_PAD_M_DQ_4_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_DQ_4_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_DQ_4_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_DQ_4_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_dq_5_cfg
#define IOMUX2_PAD_M_DQ_5_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_DQ_5_SEL_FUN_M_DQ_5_SEL (0 << 0)
#define IOMUX2_PAD_M_DQ_5_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_DQ_5_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_DQ_5_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_DQ_5_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_DQ_5_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_DQ_5_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_DQ_5_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_DQ_5_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_DQ_5_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_DQ_5_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_DQ_5_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_DQ_5_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_DQ_5_IE (1 << 17)
#define IOMUX2_PAD_M_DQ_5_SE (1 << 18)

#define IOMUX2_PAD_M_DQ_5_SEL_V_FUN_M_DQ_5_SEL (0)
#define IOMUX2_PAD_M_DQ_5_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_DQ_5_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_DQ_5_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_DQ_5_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_dq_6_cfg
#define IOMUX2_PAD_M_DQ_6_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_DQ_6_SEL_FUN_M_DQ_6_SEL (0 << 0)
#define IOMUX2_PAD_M_DQ_6_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_DQ_6_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_DQ_6_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_DQ_6_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_DQ_6_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_DQ_6_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_DQ_6_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_DQ_6_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_DQ_6_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_DQ_6_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_DQ_6_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_DQ_6_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_DQ_6_IE (1 << 17)
#define IOMUX2_PAD_M_DQ_6_SE (1 << 18)

#define IOMUX2_PAD_M_DQ_6_SEL_V_FUN_M_DQ_6_SEL (0)
#define IOMUX2_PAD_M_DQ_6_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_DQ_6_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_DQ_6_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_DQ_6_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_dq_7_cfg
#define IOMUX2_PAD_M_DQ_7_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_DQ_7_SEL_FUN_M_DQ_7_SEL (0 << 0)
#define IOMUX2_PAD_M_DQ_7_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_DQ_7_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_DQ_7_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_DQ_7_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_DQ_7_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_DQ_7_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_DQ_7_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_DQ_7_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_DQ_7_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_DQ_7_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_DQ_7_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_DQ_7_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_DQ_7_IE (1 << 17)
#define IOMUX2_PAD_M_DQ_7_SE (1 << 18)

#define IOMUX2_PAD_M_DQ_7_SEL_V_FUN_M_DQ_7_SEL (0)
#define IOMUX2_PAD_M_DQ_7_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_DQ_7_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_DQ_7_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_DQ_7_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_cs_cfg
#define IOMUX2_PAD_M_CS_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_CS_SEL_FUN_M_CS_SEL (0 << 0)
#define IOMUX2_PAD_M_CS_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_CS_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_CS_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_CS_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_CS_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_CS_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_CS_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_CS_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_CS_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_CS_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_CS_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_CS_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_CS_IE (1 << 17)
#define IOMUX2_PAD_M_CS_SE (1 << 18)

#define IOMUX2_PAD_M_CS_SEL_V_FUN_M_CS_SEL (0)
#define IOMUX2_PAD_M_CS_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_CS_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_CS_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_CS_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_dm_cfg
#define IOMUX2_PAD_M_DM_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_DM_SEL_FUN_M_DM_SEL (0 << 0)
#define IOMUX2_PAD_M_DM_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_DM_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_DM_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_DM_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_DM_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_DM_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_DM_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_DM_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_DM_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_DM_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_DM_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_DM_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_DM_IE (1 << 17)
#define IOMUX2_PAD_M_DM_SE (1 << 18)

#define IOMUX2_PAD_M_DM_SEL_V_FUN_M_DM_SEL (0)
#define IOMUX2_PAD_M_DM_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_DM_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_DM_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_DM_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_clk_cfg
#define IOMUX2_PAD_M_CLK_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_CLK_SEL_FUN_M_CLK_SEL (0 << 0)
#define IOMUX2_PAD_M_CLK_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_CLK_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_CLK_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_CLK_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_CLK_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_CLK_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_CLK_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_CLK_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_CLK_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_CLK_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_CLK_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_CLK_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_CLK_IE (1 << 17)
#define IOMUX2_PAD_M_CLK_SE (1 << 18)

#define IOMUX2_PAD_M_CLK_SEL_V_FUN_M_CLK_SEL (0)
#define IOMUX2_PAD_M_CLK_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_CLK_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_CLK_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_CLK_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_clkb_cfg
#define IOMUX2_PAD_M_CLKB_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_CLKB_SEL_FUN_M_CLKB_SEL (0 << 0)
#define IOMUX2_PAD_M_CLKB_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_CLKB_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_CLKB_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_CLKB_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_CLKB_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_CLKB_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_CLKB_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_CLKB_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_CLKB_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_CLKB_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_CLKB_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_CLKB_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_CLKB_IE (1 << 17)
#define IOMUX2_PAD_M_CLKB_SE (1 << 18)

#define IOMUX2_PAD_M_CLKB_SEL_V_FUN_M_CLKB_SEL (0)
#define IOMUX2_PAD_M_CLKB_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_CLKB_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_CLKB_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_CLKB_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_dqs_cfg
#define IOMUX2_PAD_M_DQS_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_DQS_SEL_FUN_M_DQS_SEL (0 << 0)
#define IOMUX2_PAD_M_DQS_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_DQS_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_DQS_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_DQS_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_DQS_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_DQS_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_DQS_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_DQS_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_DQS_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_DQS_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_DQS_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_DQS_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_DQS_IE (1 << 17)
#define IOMUX2_PAD_M_DQS_SE (1 << 18)

#define IOMUX2_PAD_M_DQS_SEL_V_FUN_M_DQS_SEL (0)
#define IOMUX2_PAD_M_DQS_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_DQS_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_DQS_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_DQS_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_spi_clk_cfg
#define IOMUX2_PAD_M_SPI_CLK_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_SPI_CLK_SEL_FUN_M_SPI_CLK_SEL (0 << 0)
#define IOMUX2_PAD_M_SPI_CLK_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_SPI_CLK_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_SPI_CLK_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_SPI_CLK_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_SPI_CLK_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_SPI_CLK_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_SPI_CLK_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_SPI_CLK_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_SPI_CLK_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_SPI_CLK_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_SPI_CLK_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_SPI_CLK_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_SPI_CLK_IE (1 << 17)
#define IOMUX2_PAD_M_SPI_CLK_SE (1 << 18)

#define IOMUX2_PAD_M_SPI_CLK_SEL_V_FUN_M_SPI_CLK_SEL (0)
#define IOMUX2_PAD_M_SPI_CLK_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_SPI_CLK_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_SPI_CLK_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_SPI_CLK_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_spi_cs_cfg
#define IOMUX2_PAD_M_SPI_CS_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_SPI_CS_SEL_FUN_M_SPI_CS_SEL (0 << 0)
#define IOMUX2_PAD_M_SPI_CS_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_SPI_CS_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_SPI_CS_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_SPI_CS_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_SPI_CS_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_SPI_CS_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_SPI_CS_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_SPI_CS_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_SPI_CS_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_SPI_CS_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_SPI_CS_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_SPI_CS_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_SPI_CS_IE (1 << 17)
#define IOMUX2_PAD_M_SPI_CS_SE (1 << 18)

#define IOMUX2_PAD_M_SPI_CS_SEL_V_FUN_M_SPI_CS_SEL (0)
#define IOMUX2_PAD_M_SPI_CS_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_SPI_CS_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_SPI_CS_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_SPI_CS_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_spi_d_0_cfg
#define IOMUX2_PAD_M_SPI_D_0_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_SPI_D_0_SEL_FUN_M_SPI_D_0_SEL (0 << 0)
#define IOMUX2_PAD_M_SPI_D_0_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_SPI_D_0_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_SPI_D_0_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_SPI_D_0_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_SPI_D_0_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_SPI_D_0_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_SPI_D_0_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_SPI_D_0_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_SPI_D_0_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_SPI_D_0_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_SPI_D_0_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_SPI_D_0_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_SPI_D_0_IE (1 << 17)
#define IOMUX2_PAD_M_SPI_D_0_SE (1 << 18)

#define IOMUX2_PAD_M_SPI_D_0_SEL_V_FUN_M_SPI_D_0_SEL (0)
#define IOMUX2_PAD_M_SPI_D_0_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_SPI_D_0_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_SPI_D_0_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_SPI_D_0_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_spi_d_1_cfg
#define IOMUX2_PAD_M_SPI_D_1_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_SPI_D_1_SEL_FUN_M_SPI_D_1_SEL (0 << 0)
#define IOMUX2_PAD_M_SPI_D_1_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_SPI_D_1_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_SPI_D_1_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_SPI_D_1_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_SPI_D_1_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_SPI_D_1_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_SPI_D_1_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_SPI_D_1_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_SPI_D_1_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_SPI_D_1_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_SPI_D_1_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_SPI_D_1_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_SPI_D_1_IE (1 << 17)
#define IOMUX2_PAD_M_SPI_D_1_SE (1 << 18)

#define IOMUX2_PAD_M_SPI_D_1_SEL_V_FUN_M_SPI_D_1_SEL (0)
#define IOMUX2_PAD_M_SPI_D_1_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_SPI_D_1_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_SPI_D_1_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_SPI_D_1_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_spi_d_2_cfg
#define IOMUX2_PAD_M_SPI_D_2_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_SPI_D_2_SEL_FUN_M_SPI_D_2_SEL (0 << 0)
#define IOMUX2_PAD_M_SPI_D_2_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_SPI_D_2_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_SPI_D_2_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_SPI_D_2_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_SPI_D_2_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_SPI_D_2_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_SPI_D_2_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_SPI_D_2_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_SPI_D_2_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_SPI_D_2_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_SPI_D_2_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_SPI_D_2_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_SPI_D_2_IE (1 << 17)
#define IOMUX2_PAD_M_SPI_D_2_SE (1 << 18)

#define IOMUX2_PAD_M_SPI_D_2_SEL_V_FUN_M_SPI_D_2_SEL (0)
#define IOMUX2_PAD_M_SPI_D_2_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_SPI_D_2_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_SPI_D_2_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_SPI_D_2_PULL_UP_V_PULL_UP_1_8K (3)

// pad_m_spi_d_3_cfg
#define IOMUX2_PAD_M_SPI_D_3_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_M_SPI_D_3_SEL_FUN_M_SPI_D_3_SEL (0 << 0)
#define IOMUX2_PAD_M_SPI_D_3_OUT_REG (1 << 4)
#define IOMUX2_PAD_M_SPI_D_3_OUT_FRC (1 << 5)
#define IOMUX2_PAD_M_SPI_D_3_OEN_REG (1 << 6)
#define IOMUX2_PAD_M_SPI_D_3_OEN_FRC (1 << 7)
#define IOMUX2_PAD_M_SPI_D_3_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_M_SPI_D_3_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_M_SPI_D_3_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_M_SPI_D_3_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_M_SPI_D_3_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_M_SPI_D_3_PULL_DN (1 << 10)
#define IOMUX2_PAD_M_SPI_D_3_PULL_FRC (1 << 11)
#define IOMUX2_PAD_M_SPI_D_3_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_M_SPI_D_3_IE (1 << 17)
#define IOMUX2_PAD_M_SPI_D_3_SE (1 << 18)

#define IOMUX2_PAD_M_SPI_D_3_SEL_V_FUN_M_SPI_D_3_SEL (0)
#define IOMUX2_PAD_M_SPI_D_3_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_M_SPI_D_3_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_M_SPI_D_3_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_M_SPI_D_3_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_8_cfg
#define IOMUX2_PAD_GPIO_8_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_8_SEL_FUN_GPIO_8_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_8_SEL_FUN_UART3_RXD_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_8_SEL_FUN_SEGOUT_0_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_8_SEL_FUN_DBG_DO_0_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_8_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_8_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_8_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_8_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_8_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_8_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_8_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_8_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_8_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_8_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_8_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_8_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_8_IE (1 << 17)
#define IOMUX2_PAD_GPIO_8_SE (1 << 18)

#define IOMUX2_PAD_GPIO_8_SEL_V_FUN_GPIO_8_SEL (0)
#define IOMUX2_PAD_GPIO_8_SEL_V_FUN_UART3_RXD_SEL (1)
#define IOMUX2_PAD_GPIO_8_SEL_V_FUN_SEGOUT_0_SEL (7)
#define IOMUX2_PAD_GPIO_8_SEL_V_FUN_DBG_DO_0_SEL (10)
#define IOMUX2_PAD_GPIO_8_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_8_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_8_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_8_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_9_cfg
#define IOMUX2_PAD_GPIO_9_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_9_SEL_FUN_GPIO_9_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_9_SEL_FUN_UART3_TXD_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_9_SEL_FUN_SEGOUT_1_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_9_SEL_FUN_DBG_DO_1_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_9_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_9_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_9_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_9_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_9_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_9_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_9_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_9_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_9_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_9_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_9_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_9_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_9_IE (1 << 17)
#define IOMUX2_PAD_GPIO_9_SE (1 << 18)

#define IOMUX2_PAD_GPIO_9_SEL_V_FUN_GPIO_9_SEL (0)
#define IOMUX2_PAD_GPIO_9_SEL_V_FUN_UART3_TXD_SEL (1)
#define IOMUX2_PAD_GPIO_9_SEL_V_FUN_SEGOUT_1_SEL (7)
#define IOMUX2_PAD_GPIO_9_SEL_V_FUN_DBG_DO_1_SEL (10)
#define IOMUX2_PAD_GPIO_9_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_9_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_9_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_9_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_10_cfg
#define IOMUX2_PAD_GPIO_10_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_10_SEL_FUN_GPIO_10_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_10_SEL_FUN_UART3_CTS_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_10_SEL_FUN_TCO_0_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_10_SEL_FUN_GPT2_PWM_0_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_10_SEL_FUN_GPT2_TI_0_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_10_SEL_FUN_CLKO_0_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_10_SEL_FUN_UART1_CTS_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_10_SEL_FUN_SEGOUT_2_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_10_SEL_FUN_UART1_TXD_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_10_SEL_FUN_DBG_DO_2_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_10_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_10_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_10_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_10_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_10_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_10_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_10_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_10_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_10_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_10_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_10_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_10_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_10_IE (1 << 17)
#define IOMUX2_PAD_GPIO_10_SE (1 << 18)

#define IOMUX2_PAD_GPIO_10_SEL_V_FUN_GPIO_10_SEL (0)
#define IOMUX2_PAD_GPIO_10_SEL_V_FUN_UART3_CTS_SEL (1)
#define IOMUX2_PAD_GPIO_10_SEL_V_FUN_TCO_0_SEL (2)
#define IOMUX2_PAD_GPIO_10_SEL_V_FUN_GPT2_PWM_0_SEL (3)
#define IOMUX2_PAD_GPIO_10_SEL_V_FUN_GPT2_TI_0_SEL (4)
#define IOMUX2_PAD_GPIO_10_SEL_V_FUN_CLKO_0_SEL (5)
#define IOMUX2_PAD_GPIO_10_SEL_V_FUN_UART1_CTS_SEL (6)
#define IOMUX2_PAD_GPIO_10_SEL_V_FUN_SEGOUT_2_SEL (7)
#define IOMUX2_PAD_GPIO_10_SEL_V_FUN_UART1_TXD_SEL (8)
#define IOMUX2_PAD_GPIO_10_SEL_V_FUN_DBG_DO_2_SEL (10)
#define IOMUX2_PAD_GPIO_10_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_10_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_10_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_10_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_11_cfg
#define IOMUX2_PAD_GPIO_11_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_11_SEL_FUN_GPIO_11_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_11_SEL_FUN_UART3_RTS_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_11_SEL_FUN_TCO_1_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_11_SEL_FUN_GPT2_PWM_1_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_11_SEL_FUN_GPT2_TI_1_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_11_SEL_FUN_CLKO_1_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_11_SEL_FUN_UART1_RTS_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_11_SEL_FUN_SEGOUT_3_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_11_SEL_FUN_UART2_TXD_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_11_SEL_FUN_GPADC_IN0_SEL (9 << 0)
#define IOMUX2_PAD_GPIO_11_SEL_FUN_DBG_DO_3_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_11_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_11_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_11_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_11_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_11_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_11_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_11_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_11_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_11_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_11_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_11_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_11_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_11_IE (1 << 17)
#define IOMUX2_PAD_GPIO_11_SE (1 << 18)

#define IOMUX2_PAD_GPIO_11_SEL_V_FUN_GPIO_11_SEL (0)
#define IOMUX2_PAD_GPIO_11_SEL_V_FUN_UART3_RTS_SEL (1)
#define IOMUX2_PAD_GPIO_11_SEL_V_FUN_TCO_1_SEL (2)
#define IOMUX2_PAD_GPIO_11_SEL_V_FUN_GPT2_PWM_1_SEL (3)
#define IOMUX2_PAD_GPIO_11_SEL_V_FUN_GPT2_TI_1_SEL (4)
#define IOMUX2_PAD_GPIO_11_SEL_V_FUN_CLKO_1_SEL (5)
#define IOMUX2_PAD_GPIO_11_SEL_V_FUN_UART1_RTS_SEL (6)
#define IOMUX2_PAD_GPIO_11_SEL_V_FUN_SEGOUT_3_SEL (7)
#define IOMUX2_PAD_GPIO_11_SEL_V_FUN_UART2_TXD_SEL (8)
#define IOMUX2_PAD_GPIO_11_SEL_V_FUN_GPADC_IN0_SEL (9)
#define IOMUX2_PAD_GPIO_11_SEL_V_FUN_DBG_DO_3_SEL (10)
#define IOMUX2_PAD_GPIO_11_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_11_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_11_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_11_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_12_cfg
#define IOMUX2_PAD_GPIO_12_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_12_SEL_FUN_GPIO_12_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_12_SEL_FUN_UART4_RXD_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_12_SEL_FUN_KEYIN_0_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_12_SEL_FUN_GPT2_TI_2_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_12_SEL_FUN_UART1_TXD_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_12_SEL_FUN_SEGOUT_4_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_12_SEL_FUN_UART1_CTS_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_12_SEL_FUN_DBG_DO_4_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_12_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_12_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_12_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_12_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_12_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_12_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_12_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_12_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_12_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_12_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_12_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_12_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_12_IE (1 << 17)
#define IOMUX2_PAD_GPIO_12_SE (1 << 18)

#define IOMUX2_PAD_GPIO_12_SEL_V_FUN_GPIO_12_SEL (0)
#define IOMUX2_PAD_GPIO_12_SEL_V_FUN_UART4_RXD_SEL (1)
#define IOMUX2_PAD_GPIO_12_SEL_V_FUN_KEYIN_0_SEL (3)
#define IOMUX2_PAD_GPIO_12_SEL_V_FUN_GPT2_TI_2_SEL (4)
#define IOMUX2_PAD_GPIO_12_SEL_V_FUN_UART1_TXD_SEL (6)
#define IOMUX2_PAD_GPIO_12_SEL_V_FUN_SEGOUT_4_SEL (7)
#define IOMUX2_PAD_GPIO_12_SEL_V_FUN_UART1_CTS_SEL (8)
#define IOMUX2_PAD_GPIO_12_SEL_V_FUN_DBG_DO_4_SEL (10)
#define IOMUX2_PAD_GPIO_12_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_12_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_12_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_12_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_13_cfg
#define IOMUX2_PAD_GPIO_13_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_13_SEL_FUN_GPIO_13_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_13_SEL_FUN_UART4_TXD_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_13_SEL_FUN_KEYIN_1_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_13_SEL_FUN_GPT2_TI_3_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_13_SEL_FUN_UART2_TXD_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_13_SEL_FUN_SEGOUT_5_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_13_SEL_FUN_UART1_RTS_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_13_SEL_FUN_DBG_DO_5_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_13_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_13_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_13_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_13_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_13_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_13_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_13_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_13_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_13_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_13_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_13_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_13_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_13_IE (1 << 17)
#define IOMUX2_PAD_GPIO_13_SE (1 << 18)

#define IOMUX2_PAD_GPIO_13_SEL_V_FUN_GPIO_13_SEL (0)
#define IOMUX2_PAD_GPIO_13_SEL_V_FUN_UART4_TXD_SEL (1)
#define IOMUX2_PAD_GPIO_13_SEL_V_FUN_KEYIN_1_SEL (3)
#define IOMUX2_PAD_GPIO_13_SEL_V_FUN_GPT2_TI_3_SEL (4)
#define IOMUX2_PAD_GPIO_13_SEL_V_FUN_UART2_TXD_SEL (6)
#define IOMUX2_PAD_GPIO_13_SEL_V_FUN_SEGOUT_5_SEL (7)
#define IOMUX2_PAD_GPIO_13_SEL_V_FUN_UART1_RTS_SEL (8)
#define IOMUX2_PAD_GPIO_13_SEL_V_FUN_DBG_DO_5_SEL (10)
#define IOMUX2_PAD_GPIO_13_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_13_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_13_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_13_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_14_cfg
#define IOMUX2_PAD_GPIO_14_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_14_SEL_FUN_GPIO_14_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_14_SEL_FUN_UART4_CTS_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_14_SEL_FUN_I2C2_SCL_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_14_SEL_FUN_KEYOUT_0_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_14_SEL_FUN_GPT2_PWM_0_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_14_SEL_FUN_GPT2_TI_0_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_14_SEL_FUN_SDMMC2_CLK_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_14_SEL_FUN_SEGOUT_6_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_14_SEL_FUN_UART2_CTS_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_14_SEL_FUN_DBG_DO_6_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_14_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_14_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_14_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_14_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_14_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_14_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_14_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_14_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_14_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_14_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_14_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_14_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_14_IE (1 << 17)
#define IOMUX2_PAD_GPIO_14_SE (1 << 18)

#define IOMUX2_PAD_GPIO_14_SEL_V_FUN_GPIO_14_SEL (0)
#define IOMUX2_PAD_GPIO_14_SEL_V_FUN_UART4_CTS_SEL (1)
#define IOMUX2_PAD_GPIO_14_SEL_V_FUN_I2C2_SCL_SEL (2)
#define IOMUX2_PAD_GPIO_14_SEL_V_FUN_KEYOUT_0_SEL (3)
#define IOMUX2_PAD_GPIO_14_SEL_V_FUN_GPT2_PWM_0_SEL (4)
#define IOMUX2_PAD_GPIO_14_SEL_V_FUN_GPT2_TI_0_SEL (5)
#define IOMUX2_PAD_GPIO_14_SEL_V_FUN_SDMMC2_CLK_SEL (6)
#define IOMUX2_PAD_GPIO_14_SEL_V_FUN_SEGOUT_6_SEL (7)
#define IOMUX2_PAD_GPIO_14_SEL_V_FUN_UART2_CTS_SEL (8)
#define IOMUX2_PAD_GPIO_14_SEL_V_FUN_DBG_DO_6_SEL (10)
#define IOMUX2_PAD_GPIO_14_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_14_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_14_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_14_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_15_cfg
#define IOMUX2_PAD_GPIO_15_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_15_SEL_FUN_GPIO_15_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_15_SEL_FUN_UART4_RTS_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_15_SEL_FUN_I2C2_SDA_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_15_SEL_FUN_KEYOUT_1_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_15_SEL_FUN_GPT2_PWM_1_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_15_SEL_FUN_GPT2_TI_1_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_15_SEL_FUN_SDMMC2_CMD_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_15_SEL_FUN_SEGOUT_7_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_15_SEL_FUN_UART2_RTS_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_15_SEL_FUN_GPADC_IN1_SEL (9 << 0)
#define IOMUX2_PAD_GPIO_15_SEL_FUN_DBG_DO_7_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_15_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_15_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_15_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_15_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_15_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_15_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_15_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_15_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_15_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_15_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_15_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_15_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_15_IE (1 << 17)
#define IOMUX2_PAD_GPIO_15_SE (1 << 18)

#define IOMUX2_PAD_GPIO_15_SEL_V_FUN_GPIO_15_SEL (0)
#define IOMUX2_PAD_GPIO_15_SEL_V_FUN_UART4_RTS_SEL (1)
#define IOMUX2_PAD_GPIO_15_SEL_V_FUN_I2C2_SDA_SEL (2)
#define IOMUX2_PAD_GPIO_15_SEL_V_FUN_KEYOUT_1_SEL (3)
#define IOMUX2_PAD_GPIO_15_SEL_V_FUN_GPT2_PWM_1_SEL (4)
#define IOMUX2_PAD_GPIO_15_SEL_V_FUN_GPT2_TI_1_SEL (5)
#define IOMUX2_PAD_GPIO_15_SEL_V_FUN_SDMMC2_CMD_SEL (6)
#define IOMUX2_PAD_GPIO_15_SEL_V_FUN_SEGOUT_7_SEL (7)
#define IOMUX2_PAD_GPIO_15_SEL_V_FUN_UART2_RTS_SEL (8)
#define IOMUX2_PAD_GPIO_15_SEL_V_FUN_GPADC_IN1_SEL (9)
#define IOMUX2_PAD_GPIO_15_SEL_V_FUN_DBG_DO_7_SEL (10)
#define IOMUX2_PAD_GPIO_15_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_15_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_15_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_15_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_16_cfg
#define IOMUX2_PAD_GPIO_16_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_16_SEL_FUN_GPIO_16_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_16_SEL_FUN_UART5_RXD_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_16_SEL_FUN_CLKO_2_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_16_SEL_FUN_SPI1_CLK_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_16_SEL_FUN_GPT2_PWM_2_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_16_SEL_FUN_I2S_BCLK_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_16_SEL_FUN_SDMMC2_DAT_3_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_16_SEL_FUN_SEGOUT_8_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_16_SEL_FUN_DBG_DO_8_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_16_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_16_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_16_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_16_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_16_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_16_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_16_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_16_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_16_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_16_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_16_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_16_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_16_IE (1 << 17)
#define IOMUX2_PAD_GPIO_16_SE (1 << 18)

#define IOMUX2_PAD_GPIO_16_SEL_V_FUN_GPIO_16_SEL (0)
#define IOMUX2_PAD_GPIO_16_SEL_V_FUN_UART5_RXD_SEL (1)
#define IOMUX2_PAD_GPIO_16_SEL_V_FUN_CLKO_2_SEL (2)
#define IOMUX2_PAD_GPIO_16_SEL_V_FUN_SPI1_CLK_SEL (3)
#define IOMUX2_PAD_GPIO_16_SEL_V_FUN_GPT2_PWM_2_SEL (4)
#define IOMUX2_PAD_GPIO_16_SEL_V_FUN_I2S_BCLK_SEL (5)
#define IOMUX2_PAD_GPIO_16_SEL_V_FUN_SDMMC2_DAT_3_SEL (6)
#define IOMUX2_PAD_GPIO_16_SEL_V_FUN_SEGOUT_8_SEL (7)
#define IOMUX2_PAD_GPIO_16_SEL_V_FUN_DBG_DO_8_SEL (10)
#define IOMUX2_PAD_GPIO_16_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_16_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_16_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_16_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_17_cfg
#define IOMUX2_PAD_GPIO_17_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_17_SEL_FUN_GPIO_17_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_17_SEL_FUN_UART5_TXD_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_17_SEL_FUN_CLKO_3_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_17_SEL_FUN_SPI1_CS_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_17_SEL_FUN_GPT2_PWM_3_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_17_SEL_FUN_I2S_LRCK_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_17_SEL_FUN_SDMMC2_DAT_2_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_17_SEL_FUN_SEGOUT_9_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_17_SEL_FUN_DBG_DO_9_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_17_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_17_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_17_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_17_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_17_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_17_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_17_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_17_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_17_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_17_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_17_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_17_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_17_IE (1 << 17)
#define IOMUX2_PAD_GPIO_17_SE (1 << 18)

#define IOMUX2_PAD_GPIO_17_SEL_V_FUN_GPIO_17_SEL (0)
#define IOMUX2_PAD_GPIO_17_SEL_V_FUN_UART5_TXD_SEL (1)
#define IOMUX2_PAD_GPIO_17_SEL_V_FUN_CLKO_3_SEL (2)
#define IOMUX2_PAD_GPIO_17_SEL_V_FUN_SPI1_CS_SEL (3)
#define IOMUX2_PAD_GPIO_17_SEL_V_FUN_GPT2_PWM_3_SEL (4)
#define IOMUX2_PAD_GPIO_17_SEL_V_FUN_I2S_LRCK_SEL (5)
#define IOMUX2_PAD_GPIO_17_SEL_V_FUN_SDMMC2_DAT_2_SEL (6)
#define IOMUX2_PAD_GPIO_17_SEL_V_FUN_SEGOUT_9_SEL (7)
#define IOMUX2_PAD_GPIO_17_SEL_V_FUN_DBG_DO_9_SEL (10)
#define IOMUX2_PAD_GPIO_17_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_17_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_17_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_17_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_18_cfg
#define IOMUX2_PAD_GPIO_18_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_18_SEL_FUN_GPIO_18_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_18_SEL_FUN_UART5_CTS_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_18_SEL_FUN_I2C2_SCL_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_18_SEL_FUN_SPI1_DI_0_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_18_SEL_FUN_GPT2_PWM_0_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_18_SEL_FUN_I2S_DI_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_18_SEL_FUN_SDMMC2_DAT_1_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_18_SEL_FUN_SEGOUT_10_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_18_SEL_FUN_DBG_DO_10_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_18_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_18_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_18_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_18_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_18_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_18_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_18_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_18_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_18_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_18_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_18_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_18_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_18_IE (1 << 17)
#define IOMUX2_PAD_GPIO_18_SE (1 << 18)

#define IOMUX2_PAD_GPIO_18_SEL_V_FUN_GPIO_18_SEL (0)
#define IOMUX2_PAD_GPIO_18_SEL_V_FUN_UART5_CTS_SEL (1)
#define IOMUX2_PAD_GPIO_18_SEL_V_FUN_I2C2_SCL_SEL (2)
#define IOMUX2_PAD_GPIO_18_SEL_V_FUN_SPI1_DI_0_SEL (3)
#define IOMUX2_PAD_GPIO_18_SEL_V_FUN_GPT2_PWM_0_SEL (4)
#define IOMUX2_PAD_GPIO_18_SEL_V_FUN_I2S_DI_SEL (5)
#define IOMUX2_PAD_GPIO_18_SEL_V_FUN_SDMMC2_DAT_1_SEL (6)
#define IOMUX2_PAD_GPIO_18_SEL_V_FUN_SEGOUT_10_SEL (7)
#define IOMUX2_PAD_GPIO_18_SEL_V_FUN_DBG_DO_10_SEL (10)
#define IOMUX2_PAD_GPIO_18_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_18_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_18_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_18_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_19_cfg
#define IOMUX2_PAD_GPIO_19_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_19_SEL_FUN_GPIO_19_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_19_SEL_FUN_UART5_RTS_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_19_SEL_FUN_I2C2_SDA_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_19_SEL_FUN_SPI1_DI_1_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_19_SEL_FUN_GPT2_PWM_1_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_19_SEL_FUN_I2S_DO_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_19_SEL_FUN_SDMMC2_DAT_0_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_19_SEL_FUN_SEGOUT_11_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_19_SEL_FUN_SWV_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_19_SEL_FUN_GPADC_IN2_SEL (9 << 0)
#define IOMUX2_PAD_GPIO_19_SEL_FUN_DBG_DO_11_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_19_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_19_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_19_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_19_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_19_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_19_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_19_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_19_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_19_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_19_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_19_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_19_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_19_IE (1 << 17)
#define IOMUX2_PAD_GPIO_19_SE (1 << 18)

#define IOMUX2_PAD_GPIO_19_SEL_V_FUN_GPIO_19_SEL (0)
#define IOMUX2_PAD_GPIO_19_SEL_V_FUN_UART5_RTS_SEL (1)
#define IOMUX2_PAD_GPIO_19_SEL_V_FUN_I2C2_SDA_SEL (2)
#define IOMUX2_PAD_GPIO_19_SEL_V_FUN_SPI1_DI_1_SEL (3)
#define IOMUX2_PAD_GPIO_19_SEL_V_FUN_GPT2_PWM_1_SEL (4)
#define IOMUX2_PAD_GPIO_19_SEL_V_FUN_I2S_DO_SEL (5)
#define IOMUX2_PAD_GPIO_19_SEL_V_FUN_SDMMC2_DAT_0_SEL (6)
#define IOMUX2_PAD_GPIO_19_SEL_V_FUN_SEGOUT_11_SEL (7)
#define IOMUX2_PAD_GPIO_19_SEL_V_FUN_SWV_SEL (8)
#define IOMUX2_PAD_GPIO_19_SEL_V_FUN_GPADC_IN2_SEL (9)
#define IOMUX2_PAD_GPIO_19_SEL_V_FUN_DBG_DO_11_SEL (10)
#define IOMUX2_PAD_GPIO_19_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_19_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_19_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_19_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_20_cfg
#define IOMUX2_PAD_GPIO_20_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_20_SEL_FUN_GPIO_20_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_20_SEL_FUN_SPI1_CLK_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_20_SEL_FUN_M2_SPI_CLK_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_20_SEL_FUN_KEYIN_0_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_20_SEL_FUN_CLKO_0_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_20_SEL_FUN_SDMMC2_DAT_3_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_20_SEL_FUN_SEGOUT_12_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_20_SEL_FUN_DBG_DO_12_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_20_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_20_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_20_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_20_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_20_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_20_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_20_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_20_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_20_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_20_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_20_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_20_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_20_IE (1 << 17)
#define IOMUX2_PAD_GPIO_20_SE (1 << 18)

#define IOMUX2_PAD_GPIO_20_SEL_V_FUN_GPIO_20_SEL (0)
#define IOMUX2_PAD_GPIO_20_SEL_V_FUN_SPI1_CLK_SEL (1)
#define IOMUX2_PAD_GPIO_20_SEL_V_FUN_M2_SPI_CLK_SEL (2)
#define IOMUX2_PAD_GPIO_20_SEL_V_FUN_KEYIN_0_SEL (3)
#define IOMUX2_PAD_GPIO_20_SEL_V_FUN_CLKO_0_SEL (4)
#define IOMUX2_PAD_GPIO_20_SEL_V_FUN_SDMMC2_DAT_3_SEL (6)
#define IOMUX2_PAD_GPIO_20_SEL_V_FUN_SEGOUT_12_SEL (7)
#define IOMUX2_PAD_GPIO_20_SEL_V_FUN_DBG_DO_12_SEL (10)
#define IOMUX2_PAD_GPIO_20_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_20_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_20_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_20_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_21_cfg
#define IOMUX2_PAD_GPIO_21_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_21_SEL_FUN_GPIO_21_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_21_SEL_FUN_SPI1_CS_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_21_SEL_FUN_M2_SPI_CS_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_21_SEL_FUN_KEYIN_1_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_21_SEL_FUN_CLKO_1_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_21_SEL_FUN_SDMMC2_DAT_2_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_21_SEL_FUN_SEGOUT_13_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_21_SEL_FUN_DBG_DO_13_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_21_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_21_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_21_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_21_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_21_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_21_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_21_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_21_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_21_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_21_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_21_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_21_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_21_IE (1 << 17)
#define IOMUX2_PAD_GPIO_21_SE (1 << 18)

#define IOMUX2_PAD_GPIO_21_SEL_V_FUN_GPIO_21_SEL (0)
#define IOMUX2_PAD_GPIO_21_SEL_V_FUN_SPI1_CS_SEL (1)
#define IOMUX2_PAD_GPIO_21_SEL_V_FUN_M2_SPI_CS_SEL (2)
#define IOMUX2_PAD_GPIO_21_SEL_V_FUN_KEYIN_1_SEL (3)
#define IOMUX2_PAD_GPIO_21_SEL_V_FUN_CLKO_1_SEL (4)
#define IOMUX2_PAD_GPIO_21_SEL_V_FUN_SDMMC2_DAT_2_SEL (6)
#define IOMUX2_PAD_GPIO_21_SEL_V_FUN_SEGOUT_13_SEL (7)
#define IOMUX2_PAD_GPIO_21_SEL_V_FUN_DBG_DO_13_SEL (10)
#define IOMUX2_PAD_GPIO_21_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_21_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_21_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_21_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_22_cfg
#define IOMUX2_PAD_GPIO_22_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_22_SEL_FUN_GPIO_22_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_22_SEL_FUN_SPI1_DI_0_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_22_SEL_FUN_M2_SPI_D_0_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_22_SEL_FUN_KEYOUT_0_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_22_SEL_FUN_CLKO_2_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_22_SEL_FUN_SDMMC2_DAT_1_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_22_SEL_FUN_SEGOUT_14_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_22_SEL_FUN_DBG_DO_14_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_22_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_22_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_22_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_22_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_22_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_22_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_22_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_22_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_22_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_22_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_22_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_22_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_22_IE (1 << 17)
#define IOMUX2_PAD_GPIO_22_SE (1 << 18)

#define IOMUX2_PAD_GPIO_22_SEL_V_FUN_GPIO_22_SEL (0)
#define IOMUX2_PAD_GPIO_22_SEL_V_FUN_SPI1_DI_0_SEL (1)
#define IOMUX2_PAD_GPIO_22_SEL_V_FUN_M2_SPI_D_0_SEL (2)
#define IOMUX2_PAD_GPIO_22_SEL_V_FUN_KEYOUT_0_SEL (3)
#define IOMUX2_PAD_GPIO_22_SEL_V_FUN_CLKO_2_SEL (4)
#define IOMUX2_PAD_GPIO_22_SEL_V_FUN_SDMMC2_DAT_1_SEL (6)
#define IOMUX2_PAD_GPIO_22_SEL_V_FUN_SEGOUT_14_SEL (7)
#define IOMUX2_PAD_GPIO_22_SEL_V_FUN_DBG_DO_14_SEL (10)
#define IOMUX2_PAD_GPIO_22_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_22_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_22_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_22_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_23_cfg
#define IOMUX2_PAD_GPIO_23_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_23_SEL_FUN_GPIO_23_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_23_SEL_FUN_SPI1_DI_1_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_23_SEL_FUN_M2_SPI_D_1_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_23_SEL_FUN_KEYOUT_1_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_23_SEL_FUN_CLKO_3_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_23_SEL_FUN_TCO_0_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_23_SEL_FUN_SDMMC2_DAT_0_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_23_SEL_FUN_SEGOUT_15_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_23_SEL_FUN_SWV_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_23_SEL_FUN_GPADC_IN3_SEL (9 << 0)
#define IOMUX2_PAD_GPIO_23_SEL_FUN_DBG_DO_15_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_23_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_23_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_23_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_23_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_23_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_23_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_23_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_23_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_23_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_23_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_23_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_23_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_23_IE (1 << 17)
#define IOMUX2_PAD_GPIO_23_SE (1 << 18)

#define IOMUX2_PAD_GPIO_23_SEL_V_FUN_GPIO_23_SEL (0)
#define IOMUX2_PAD_GPIO_23_SEL_V_FUN_SPI1_DI_1_SEL (1)
#define IOMUX2_PAD_GPIO_23_SEL_V_FUN_M2_SPI_D_1_SEL (2)
#define IOMUX2_PAD_GPIO_23_SEL_V_FUN_KEYOUT_1_SEL (3)
#define IOMUX2_PAD_GPIO_23_SEL_V_FUN_CLKO_3_SEL (4)
#define IOMUX2_PAD_GPIO_23_SEL_V_FUN_TCO_0_SEL (5)
#define IOMUX2_PAD_GPIO_23_SEL_V_FUN_SDMMC2_DAT_0_SEL (6)
#define IOMUX2_PAD_GPIO_23_SEL_V_FUN_SEGOUT_15_SEL (7)
#define IOMUX2_PAD_GPIO_23_SEL_V_FUN_SWV_SEL (8)
#define IOMUX2_PAD_GPIO_23_SEL_V_FUN_GPADC_IN3_SEL (9)
#define IOMUX2_PAD_GPIO_23_SEL_V_FUN_DBG_DO_15_SEL (10)
#define IOMUX2_PAD_GPIO_23_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_23_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_23_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_23_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_24_cfg
#define IOMUX2_PAD_GPIO_24_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_24_SEL_FUN_GPIO_24_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_24_SEL_FUN_I2C1_SCL_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_24_SEL_FUN_M2_SPI_D_2_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_24_SEL_FUN_GPT2_PWM_0_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_24_SEL_FUN_GPT2_TI_2_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_24_SEL_FUN_TCO_1_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_24_SEL_FUN_SDMMC2_CLK_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_24_SEL_FUN_SEGOUT_16_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_24_SEL_FUN_DBG_TRIG_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_24_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_24_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_24_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_24_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_24_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_24_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_24_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_24_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_24_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_24_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_24_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_24_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_24_IE (1 << 17)
#define IOMUX2_PAD_GPIO_24_SE (1 << 18)

#define IOMUX2_PAD_GPIO_24_SEL_V_FUN_GPIO_24_SEL (0)
#define IOMUX2_PAD_GPIO_24_SEL_V_FUN_I2C1_SCL_SEL (1)
#define IOMUX2_PAD_GPIO_24_SEL_V_FUN_M2_SPI_D_2_SEL (2)
#define IOMUX2_PAD_GPIO_24_SEL_V_FUN_GPT2_PWM_0_SEL (3)
#define IOMUX2_PAD_GPIO_24_SEL_V_FUN_GPT2_TI_2_SEL (4)
#define IOMUX2_PAD_GPIO_24_SEL_V_FUN_TCO_1_SEL (5)
#define IOMUX2_PAD_GPIO_24_SEL_V_FUN_SDMMC2_CLK_SEL (6)
#define IOMUX2_PAD_GPIO_24_SEL_V_FUN_SEGOUT_16_SEL (7)
#define IOMUX2_PAD_GPIO_24_SEL_V_FUN_DBG_TRIG_SEL (10)
#define IOMUX2_PAD_GPIO_24_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_24_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_24_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_24_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_25_cfg
#define IOMUX2_PAD_GPIO_25_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_25_SEL_FUN_GPIO_25_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_25_SEL_FUN_I2C1_SDA_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_25_SEL_FUN_M2_SPI_D_3_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_25_SEL_FUN_GPT2_PWM_1_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_25_SEL_FUN_GPT2_TI_3_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_25_SEL_FUN_TCO_0_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_25_SEL_FUN_SDMMC2_CMD_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_25_SEL_FUN_SEGOUT_17_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_25_SEL_FUN_DBG_CLK_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_25_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_25_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_25_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_25_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_25_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_25_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_25_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_25_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_25_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_25_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_25_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_25_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_25_IE (1 << 17)
#define IOMUX2_PAD_GPIO_25_SE (1 << 18)

#define IOMUX2_PAD_GPIO_25_SEL_V_FUN_GPIO_25_SEL (0)
#define IOMUX2_PAD_GPIO_25_SEL_V_FUN_I2C1_SDA_SEL (1)
#define IOMUX2_PAD_GPIO_25_SEL_V_FUN_M2_SPI_D_3_SEL (2)
#define IOMUX2_PAD_GPIO_25_SEL_V_FUN_GPT2_PWM_1_SEL (3)
#define IOMUX2_PAD_GPIO_25_SEL_V_FUN_GPT2_TI_3_SEL (4)
#define IOMUX2_PAD_GPIO_25_SEL_V_FUN_TCO_0_SEL (5)
#define IOMUX2_PAD_GPIO_25_SEL_V_FUN_SDMMC2_CMD_SEL (6)
#define IOMUX2_PAD_GPIO_25_SEL_V_FUN_SEGOUT_17_SEL (7)
#define IOMUX2_PAD_GPIO_25_SEL_V_FUN_DBG_CLK_SEL (10)
#define IOMUX2_PAD_GPIO_25_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_25_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_25_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_25_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_26_cfg
#define IOMUX2_PAD_GPIO_26_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_26_SEL_FUN_GPIO_26_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_26_SEL_FUN_SDMMC2_CLK_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_26_SEL_FUN_UART3_CTS_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_26_SEL_FUN_CLKO_2_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_26_SEL_FUN_GPT2_PWM_2_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_26_SEL_FUN_TCO_0_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_26_SEL_FUN_M2_SPI_CLK_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_26_SEL_FUN_UART4_CTS_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_26_SEL_FUN_ADI_SCL_M_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_26_SEL_FUN_SWV_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_26_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_26_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_26_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_26_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_26_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_26_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_26_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_26_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_26_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_26_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_26_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_26_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_26_IE (1 << 17)
#define IOMUX2_PAD_GPIO_26_SE (1 << 18)

#define IOMUX2_PAD_GPIO_26_SEL_V_FUN_GPIO_26_SEL (0)
#define IOMUX2_PAD_GPIO_26_SEL_V_FUN_SDMMC2_CLK_SEL (1)
#define IOMUX2_PAD_GPIO_26_SEL_V_FUN_UART3_CTS_SEL (2)
#define IOMUX2_PAD_GPIO_26_SEL_V_FUN_CLKO_2_SEL (3)
#define IOMUX2_PAD_GPIO_26_SEL_V_FUN_GPT2_PWM_2_SEL (4)
#define IOMUX2_PAD_GPIO_26_SEL_V_FUN_TCO_0_SEL (5)
#define IOMUX2_PAD_GPIO_26_SEL_V_FUN_M2_SPI_CLK_SEL (6)
#define IOMUX2_PAD_GPIO_26_SEL_V_FUN_UART4_CTS_SEL (7)
#define IOMUX2_PAD_GPIO_26_SEL_V_FUN_ADI_SCL_M_SEL (8)
#define IOMUX2_PAD_GPIO_26_SEL_V_FUN_SWV_SEL (10)
#define IOMUX2_PAD_GPIO_26_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_26_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_26_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_26_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_27_cfg
#define IOMUX2_PAD_GPIO_27_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_27_SEL_FUN_GPIO_27_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_27_SEL_FUN_SDMMC2_CMD_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_27_SEL_FUN_UART3_RTS_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_27_SEL_FUN_CLKO_3_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_27_SEL_FUN_GPT2_PWM_3_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_27_SEL_FUN_TCO_1_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_27_SEL_FUN_M2_SPI_CS_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_27_SEL_FUN_UART4_RTS_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_27_SEL_FUN_ADI_SDIO_M_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_27_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_27_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_27_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_27_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_27_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_27_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_27_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_27_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_27_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_27_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_27_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_27_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_27_IE (1 << 17)
#define IOMUX2_PAD_GPIO_27_SE (1 << 18)

#define IOMUX2_PAD_GPIO_27_SEL_V_FUN_GPIO_27_SEL (0)
#define IOMUX2_PAD_GPIO_27_SEL_V_FUN_SDMMC2_CMD_SEL (1)
#define IOMUX2_PAD_GPIO_27_SEL_V_FUN_UART3_RTS_SEL (2)
#define IOMUX2_PAD_GPIO_27_SEL_V_FUN_CLKO_3_SEL (3)
#define IOMUX2_PAD_GPIO_27_SEL_V_FUN_GPT2_PWM_3_SEL (4)
#define IOMUX2_PAD_GPIO_27_SEL_V_FUN_TCO_1_SEL (5)
#define IOMUX2_PAD_GPIO_27_SEL_V_FUN_M2_SPI_CS_SEL (6)
#define IOMUX2_PAD_GPIO_27_SEL_V_FUN_UART4_RTS_SEL (7)
#define IOMUX2_PAD_GPIO_27_SEL_V_FUN_ADI_SDIO_M_SEL (8)
#define IOMUX2_PAD_GPIO_27_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_27_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_27_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_27_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_28_cfg
#define IOMUX2_PAD_GPIO_28_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_28_SEL_FUN_GPIO_28_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_28_SEL_FUN_SDMMC2_DAT_0_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_28_SEL_FUN_SPI1_CLK_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_28_SEL_FUN_KEYIN_2_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_28_SEL_FUN_CLKO_0_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_28_SEL_FUN_I2S_BCLK_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_28_SEL_FUN_M2_SPI_D_0_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_28_SEL_FUN_UART4_RXD_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_28_SEL_FUN_RF_SPI_CLK_M_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_28_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_28_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_28_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_28_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_28_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_28_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_28_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_28_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_28_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_28_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_28_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_28_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_28_IE (1 << 17)
#define IOMUX2_PAD_GPIO_28_SE (1 << 18)

#define IOMUX2_PAD_GPIO_28_SEL_V_FUN_GPIO_28_SEL (0)
#define IOMUX2_PAD_GPIO_28_SEL_V_FUN_SDMMC2_DAT_0_SEL (1)
#define IOMUX2_PAD_GPIO_28_SEL_V_FUN_SPI1_CLK_SEL (2)
#define IOMUX2_PAD_GPIO_28_SEL_V_FUN_KEYIN_2_SEL (3)
#define IOMUX2_PAD_GPIO_28_SEL_V_FUN_CLKO_0_SEL (4)
#define IOMUX2_PAD_GPIO_28_SEL_V_FUN_I2S_BCLK_SEL (5)
#define IOMUX2_PAD_GPIO_28_SEL_V_FUN_M2_SPI_D_0_SEL (6)
#define IOMUX2_PAD_GPIO_28_SEL_V_FUN_UART4_RXD_SEL (7)
#define IOMUX2_PAD_GPIO_28_SEL_V_FUN_RF_SPI_CLK_M_SEL (8)
#define IOMUX2_PAD_GPIO_28_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_28_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_28_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_28_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_29_cfg
#define IOMUX2_PAD_GPIO_29_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_29_SEL_FUN_GPIO_29_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_29_SEL_FUN_SDMMC2_DAT_1_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_29_SEL_FUN_SPI1_CS_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_29_SEL_FUN_KEYIN_3_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_29_SEL_FUN_CLKO_1_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_29_SEL_FUN_I2S_LRCK_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_29_SEL_FUN_M2_SPI_D_1_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_29_SEL_FUN_UART4_TXD_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_29_SEL_FUN_RF_SPI_CS_M_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_29_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_29_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_29_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_29_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_29_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_29_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_29_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_29_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_29_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_29_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_29_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_29_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_29_IE (1 << 17)
#define IOMUX2_PAD_GPIO_29_SE (1 << 18)

#define IOMUX2_PAD_GPIO_29_SEL_V_FUN_GPIO_29_SEL (0)
#define IOMUX2_PAD_GPIO_29_SEL_V_FUN_SDMMC2_DAT_1_SEL (1)
#define IOMUX2_PAD_GPIO_29_SEL_V_FUN_SPI1_CS_SEL (2)
#define IOMUX2_PAD_GPIO_29_SEL_V_FUN_KEYIN_3_SEL (3)
#define IOMUX2_PAD_GPIO_29_SEL_V_FUN_CLKO_1_SEL (4)
#define IOMUX2_PAD_GPIO_29_SEL_V_FUN_I2S_LRCK_SEL (5)
#define IOMUX2_PAD_GPIO_29_SEL_V_FUN_M2_SPI_D_1_SEL (6)
#define IOMUX2_PAD_GPIO_29_SEL_V_FUN_UART4_TXD_SEL (7)
#define IOMUX2_PAD_GPIO_29_SEL_V_FUN_RF_SPI_CS_M_SEL (8)
#define IOMUX2_PAD_GPIO_29_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_29_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_29_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_29_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_30_cfg
#define IOMUX2_PAD_GPIO_30_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_30_SEL_FUN_GPIO_30_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_30_SEL_FUN_SDMMC2_DAT_2_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_30_SEL_FUN_SPI1_DI_0_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_30_SEL_FUN_KEYOUT_2_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_30_SEL_FUN_CLKO_2_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_30_SEL_FUN_I2S_DI_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_30_SEL_FUN_M2_SPI_D_2_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_30_SEL_FUN_UART3_CTS_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_30_SEL_FUN_RF_SPI_DIO_M_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_30_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_30_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_30_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_30_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_30_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_30_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_30_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_30_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_30_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_30_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_30_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_30_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_30_IE (1 << 17)
#define IOMUX2_PAD_GPIO_30_SE (1 << 18)

#define IOMUX2_PAD_GPIO_30_SEL_V_FUN_GPIO_30_SEL (0)
#define IOMUX2_PAD_GPIO_30_SEL_V_FUN_SDMMC2_DAT_2_SEL (1)
#define IOMUX2_PAD_GPIO_30_SEL_V_FUN_SPI1_DI_0_SEL (2)
#define IOMUX2_PAD_GPIO_30_SEL_V_FUN_KEYOUT_2_SEL (3)
#define IOMUX2_PAD_GPIO_30_SEL_V_FUN_CLKO_2_SEL (4)
#define IOMUX2_PAD_GPIO_30_SEL_V_FUN_I2S_DI_SEL (5)
#define IOMUX2_PAD_GPIO_30_SEL_V_FUN_M2_SPI_D_2_SEL (6)
#define IOMUX2_PAD_GPIO_30_SEL_V_FUN_UART3_CTS_SEL (7)
#define IOMUX2_PAD_GPIO_30_SEL_V_FUN_RF_SPI_DIO_M_SEL (8)
#define IOMUX2_PAD_GPIO_30_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_30_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_30_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_30_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_31_cfg
#define IOMUX2_PAD_GPIO_31_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_31_SEL_FUN_GPIO_31_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_31_SEL_FUN_SDMMC2_DAT_3_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_31_SEL_FUN_SPI1_DI_1_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_31_SEL_FUN_KEYOUT_3_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_31_SEL_FUN_CLKO_3_SEL (4 << 0)
#define IOMUX2_PAD_GPIO_31_SEL_FUN_I2S_DO_SEL (5 << 0)
#define IOMUX2_PAD_GPIO_31_SEL_FUN_M2_SPI_D_3_SEL (6 << 0)
#define IOMUX2_PAD_GPIO_31_SEL_FUN_UART3_RTS_SEL (7 << 0)
#define IOMUX2_PAD_GPIO_31_SEL_FUN_DFE_RX_EN_M_SEL (8 << 0)
#define IOMUX2_PAD_GPIO_31_SEL_FUN_SWV_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_31_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_31_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_31_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_31_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_31_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_31_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_31_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_31_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_31_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_31_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_31_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_31_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_31_IE (1 << 17)
#define IOMUX2_PAD_GPIO_31_SE (1 << 18)

#define IOMUX2_PAD_GPIO_31_SEL_V_FUN_GPIO_31_SEL (0)
#define IOMUX2_PAD_GPIO_31_SEL_V_FUN_SDMMC2_DAT_3_SEL (1)
#define IOMUX2_PAD_GPIO_31_SEL_V_FUN_SPI1_DI_1_SEL (2)
#define IOMUX2_PAD_GPIO_31_SEL_V_FUN_KEYOUT_3_SEL (3)
#define IOMUX2_PAD_GPIO_31_SEL_V_FUN_CLKO_3_SEL (4)
#define IOMUX2_PAD_GPIO_31_SEL_V_FUN_I2S_DO_SEL (5)
#define IOMUX2_PAD_GPIO_31_SEL_V_FUN_M2_SPI_D_3_SEL (6)
#define IOMUX2_PAD_GPIO_31_SEL_V_FUN_UART3_RTS_SEL (7)
#define IOMUX2_PAD_GPIO_31_SEL_V_FUN_DFE_RX_EN_M_SEL (8)
#define IOMUX2_PAD_GPIO_31_SEL_V_FUN_SWV_SEL (10)
#define IOMUX2_PAD_GPIO_31_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_31_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_31_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_31_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_32_cfg
#define IOMUX2_PAD_GPIO_32_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_32_SEL_FUN_GPIO_32_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_32_SEL_FUN_HST_RXD_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_32_SEL_FUN_SPI1_CLK_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_32_SEL_FUN_UART5_RXD_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_32_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_32_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_32_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_32_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_32_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_32_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_32_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_32_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_32_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_32_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_32_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_32_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_32_IE (1 << 17)
#define IOMUX2_PAD_GPIO_32_SE (1 << 18)

#define IOMUX2_PAD_GPIO_32_SEL_V_FUN_GPIO_32_SEL (0)
#define IOMUX2_PAD_GPIO_32_SEL_V_FUN_HST_RXD_SEL (1)
#define IOMUX2_PAD_GPIO_32_SEL_V_FUN_SPI1_CLK_SEL (2)
#define IOMUX2_PAD_GPIO_32_SEL_V_FUN_UART5_RXD_SEL (3)
#define IOMUX2_PAD_GPIO_32_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_32_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_32_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_32_PULL_UP_V_PULL_UP_1_8K (3)

// pad_gpio_33_cfg
#define IOMUX2_PAD_GPIO_33_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_GPIO_33_SEL_FUN_GPIO_33_SEL (0 << 0)
#define IOMUX2_PAD_GPIO_33_SEL_FUN_HST_TXD_SEL (1 << 0)
#define IOMUX2_PAD_GPIO_33_SEL_FUN_SPI1_CS_SEL (2 << 0)
#define IOMUX2_PAD_GPIO_33_SEL_FUN_UART5_TXD_SEL (3 << 0)
#define IOMUX2_PAD_GPIO_33_SEL_FUN_SWV_SEL (10 << 0)
#define IOMUX2_PAD_GPIO_33_OUT_REG (1 << 4)
#define IOMUX2_PAD_GPIO_33_OUT_FRC (1 << 5)
#define IOMUX2_PAD_GPIO_33_OEN_REG (1 << 6)
#define IOMUX2_PAD_GPIO_33_OEN_FRC (1 << 7)
#define IOMUX2_PAD_GPIO_33_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_GPIO_33_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_GPIO_33_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_GPIO_33_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_GPIO_33_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_GPIO_33_PULL_DN (1 << 10)
#define IOMUX2_PAD_GPIO_33_PULL_FRC (1 << 11)
#define IOMUX2_PAD_GPIO_33_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_GPIO_33_IE (1 << 17)
#define IOMUX2_PAD_GPIO_33_SE (1 << 18)

#define IOMUX2_PAD_GPIO_33_SEL_V_FUN_GPIO_33_SEL (0)
#define IOMUX2_PAD_GPIO_33_SEL_V_FUN_HST_TXD_SEL (1)
#define IOMUX2_PAD_GPIO_33_SEL_V_FUN_SPI1_CS_SEL (2)
#define IOMUX2_PAD_GPIO_33_SEL_V_FUN_UART5_TXD_SEL (3)
#define IOMUX2_PAD_GPIO_33_SEL_V_FUN_SWV_SEL (10)
#define IOMUX2_PAD_GPIO_33_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_GPIO_33_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_GPIO_33_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_GPIO_33_PULL_UP_V_PULL_UP_1_8K (3)

// pad_swclk_cfg
#define IOMUX2_PAD_SWCLK_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_SWCLK_SEL_FUN_SWCLK_SEL (0 << 0)
#define IOMUX2_PAD_SWCLK_SEL_FUN_GPIO_34_SEL (1 << 0)
#define IOMUX2_PAD_SWCLK_SEL_FUN_SPI1_DI_0_SEL (2 << 0)
#define IOMUX2_PAD_SWCLK_SEL_FUN_UART5_CTS_SEL (3 << 0)
#define IOMUX2_PAD_SWCLK_OUT_REG (1 << 4)
#define IOMUX2_PAD_SWCLK_OUT_FRC (1 << 5)
#define IOMUX2_PAD_SWCLK_OEN_REG (1 << 6)
#define IOMUX2_PAD_SWCLK_OEN_FRC (1 << 7)
#define IOMUX2_PAD_SWCLK_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_SWCLK_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_SWCLK_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_SWCLK_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_SWCLK_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_SWCLK_PULL_DN (1 << 10)
#define IOMUX2_PAD_SWCLK_PULL_FRC (1 << 11)
#define IOMUX2_PAD_SWCLK_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_SWCLK_IE (1 << 17)
#define IOMUX2_PAD_SWCLK_SE (1 << 18)

#define IOMUX2_PAD_SWCLK_SEL_V_FUN_SWCLK_SEL (0)
#define IOMUX2_PAD_SWCLK_SEL_V_FUN_GPIO_34_SEL (1)
#define IOMUX2_PAD_SWCLK_SEL_V_FUN_SPI1_DI_0_SEL (2)
#define IOMUX2_PAD_SWCLK_SEL_V_FUN_UART5_CTS_SEL (3)
#define IOMUX2_PAD_SWCLK_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_SWCLK_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_SWCLK_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_SWCLK_PULL_UP_V_PULL_UP_1_8K (3)

// pad_swdio_cfg
#define IOMUX2_PAD_SWDIO_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_SWDIO_SEL_FUN_SWDIO_SEL (0 << 0)
#define IOMUX2_PAD_SWDIO_SEL_FUN_GPIO_35_SEL (1 << 0)
#define IOMUX2_PAD_SWDIO_SEL_FUN_SPI1_DI_1_SEL (2 << 0)
#define IOMUX2_PAD_SWDIO_SEL_FUN_UART5_RTS_SEL (3 << 0)
#define IOMUX2_PAD_SWDIO_OUT_REG (1 << 4)
#define IOMUX2_PAD_SWDIO_OUT_FRC (1 << 5)
#define IOMUX2_PAD_SWDIO_OEN_REG (1 << 6)
#define IOMUX2_PAD_SWDIO_OEN_FRC (1 << 7)
#define IOMUX2_PAD_SWDIO_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_SWDIO_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_SWDIO_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_SWDIO_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_SWDIO_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_SWDIO_PULL_DN (1 << 10)
#define IOMUX2_PAD_SWDIO_PULL_FRC (1 << 11)
#define IOMUX2_PAD_SWDIO_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_SWDIO_IE (1 << 17)
#define IOMUX2_PAD_SWDIO_SE (1 << 18)

#define IOMUX2_PAD_SWDIO_SEL_V_FUN_SWDIO_SEL (0)
#define IOMUX2_PAD_SWDIO_SEL_V_FUN_GPIO_35_SEL (1)
#define IOMUX2_PAD_SWDIO_SEL_V_FUN_SPI1_DI_1_SEL (2)
#define IOMUX2_PAD_SWDIO_SEL_V_FUN_UART5_RTS_SEL (3)
#define IOMUX2_PAD_SWDIO_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_SWDIO_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_SWDIO_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_SWDIO_PULL_UP_V_PULL_UP_1_8K (3)

// pad_sim_clk_0_cfg
#define IOMUX2_PAD_SIM_CLK_0_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_SIM_CLK_0_SEL_FUN_SIM_CLK_0_SEL (0 << 0)
#define IOMUX2_PAD_SIM_CLK_0_OUT_REG (1 << 4)
#define IOMUX2_PAD_SIM_CLK_0_OUT_FRC (1 << 5)
#define IOMUX2_PAD_SIM_CLK_0_OEN_REG (1 << 6)
#define IOMUX2_PAD_SIM_CLK_0_OEN_FRC (1 << 7)
#define IOMUX2_PAD_SIM_CLK_0_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_SIM_CLK_0_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_SIM_CLK_0_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_SIM_CLK_0_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_SIM_CLK_0_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_SIM_CLK_0_PULL_DN (1 << 10)
#define IOMUX2_PAD_SIM_CLK_0_PULL_FRC (1 << 11)
#define IOMUX2_PAD_SIM_CLK_0_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_SIM_CLK_0_IE (1 << 17)
#define IOMUX2_PAD_SIM_CLK_0_SE (1 << 18)

#define IOMUX2_PAD_SIM_CLK_0_SEL_V_FUN_SIM_CLK_0_SEL (0)
#define IOMUX2_PAD_SIM_CLK_0_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_SIM_CLK_0_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_SIM_CLK_0_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_SIM_CLK_0_PULL_UP_V_PULL_UP_1_8K (3)

// pad_sim_rst_0_cfg
#define IOMUX2_PAD_SIM_RST_0_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_SIM_RST_0_SEL_FUN_SIM_RST_0_SEL (0 << 0)
#define IOMUX2_PAD_SIM_RST_0_OUT_REG (1 << 4)
#define IOMUX2_PAD_SIM_RST_0_OUT_FRC (1 << 5)
#define IOMUX2_PAD_SIM_RST_0_OEN_REG (1 << 6)
#define IOMUX2_PAD_SIM_RST_0_OEN_FRC (1 << 7)
#define IOMUX2_PAD_SIM_RST_0_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_SIM_RST_0_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_SIM_RST_0_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_SIM_RST_0_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_SIM_RST_0_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_SIM_RST_0_PULL_DN (1 << 10)
#define IOMUX2_PAD_SIM_RST_0_PULL_FRC (1 << 11)
#define IOMUX2_PAD_SIM_RST_0_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_SIM_RST_0_IE (1 << 17)
#define IOMUX2_PAD_SIM_RST_0_SE (1 << 18)

#define IOMUX2_PAD_SIM_RST_0_SEL_V_FUN_SIM_RST_0_SEL (0)
#define IOMUX2_PAD_SIM_RST_0_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_SIM_RST_0_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_SIM_RST_0_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_SIM_RST_0_PULL_UP_V_PULL_UP_1_8K (3)

// pad_sim_dio_0_cfg
#define IOMUX2_PAD_SIM_DIO_0_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_SIM_DIO_0_SEL_FUN_SIM_DIO_0_SEL (0 << 0)
#define IOMUX2_PAD_SIM_DIO_0_OUT_REG (1 << 4)
#define IOMUX2_PAD_SIM_DIO_0_OUT_FRC (1 << 5)
#define IOMUX2_PAD_SIM_DIO_0_OEN_REG (1 << 6)
#define IOMUX2_PAD_SIM_DIO_0_OEN_FRC (1 << 7)
#define IOMUX2_PAD_SIM_DIO_0_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_SIM_DIO_0_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_SIM_DIO_0_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_SIM_DIO_0_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_SIM_DIO_0_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_SIM_DIO_0_PULL_DN (1 << 10)
#define IOMUX2_PAD_SIM_DIO_0_PULL_FRC (1 << 11)
#define IOMUX2_PAD_SIM_DIO_0_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_SIM_DIO_0_IE (1 << 17)
#define IOMUX2_PAD_SIM_DIO_0_SE (1 << 18)

#define IOMUX2_PAD_SIM_DIO_0_SEL_V_FUN_SIM_DIO_0_SEL (0)
#define IOMUX2_PAD_SIM_DIO_0_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_SIM_DIO_0_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_SIM_DIO_0_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_SIM_DIO_0_PULL_UP_V_PULL_UP_1_8K (3)

// pad_sim_clk_1_cfg
#define IOMUX2_PAD_SIM_CLK_1_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_SIM_CLK_1_SEL_FUN_SIM_CLK_1_SEL (0 << 0)
#define IOMUX2_PAD_SIM_CLK_1_SEL_FUN_GPIO_36_SEL (1 << 0)
#define IOMUX2_PAD_SIM_CLK_1_SEL_FUN_TCO_0_SEL (2 << 0)
#define IOMUX2_PAD_SIM_CLK_1_SEL_FUN_GPT2_PWM_3_SEL (4 << 0)
#define IOMUX2_PAD_SIM_CLK_1_OUT_REG (1 << 4)
#define IOMUX2_PAD_SIM_CLK_1_OUT_FRC (1 << 5)
#define IOMUX2_PAD_SIM_CLK_1_OEN_REG (1 << 6)
#define IOMUX2_PAD_SIM_CLK_1_OEN_FRC (1 << 7)
#define IOMUX2_PAD_SIM_CLK_1_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_SIM_CLK_1_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_SIM_CLK_1_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_SIM_CLK_1_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_SIM_CLK_1_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_SIM_CLK_1_PULL_DN (1 << 10)
#define IOMUX2_PAD_SIM_CLK_1_PULL_FRC (1 << 11)
#define IOMUX2_PAD_SIM_CLK_1_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_SIM_CLK_1_IE (1 << 17)
#define IOMUX2_PAD_SIM_CLK_1_SE (1 << 18)

#define IOMUX2_PAD_SIM_CLK_1_SEL_V_FUN_SIM_CLK_1_SEL (0)
#define IOMUX2_PAD_SIM_CLK_1_SEL_V_FUN_GPIO_36_SEL (1)
#define IOMUX2_PAD_SIM_CLK_1_SEL_V_FUN_TCO_0_SEL (2)
#define IOMUX2_PAD_SIM_CLK_1_SEL_V_FUN_GPT2_PWM_3_SEL (4)
#define IOMUX2_PAD_SIM_CLK_1_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_SIM_CLK_1_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_SIM_CLK_1_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_SIM_CLK_1_PULL_UP_V_PULL_UP_1_8K (3)

// pad_sim_rst_1_cfg
#define IOMUX2_PAD_SIM_RST_1_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_SIM_RST_1_SEL_FUN_SIM_RST_1_SEL (0 << 0)
#define IOMUX2_PAD_SIM_RST_1_SEL_FUN_GPIO_37_SEL (1 << 0)
#define IOMUX2_PAD_SIM_RST_1_SEL_FUN_TCO_1_SEL (2 << 0)
#define IOMUX2_PAD_SIM_RST_1_SEL_FUN_GPT2_PWM_0_SEL (4 << 0)
#define IOMUX2_PAD_SIM_RST_1_OUT_REG (1 << 4)
#define IOMUX2_PAD_SIM_RST_1_OUT_FRC (1 << 5)
#define IOMUX2_PAD_SIM_RST_1_OEN_REG (1 << 6)
#define IOMUX2_PAD_SIM_RST_1_OEN_FRC (1 << 7)
#define IOMUX2_PAD_SIM_RST_1_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_SIM_RST_1_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_SIM_RST_1_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_SIM_RST_1_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_SIM_RST_1_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_SIM_RST_1_PULL_DN (1 << 10)
#define IOMUX2_PAD_SIM_RST_1_PULL_FRC (1 << 11)
#define IOMUX2_PAD_SIM_RST_1_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_SIM_RST_1_IE (1 << 17)
#define IOMUX2_PAD_SIM_RST_1_SE (1 << 18)

#define IOMUX2_PAD_SIM_RST_1_SEL_V_FUN_SIM_RST_1_SEL (0)
#define IOMUX2_PAD_SIM_RST_1_SEL_V_FUN_GPIO_37_SEL (1)
#define IOMUX2_PAD_SIM_RST_1_SEL_V_FUN_TCO_1_SEL (2)
#define IOMUX2_PAD_SIM_RST_1_SEL_V_FUN_GPT2_PWM_0_SEL (4)
#define IOMUX2_PAD_SIM_RST_1_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_SIM_RST_1_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_SIM_RST_1_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_SIM_RST_1_PULL_UP_V_PULL_UP_1_8K (3)

// pad_sim_dio_1_cfg
#define IOMUX2_PAD_SIM_DIO_1_SEL(n) (((n)&0xf) << 0)
#define IOMUX2_PAD_SIM_DIO_1_SEL_FUN_SIM_DIO_1_SEL (0 << 0)
#define IOMUX2_PAD_SIM_DIO_1_SEL_FUN_GPIO_38_SEL (1 << 0)
#define IOMUX2_PAD_SIM_DIO_1_SEL_FUN_GPT2_TI_0_SEL (4 << 0)
#define IOMUX2_PAD_SIM_DIO_1_OUT_REG (1 << 4)
#define IOMUX2_PAD_SIM_DIO_1_OUT_FRC (1 << 5)
#define IOMUX2_PAD_SIM_DIO_1_OEN_REG (1 << 6)
#define IOMUX2_PAD_SIM_DIO_1_OEN_FRC (1 << 7)
#define IOMUX2_PAD_SIM_DIO_1_PULL_UP(n) (((n)&0x3) << 8)
#define IOMUX2_PAD_SIM_DIO_1_PULL_UP_PULL_UP_OFF (0 << 8)
#define IOMUX2_PAD_SIM_DIO_1_PULL_UP_PULL_UP_4_7K (1 << 8)
#define IOMUX2_PAD_SIM_DIO_1_PULL_UP_PULL_UP_20K (2 << 8)
#define IOMUX2_PAD_SIM_DIO_1_PULL_UP_PULL_UP_1_8K (3 << 8)
#define IOMUX2_PAD_SIM_DIO_1_PULL_DN (1 << 10)
#define IOMUX2_PAD_SIM_DIO_1_PULL_FRC (1 << 11)
#define IOMUX2_PAD_SIM_DIO_1_DRV_STRENGTH(n) (((n)&0x3) << 14)
#define IOMUX2_PAD_SIM_DIO_1_IE (1 << 17)
#define IOMUX2_PAD_SIM_DIO_1_SE (1 << 18)

#define IOMUX2_PAD_SIM_DIO_1_SEL_V_FUN_SIM_DIO_1_SEL (0)
#define IOMUX2_PAD_SIM_DIO_1_SEL_V_FUN_GPIO_38_SEL (1)
#define IOMUX2_PAD_SIM_DIO_1_SEL_V_FUN_GPT2_TI_0_SEL (4)
#define IOMUX2_PAD_SIM_DIO_1_PULL_UP_V_PULL_UP_OFF (0)
#define IOMUX2_PAD_SIM_DIO_1_PULL_UP_V_PULL_UP_4_7K (1)
#define IOMUX2_PAD_SIM_DIO_1_PULL_UP_V_PULL_UP_20K (2)
#define IOMUX2_PAD_SIM_DIO_1_PULL_UP_V_PULL_UP_1_8K (3)

#endif // _IOMUX2_H_
