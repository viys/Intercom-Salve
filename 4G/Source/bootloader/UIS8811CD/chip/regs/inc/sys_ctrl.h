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

#ifndef _SYS_CTRL_H_
#define _SYS_CTRL_H_

// Auto generated (v1.0-38-g167fa99). Don't edit it manually!

#define REG_SYS_CTRL_BASE (0x41a19000)

typedef volatile struct
{
    uint32_t chip_id;              // 0x00000000
    uint32_t reg_dbg;              // 0x00000004
    uint32_t sys_rst_set0;         // 0x00000008
    uint32_t sys_rst_clr0;         // 0x0000000c
    uint32_t sys_rst_set1;         // 0x00000010
    uint32_t sys_rst_clr1;         // 0x00000014
    uint32_t clk_sys_enable0;      // 0x00000018
    uint32_t clk_sys_disable0;     // 0x0000001c
    uint32_t clk_sys_enable1;      // 0x00000020
    uint32_t clk_sys_disable1;     // 0x00000024
    uint32_t pll_ctrl;             // 0x00000028
    uint32_t sel_clock_sys;        // 0x0000002c
    uint32_t cfg_clk_sys;          // 0x00000030
    uint32_t cfg_clk_spiflash;     // 0x00000034
    uint32_t cfg_clk_spiflash_ext; // 0x00000038
    uint32_t cfg_clk_dbg;          // 0x0000003c
    uint32_t cfg_clk_uart3;        // 0x00000040
    uint32_t cfg_clk_uart4;        // 0x00000044
    uint32_t cfg_clk_uart5;        // 0x00000048
    uint32_t cfg_clk_psram;        // 0x0000004c
    uint32_t cfg_clk_dblr;         // 0x00000050
    uint32_t cfg_clk_i2s;          // 0x00000054
    uint32_t cfg_clk_out;          // 0x00000058
    uint32_t reset_cause;          // 0x0000005c
    uint32_t sys_wakeup_cause;     // 0x00000060
    uint32_t misc_ctrl;            // 0x00000064
    uint32_t pad_ctrl;             // 0x00000068
    uint32_t cpu_ctrl;             // 0x0000006c
    uint32_t otp_reg;              // 0x00000070
    uint32_t mpll_setting0;        // 0x00000074
    uint32_t mpll_setting1;        // 0x00000078
    uint32_t mpll_sdm_freq;        // 0x0000007c
    uint32_t dbg_disable_acg0;     // 0x00000080
    uint32_t dbg_disable_acg1;     // 0x00000084
    uint32_t dbg_disable_acg2;     // 0x00000088
    uint32_t dbg_disable_acg3;     // 0x0000008c
    uint32_t cfg_clk_nbiot_fast;   // 0x00000090
    uint32_t calib_th_32k;         // 0x00000094
    uint32_t calib_th_rc26m;       // 0x00000098
    uint32_t ctrl_of_32k_calib;    // 0x0000009c
    uint32_t ctrl_of_rc26m_calib;  // 0x000000a0
    uint32_t num_of_clk1_32k;      // 0x000000a4
    uint32_t num_of_clk1_rc26m;    // 0x000000a8
    uint32_t star_cfg_nsst;        // 0x000000ac
    uint32_t star_cfg_sst;         // 0x000000b0
    uint32_t mem_cfg1;             // 0x000000b4
    uint32_t timer2_divider;       // 0x000000b8
    uint32_t st_divider;           // 0x000000bc
    uint32_t sst_divider;          // 0x000000c0
    uint32_t sys_ctrl_rsd0;        // 0x000000c4
    uint32_t sys_ctrl_rsd1;        // 0x000000c8
    uint32_t sys_ctrl_rsd2;        // 0x000000cc
    uint32_t sys_ctrl_rsd3;        // 0x000000d0
    uint32_t mem_cfg2;             // 0x000000d4
    uint32_t clk_mnt26m_th0;       // 0x000000d8
    uint32_t clk_mnt26m_th1;       // 0x000000dc
    uint32_t clk_mnt26m_th2;       // 0x000000e0
    uint32_t clk_mnt26m_th3;       // 0x000000e4
    uint32_t clk_mnt32k_th0;       // 0x000000e8
    uint32_t clk_mnt32k_th1;       // 0x000000ec
    uint32_t clk_mnt_ctrl;         // 0x000000f0
    uint32_t clk_mnt26m_st;        // 0x000000f4
    uint32_t clk_mnt32k_st;        // 0x000000f8
    uint32_t dbg_ctrl;             // 0x000000fc
    uint32_t lp_irq;               // 0x00000100
    uint32_t adi_if_evt;           // 0x00000104
    uint32_t cfg_clk_efuse;        // 0x00000108
    uint32_t rfspi_apb_sel;        // 0x0000010c
    uint32_t cfg_clk_sram;         // 0x00000110
    uint32_t sram_h2h_config;      // 0x00000114
} HWP_SYS_CTRL_T;

#define hwp_sysCtrl ((HWP_SYS_CTRL_T *)REG_ACCESS_ADDRESS(REG_SYS_CTRL_BASE))

// chip_id
typedef union {
    uint32_t v;
    struct
    {
        uint32_t metal_id : 12; // [11:0], read only
        uint32_t bond_id : 1;   // [12], read only
        uint32_t minor_id : 5;  // [17:13], read only
        uint32_t major_id : 14; // [31:18], read only
    } b;
} REG_SYS_CTRL_CHIP_ID_T;

// reg_dbg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t scratch : 16; // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_SYS_CTRL_REG_DBG_T;

// sys_rst_set0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_rst_starmcu : 1;  // [0], write set
        uint32_t set_rst_sys_dma : 1;  // [1], write set
        uint32_t set_rst_sys_dec1 : 1; // [2], write set
        uint32_t set_rst_sys_dec2 : 1; // [3], write set
        uint32_t set_rst_timer2 : 1;   // [4], write set
        uint32_t set_rst_wdt : 1;      // [5], write set
        uint32_t set_rst_uart3 : 1;    // [6], write set
        uint32_t set_rst_uart4 : 1;    // [7], write set
        uint32_t set_rst_uart5 : 1;    // [8], write set
        uint32_t set_rst_spi1 : 1;     // [9], write set
        uint32_t set_rst_spi2 : 1;     // [10], write set
        uint32_t set_rst_gpio2 : 1;    // [11], write set
        uint32_t set_rst_gpt2 : 1;     // [12], write set
        uint32_t set_rst_i2c1 : 1;     // [13], write set
        uint32_t set_rst_i2c2 : 1;     // [14], write set
        uint32_t set_rst_i2c3 : 1;     // [15], write set
        uint32_t set_rst_sdmmc : 1;    // [16], write set
        uint32_t set_rst_seg_lcd : 1;  // [17], write set
        uint32_t set_rst_keypad : 1;   // [18], write set
        uint32_t set_rst_i2s : 1;      // [19], write set
        uint32_t set_rst_page_spy : 1; // [20], write set
        uint32_t __30_21 : 10;         // [30:21]
        uint32_t set_rst_global : 1;   // [31], write set
    } b;
} REG_SYS_CTRL_SYS_RST_SET0_T;

// sys_rst_clr0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_rst_starmcu : 1;  // [0], write clear
        uint32_t clr_rst_sys_dma : 1;  // [1], write clear
        uint32_t clr_rst_sys_dec1 : 1; // [2], write clear
        uint32_t clr_rst_sys_dec2 : 1; // [3], write clear
        uint32_t clr_rst_timer2 : 1;   // [4], write clear
        uint32_t clr_rst_wdt : 1;      // [5], write clear
        uint32_t clr_rst_uart3 : 1;    // [6], write clear
        uint32_t clr_rst_uart4 : 1;    // [7], write clear
        uint32_t clr_rst_uart5 : 1;    // [8], write clear
        uint32_t clr_rst_spi1 : 1;     // [9], write clear
        uint32_t clr_rst_spi2 : 1;     // [10], write clear
        uint32_t clr_rst_gpio2 : 1;    // [11], write clear
        uint32_t clr_rst_gpt2 : 1;     // [12], write clear
        uint32_t clr_rst_i2c1 : 1;     // [13], write clear
        uint32_t clr_rst_i2c2 : 1;     // [14], write clear
        uint32_t clr_rst_i2c3 : 1;     // [15], write clear
        uint32_t clr_rst_sdmmc : 1;    // [16], write clear
        uint32_t clr_rst_seg_lcd : 1;  // [17], write clear
        uint32_t clr_rst_keypad : 1;   // [18], write clear
        uint32_t clr_rst_i2s : 1;      // [19], write clear
        uint32_t clr_rst_page_spy : 1; // [20], write clear
        uint32_t __31_21 : 11;         // [31:21]
    } b;
} REG_SYS_CTRL_SYS_RST_CLR0_T;

// sys_rst_set1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t set_rst_efuse : 1;     // [0], write set
        uint32_t set_rst_iomux2 : 1;    // [1], write set
        uint32_t set_rst_flash : 1;     // [2], write set
        uint32_t set_rst_flash_ext : 1; // [3], write set
        uint32_t set_rst_adi_if : 1;    // [4], write set
        uint32_t set_rst_sci2 : 1;      // [5], write set
        uint32_t set_rst_dbg : 1;       // [6], write set
        uint32_t set_rst_psram : 1;     // [7], write set
        uint32_t set_rst_ce_top : 1;    // [8], write set
        uint32_t set_rst_med : 1;       // [9], write set
        uint32_t __31_10 : 22;          // [31:10]
    } b;
} REG_SYS_CTRL_SYS_RST_SET1_T;

// sys_rst_clr1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_rst_efuse : 1;     // [0], write clear
        uint32_t clr_rst_iomux2 : 1;    // [1], write clear
        uint32_t clr_rst_flash : 1;     // [2], write clear
        uint32_t clr_rst_flash_ext : 1; // [3], write clear
        uint32_t clr_rst_adi_if : 1;    // [4], write clear
        uint32_t clr_rst_sci2 : 1;      // [5], write clear
        uint32_t clr_rst_dbg : 1;       // [6], write clear
        uint32_t clr_rst_psram : 1;     // [7], write clear
        uint32_t clr_rst_ce_top : 1;    // [8], write clear
        uint32_t clr_rst_med : 1;       // [9], write clear
        uint32_t __31_10 : 22;          // [31:10]
    } b;
} REG_SYS_CTRL_SYS_RST_CLR1_T;

// clk_sys_enable0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_starmcu : 1;  // [0], write set
        uint32_t enable_sys_dma : 1;  // [1], write set
        uint32_t enable_sys_dec1 : 1; // [2], write set
        uint32_t enable_sys_dec2 : 1; // [3], write set
        uint32_t enable_timer2 : 1;   // [4], write set
        uint32_t enable_wdt : 1;      // [5], write set
        uint32_t enable_uart3 : 1;    // [6], write set
        uint32_t enable_uart4 : 1;    // [7], write set
        uint32_t enable_uart5 : 1;    // [8], write set
        uint32_t enable_spi1 : 1;     // [9], write set
        uint32_t enable_spi2 : 1;     // [10], write set
        uint32_t enable_gpio2 : 1;    // [11], write set
        uint32_t enable_gpt2 : 1;     // [12], write set
        uint32_t enable_i2c1 : 1;     // [13], write set
        uint32_t enable_i2c2 : 1;     // [14], write set
        uint32_t enable_i2c3 : 1;     // [15], write set
        uint32_t enable_sdmmc : 1;    // [16], write set
        uint32_t enable_seg_lcd : 1;  // [17], write set
        uint32_t enable_keypad : 1;   // [18], write set
        uint32_t enable_i2s : 1;      // [19], write set
        uint32_t enable_page_spy : 1; // [20], write set
        uint32_t __31_21 : 11;        // [31:21]
    } b;
} REG_SYS_CTRL_CLK_SYS_ENABLE0_T;

// clk_sys_disable0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t disable_starmcu : 1;  // [0], write clear
        uint32_t disable_sys_dma : 1;  // [1], write clear
        uint32_t disable_sys_dec1 : 1; // [2], write clear
        uint32_t disable_sys_dec2 : 1; // [3], write clear
        uint32_t disable_timer2 : 1;   // [4], write clear
        uint32_t disable_wdt : 1;      // [5], write clear
        uint32_t disable_uart3 : 1;    // [6], write clear
        uint32_t disable_uart4 : 1;    // [7], write clear
        uint32_t disable_uart5 : 1;    // [8], write clear
        uint32_t disable_spi1 : 1;     // [9], write clear
        uint32_t disable_spi2 : 1;     // [10], write clear
        uint32_t disable_gpio2 : 1;    // [11], write clear
        uint32_t disable_gpt2 : 1;     // [12], write clear
        uint32_t disable_i2c1 : 1;     // [13], write clear
        uint32_t disable_i2c2 : 1;     // [14], write clear
        uint32_t disable_i2c3 : 1;     // [15], write clear
        uint32_t disable_sdmmc : 1;    // [16], write clear
        uint32_t disable_seg_lcd : 1;  // [17], write clear
        uint32_t disable_keypad : 1;   // [18], write clear
        uint32_t disable_i2s : 1;      // [19], write clear
        uint32_t disable_page_spy : 1; // [20], write clear
        uint32_t __31_21 : 11;         // [31:21]
    } b;
} REG_SYS_CTRL_CLK_SYS_DISABLE0_T;

// clk_sys_enable1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable_efuse : 1;     // [0], write set
        uint32_t enable_iomux2 : 1;    // [1], write set
        uint32_t enable_flash : 1;     // [2], write set
        uint32_t enable_flash_ext : 1; // [3], write set
        uint32_t enable_adi_if : 1;    // [4], write set
        uint32_t enable_sci2 : 1;      // [5], write set
        uint32_t enable_dbg : 1;       // [6], write set
        uint32_t enable_psram : 1;     // [7], write set
        uint32_t enable_ce_top : 1;    // [8], write set
        uint32_t enable_med : 1;       // [9], write set
        uint32_t __31_10 : 22;         // [31:10]
    } b;
} REG_SYS_CTRL_CLK_SYS_ENABLE1_T;

// clk_sys_disable1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t disable_efuse : 1;     // [0], write clear
        uint32_t disable_iomux2 : 1;    // [1], write clear
        uint32_t disable_flash : 1;     // [2], write clear
        uint32_t disable_flash_ext : 1; // [3], write clear
        uint32_t disable_adi_if : 1;    // [4], write clear
        uint32_t disable_sci2 : 1;      // [5], write clear
        uint32_t disable_dbg : 1;       // [6], write clear
        uint32_t disable_psram : 1;     // [7], write clear
        uint32_t disable_ce_top : 1;    // [8], write clear
        uint32_t disable_med : 1;       // [9], write clear
        uint32_t __31_10 : 22;          // [31:10]
    } b;
} REG_SYS_CTRL_CLK_SYS_DISABLE1_T;

// pll_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mcu_pll_pu : 1;     // [0], write set
        uint32_t mcu_pll_pd : 1;     // [1], write clear
        uint32_t mcu_pll_locked : 1; // [2], read only
        uint32_t __3_3 : 1;          // [3]
        uint32_t nb_pll_pu : 1;      // [4], write set
        uint32_t nb_pll_pd : 1;      // [5], write clear
        uint32_t nb_pll_locked : 1;  // [6], read only
        uint32_t __31_7 : 25;        // [31:7]
    } b;
} REG_SYS_CTRL_PLL_CTRL_T;

// sel_clock_sys
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sel_clk_slow : 2;        // [1:0]
        uint32_t sel_clk_sys : 1;         // [2]
        uint32_t sel_clk_flash : 1;       // [3]
        uint32_t sel_clk_flash_ext : 1;   // [4]
        uint32_t sel_clk_uart3 : 2;       // [6:5]
        uint32_t sel_clk_uart4 : 2;       // [8:7]
        uint32_t sel_clk_uart5 : 2;       // [10:9]
        uint32_t sel_clk_dbg : 2;         // [12:11]
        uint32_t sel_clk_spi1 : 1;        // [13]
        uint32_t sel_clk_spi2 : 1;        // [14]
        uint32_t sel_clk_rfspi : 1;       // [15]
        uint32_t sel_clk_psram : 1;       // [16]
        uint32_t sel_clk_i2s : 1;         // [17]
        uint32_t __20_18 : 3;             // [20:18]
        uint32_t sel_clk_xtal52m_bak : 1; // [21]
        uint32_t sel_clk_efuse : 2;       // [23:22]
        uint32_t sel_sst_clk : 2;         // [25:24]
        uint32_t sel_st_clk : 2;          // [27:26]
        uint32_t sel_timer2_clk : 2;      // [29:28]
        uint32_t __31_30 : 2;             // [31:30]
    } b;
} REG_SYS_CTRL_SEL_CLOCK_SYS_T;

// cfg_clk_sys
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sys_div_num : 9;      // [8:0]
        uint32_t sys_div_denom : 9;    // [17:9]
        uint32_t sys_pclken_num : 6;   // [23:18]
        uint32_t sys_pclken_denom : 6; // [29:24]
        uint32_t __31_30 : 2;          // [31:30]
    } b;
} REG_SYS_CTRL_CFG_CLK_SYS_T;

// cfg_clk_spiflash
typedef union {
    uint32_t v;
    struct
    {
        uint32_t spiflash_freq : 4; // [3:0]
        uint32_t __31_4 : 28;       // [31:4]
    } b;
} REG_SYS_CTRL_CFG_CLK_SPIFLASH_T;

// cfg_clk_spiflash_ext
typedef union {
    uint32_t v;
    struct
    {
        uint32_t spiflash_ext_freq : 4; // [3:0]
        uint32_t __31_4 : 28;           // [31:4]
    } b;
} REG_SYS_CTRL_CFG_CLK_SPIFLASH_EXT_T;

// cfg_clk_dbg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t dbg_div_num : 10;   // [9:0]
        uint32_t dbg_div_denom : 14; // [23:10]
        uint32_t __31_24 : 8;        // [31:24]
    } b;
} REG_SYS_CTRL_CFG_CLK_DBG_T;

// cfg_clk_uart3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t uart3_div_num : 10;   // [9:0]
        uint32_t uart3_div_denom : 14; // [23:10]
        uint32_t __31_24 : 8;          // [31:24]
    } b;
} REG_SYS_CTRL_CFG_CLK_UART3_T;

// cfg_clk_uart4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t uart4_div_num : 10;   // [9:0]
        uint32_t uart4_div_denom : 14; // [23:10]
        uint32_t __31_24 : 8;          // [31:24]
    } b;
} REG_SYS_CTRL_CFG_CLK_UART4_T;

// cfg_clk_uart5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t uart5_div_num : 10;   // [9:0]
        uint32_t uart5_div_denom : 14; // [23:10]
        uint32_t __31_24 : 8;          // [31:24]
    } b;
} REG_SYS_CTRL_CFG_CLK_UART5_T;

// cfg_clk_psram
typedef union {
    uint32_t v;
    struct
    {
        uint32_t psram_freq : 4; // [3:0]
        uint32_t __31_4 : 28;    // [31:4]
    } b;
} REG_SYS_CTRL_CFG_CLK_PSRAM_T;

// cfg_clk_dblr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sys_dblr_div_denom : 6;  // [5:0]
        uint32_t sys_dblr_div_denom2 : 6; // [11:6]
        uint32_t mpll_clk_en : 1;         // [12]
        uint32_t __17_13 : 5;             // [17:13]
        uint32_t clkdblr_ibias_ctrl : 2;  // [19:18]
        uint32_t clkdblr_vbias_ctrl : 2;  // [21:20]
        uint32_t clkdblr_r_ctrl : 3;      // [24:22]
        uint32_t clkdblr_clk_en : 1;      // [25]
        uint32_t pu_clkdblr : 1;          // [26]
        uint32_t dblr_rsd : 4;            // [30:27]
        uint32_t __31_31 : 1;             // [31]
    } b;
} REG_SYS_CTRL_CFG_CLK_DBLR_T;

// cfg_clk_i2s
typedef union {
    uint32_t v;
    struct
    {
        uint32_t i2s_div_denom : 16;      // [15:0]
        uint32_t i2s_mclk_div_denom : 16; // [31:16]
    } b;
} REG_SYS_CTRL_CFG_CLK_I2S_T;

// cfg_clk_out
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clkout0_sel : 3; // [2:0]
        uint32_t clkout1_sel : 3; // [5:3]
        uint32_t clkout2_sel : 3; // [8:6]
        uint32_t clkout3_sel : 3; // [11:9]
        uint32_t __27_12 : 16;    // [27:12]
        uint32_t clkout0_en : 1;  // [28]
        uint32_t clkout1_en : 1;  // [29]
        uint32_t clkout2_en : 1;  // [30]
        uint32_t clkout3_en : 1;  // [31]
    } b;
} REG_SYS_CTRL_CFG_CLK_OUT_T;

// reset_cause
typedef union {
    uint32_t v;
    struct
    {
        uint32_t globalsoft_reset : 1;      // [0], write clear
        uint32_t debughost_reset : 1;       // [1], write clear
        uint32_t mcu_sysresetreq_reset : 1; // [2], write clear
        uint32_t mcu_lockup_reset : 1;      // [3], write clear
        uint32_t wdt_reset : 1;             // [4], write clear
        uint32_t clk_monitor_32k_reset : 1; // [5], write clear
        uint32_t clk_monitor_26m_reset : 1; // [6], write clear
        uint32_t __15_7 : 9;                // [15:7]
        uint32_t boot_mode : 4;             // [19:16]
        uint32_t sw_boot_mode : 4;          // [23:20]
        uint32_t __31_24 : 8;               // [31:24]
    } b;
} REG_SYS_CTRL_RESET_CAUSE_T;

// sys_wakeup_cause
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mcu_lps_wakeup : 1;     // [0], read only
        uint32_t mcu_gpt1_wakeup : 1;    // [1], read only
        uint32_t mcu_uart1_wakeup : 1;   // [2], read only
        uint32_t mcu_uart2_wakeup : 1;   // [3], read only
        uint32_t mcu_timer1_wakeup : 1;  // [4], read only
        uint32_t mcu_gpio1_wakeup : 1;   // [5], read only
        uint32_t mcu_dbg_wakeup : 1;     // [6], read only
        uint32_t mcu_ana_wakeup : 1;     // [7], read only
        uint32_t mcu_ext_rst_wakeup : 1; // [8], read only
        uint32_t __31_9 : 23;            // [31:9]
    } b;
} REG_SYS_CTRL_SYS_WAKEUP_CAUSE_T;

// misc_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t lockup_rst_en : 1;           // [0]
        uint32_t wdt_rstn_en : 1;             // [1]
        uint32_t clk_monitor_32k_rstn_en : 1; // [2]
        uint32_t clk_monitor_26m_rstn_en : 1; // [3]
        uint32_t host_reset_l_en : 1;         // [4]
        uint32_t force_xcpu_reset_l_en : 1;   // [5]
        uint32_t psram_eco_en : 1;            // [6]
        uint32_t mem_retention : 1;           // [7]
        uint32_t dbg_out_sel : 4;             // [11:8]
        uint32_t dbg_trig_sel : 4;            // [15:12]
        uint32_t dbg_clk_sel : 4;             // [19:16]
        uint32_t dbg_clk_en : 1;              // [20]
        uint32_t hresp_err_mask_sysifc1 : 1;  // [21]
        uint32_t hresp_err_mask_sysifc2 : 1;  // [22]
        uint32_t adi_sel_pad : 1;             // [23]
        uint32_t tst_h_wpd : 1;               // [24]
        uint32_t boot_mode_wpd : 2;           // [26:25]
        uint32_t locksau : 1;                 // [27]
        uint32_t locknsmpu : 1;               // [28]
        uint32_t locksmpu : 1;                // [29]
        uint32_t locknsvtor : 1;              // [30]
        uint32_t locksvtaircr : 1;            // [31]
    } b;
} REG_SYS_CTRL_MISC_CTRL_T;

// pad_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ibit_flash : 3;    // [2:0]
        uint32_t ibit_psram : 3;    // [5:3]
        uint32_t ibit_a_die_if : 2; // [7:6]
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_SYS_CTRL_PAD_CTRL_T;

// cpu_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t starmcu_sleeping : 1;  // [0], read only
        uint32_t starmcu_sleepdeep : 1; // [1], read only
        uint32_t starmcu_wicenreq : 1;  // [2]
        uint32_t starmcu_wicenack : 1;  // [3], read only
        uint32_t soft_cpu_sleep : 1;    // [4], write clear
        uint32_t __31_5 : 27;           // [31:5]
    } b;
} REG_SYS_CTRL_CPU_CTRL_T;

// otp_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t force_enable_swd : 1;     // [0]
        uint32_t force_enable_idbg : 1;    // [1]
        uint32_t force_enable_nidbg : 1;   // [2]
        uint32_t force_enable_sidbg : 1;   // [3]
        uint32_t force_enable_snidbg : 1;  // [4]
        uint32_t force_enable_dbghost : 1; // [5]
        uint32_t otp_reg_rsvd : 6;         // [11:6]
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_SYS_CTRL_OTP_REG_T;

// mpll_setting0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mpll_sdm_clk_sel_dr : 1;  // [0]
        uint32_t mpll_sdm_clk_sel_reg : 1; // [1]
        uint32_t mpll_refmulti2_en : 1;    // [2]
        uint32_t mpll_pcon_mode : 1;       // [3]
        uint32_t mpll_test_en : 1;         // [4]
        uint32_t mpll_cp_offset_en : 1;    // [5]
        uint32_t mpll_cpbias_ibit : 3;     // [8:6]
        uint32_t mpll_cp_offset : 2;       // [10:9]
        uint32_t mpll_sdmclk_disable : 1;  // [11]
        uint32_t __13_12 : 2;              // [13:12]
        uint32_t mpll_en_reg : 1;          // [14]
        uint32_t mpll_en_dr : 1;           // [15]
        uint32_t mpll_sel_reg : 1;         // [16]
        uint32_t mpll_sel_dr : 1;          // [17]
        uint32_t mpll_rstb_reg : 1;        // [18]
        uint32_t mpll_rstb_dr : 1;         // [19]
        uint32_t mpll_ready_timer : 2;     // [21:20]
        uint32_t mpll_band_sel : 2;        // [23:22]
        uint32_t mpll_lock : 1;            // [24], read only
        uint32_t mpll_clktest_out : 1;     // [25], read only
        uint32_t __31_26 : 6;              // [31:26]
    } b;
} REG_SYS_CTRL_MPLL_SETTING0_T;

// mpll_setting1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t mpll_sdm_dither_bypass : 1; // [0]
        uint32_t mpll_sdm_int_dec_sel : 3;   // [3:1]
        uint32_t mpll_sdm_resetn_reg : 1;    // [4]
        uint32_t mpll_sdm_resetn_dr : 1;     // [5]
        uint32_t mpll_sdm_clk_inv : 1;       // [6]
        uint32_t __15_7 : 9;                 // [15:7]
        uint32_t mpll_sdm_clk_test_en : 1;   // [16]
        uint32_t mpll_vco_high_test : 1;     // [17]
        uint32_t mpll_vco_low_test : 1;      // [18]
        uint32_t mpll_int_mode : 1;          // [19]
        uint32_t mpll_testsig_sel : 2;       // [21:20]
        uint32_t mpll_rsvd_reg : 8;          // [29:22]
        uint32_t __31_30 : 2;                // [31:30]
    } b;
} REG_SYS_CTRL_MPLL_SETTING1_T;

// cfg_clk_nbiot_fast
typedef union {
    uint32_t v;
    struct
    {
        uint32_t nbiot_fast_div_num : 8;   // [7:0]
        uint32_t nbiot_fast_div_denom : 8; // [15:8]
        uint32_t __31_16 : 16;             // [31:16]
    } b;
} REG_SYS_CTRL_CFG_CLK_NBIOT_FAST_T;

// ctrl_of_32k_calib
typedef union {
    uint32_t v;
    struct
    {
        uint32_t calib_en_32k : 1;      // [0], write set
        uint32_t calib_int_en_32k : 1;  // [1]
        uint32_t calib_int_clr_32k : 1; // [2], write set
        uint32_t __31_3 : 29;           // [31:3]
    } b;
} REG_SYS_CTRL_CTRL_OF_32K_CALIB_T;

// ctrl_of_rc26m_calib
typedef union {
    uint32_t v;
    struct
    {
        uint32_t calib_en_rc26m : 1;      // [0], write set
        uint32_t calib_int_en_rc26m : 1;  // [1]
        uint32_t calib_int_clr_rc26m : 1; // [2], write set
        uint32_t __31_3 : 29;             // [31:3]
    } b;
} REG_SYS_CTRL_CTRL_OF_RC26M_CALIB_T;

// star_cfg_nsst
typedef union {
    uint32_t v;
    struct
    {
        uint32_t star_nsstcalib : 26; // [25:0]
        uint32_t __31_26 : 6;         // [31:26]
    } b;
} REG_SYS_CTRL_STAR_CFG_NSST_T;

// star_cfg_sst
typedef union {
    uint32_t v;
    struct
    {
        uint32_t star_sstcalib : 26; // [25:0]
        uint32_t __31_26 : 6;        // [31:26]
    } b;
} REG_SYS_CTRL_STAR_CFG_SST_T;

// timer2_divider
typedef union {
    uint32_t v;
    struct
    {
        uint32_t timer2_div_denom : 8; // [7:0]
        uint32_t timer2_div_num : 8;   // [15:8]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_SYS_CTRL_TIMER2_DIVIDER_T;

// st_divider
typedef union {
    uint32_t v;
    struct
    {
        uint32_t st_div_denom : 8; // [7:0]
        uint32_t st_div_num : 8;   // [15:8]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_SYS_CTRL_ST_DIVIDER_T;

// sst_divider
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sst_div_denom : 8; // [7:0]
        uint32_t sst_div_num : 8;   // [15:8]
        uint32_t __31_16 : 16;      // [31:16]
    } b;
} REG_SYS_CTRL_SST_DIVIDER_T;

// clk_mnt26m_th0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_mnt26m_th0 : 8; // [7:0]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_SYS_CTRL_CLK_MNT26M_TH0_T;

// clk_mnt26m_th1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_mnt26m_th1 : 16; // [15:0]
        uint32_t __31_16 : 16;        // [31:16]
    } b;
} REG_SYS_CTRL_CLK_MNT26M_TH1_T;

// clk_mnt26m_th2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_mnt26m_th2 : 7; // [6:0]
        uint32_t __31_7 : 25;        // [31:7]
    } b;
} REG_SYS_CTRL_CLK_MNT26M_TH2_T;

// clk_mnt26m_th3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_mnt26m_th3 : 9; // [8:0]
        uint32_t __31_9 : 23;        // [31:9]
    } b;
} REG_SYS_CTRL_CLK_MNT26M_TH3_T;

// clk_mnt32k_th0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_mnt32k_th0 : 12; // [11:0]
        uint32_t __31_12 : 20;        // [31:12]
    } b;
} REG_SYS_CTRL_CLK_MNT32K_TH0_T;

// clk_mnt32k_th1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_mnt32k_th1 : 12; // [11:0]
        uint32_t __31_12 : 20;        // [31:12]
    } b;
} REG_SYS_CTRL_CLK_MNT32K_TH1_T;

// clk_mnt_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clk_mnt32k_en : 1;     // [0]
        uint32_t clk_mnt26m_en : 1;     // [1]
        uint32_t en_int_clk_mnt32k : 1; // [2]
        uint32_t en_int_clk_mnt26m : 1; // [3]
        uint32_t st_clk_mnt32k : 1;     // [4], read only
        uint32_t st_clk_mnt26m : 1;     // [5], read only
        uint32_t __31_6 : 26;           // [31:6]
    } b;
} REG_SYS_CTRL_CLK_MNT_CTRL_T;

// clk_mnt26m_st
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_int_clk_mnt26m : 1; // [0], write set
        uint32_t __31_1 : 31;            // [31:1]
    } b;
} REG_SYS_CTRL_CLK_MNT26M_ST_T;

// clk_mnt32k_st
typedef union {
    uint32_t v;
    struct
    {
        uint32_t clr_int_clk_mnt32k : 1; // [0], write set
        uint32_t __31_1 : 31;            // [31:1]
    } b;
} REG_SYS_CTRL_CLK_MNT32K_ST_T;

// dbg_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t cfg_clkout_en : 1;     // [0]
        uint32_t cfg_clkout_sel : 4;    // [4:1]
        uint32_t __7_5 : 3;             // [7:5]
        uint32_t med_debug_bus_sel : 5; // [12:8]
        uint32_t __31_13 : 19;          // [31:13]
    } b;
} REG_SYS_CTRL_DBG_CTRL_T;

// lp_irq
typedef union {
    uint32_t v;
    struct
    {
        uint32_t timer1_os_irq : 1; // [0], read only
        uint32_t timer1_irq : 1;    // [1], read only
        uint32_t lps_irq : 1;       // [2], read only
        uint32_t pwr_ctrl_irq : 1;  // [3], read only
        uint32_t gpt1_irq : 1;      // [4], read only
        uint32_t gpio1_irq : 1;     // [5], read only
        uint32_t uart2_irq : 1;     // [6], read only
        uint32_t uart1_irq : 1;     // [7], read only
        uint32_t __31_8 : 24;       // [31:8]
    } b;
} REG_SYS_CTRL_LP_IRQ_T;

// adi_if_evt
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adi_if_evt0 : 1; // [0]
        uint32_t adi_if_evt1 : 1; // [1]
        uint32_t __31_2 : 30;     // [31:2]
    } b;
} REG_SYS_CTRL_ADI_IF_EVT_T;

// cfg_clk_efuse
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efuse_div_denom : 3; // [2:0]
        uint32_t __31_3 : 29;         // [31:3]
    } b;
} REG_SYS_CTRL_CFG_CLK_EFUSE_T;

// rfspi_apb_sel
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rfspi_apb_sel_sw : 1; // [0]
        uint32_t rfspi_apb_sel_dr : 1; // [1]
        uint32_t __31_2 : 30;          // [31:2]
    } b;
} REG_SYS_CTRL_RFSPI_APB_SEL_T;

// cfg_clk_sram
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sram_div_denom : 4; // [3:0]
        uint32_t __31_4 : 28;        // [31:4]
    } b;
} REG_SYS_CTRL_CFG_CLK_SRAM_T;

// sram_h2h_config
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sram_h2h_incr_pre_read : 1;       // [0]
        uint32_t sram_h2h_nobuf_early_resp_en : 1; // [1]
        uint32_t sram_h2h_acg_en : 1;              // [2]
        uint32_t __31_3 : 29;                      // [31:3]
    } b;
} REG_SYS_CTRL_SRAM_H2H_CONFIG_T;

// chip_id
#define SYS_CTRL_METAL_ID(n) (((n)&0xfff) << 0)
#define SYS_CTRL_BOND_ID (1 << 12)
#define SYS_CTRL_MINOR_ID(n) (((n)&0x1f) << 13)
#define SYS_CTRL_MAJOR_ID(n) (((n)&0x3fff) << 18)

// reg_dbg
#define SYS_CTRL_SCRATCH(n) (((n)&0xffff) << 0)

// sys_rst_set0
#define SYS_CTRL_SET_RST_STARMCU (1 << 0)
#define SYS_CTRL_SET_RST_SYS_DMA (1 << 1)
#define SYS_CTRL_SET_RST_SYS_DEC1 (1 << 2)
#define SYS_CTRL_SET_RST_SYS_DEC2 (1 << 3)
#define SYS_CTRL_SET_RST_TIMER2 (1 << 4)
#define SYS_CTRL_SET_RST_WDT (1 << 5)
#define SYS_CTRL_SET_RST_UART3 (1 << 6)
#define SYS_CTRL_SET_RST_UART4 (1 << 7)
#define SYS_CTRL_SET_RST_UART5 (1 << 8)
#define SYS_CTRL_SET_RST_SPI1 (1 << 9)
#define SYS_CTRL_SET_RST_SPI2 (1 << 10)
#define SYS_CTRL_SET_RST_GPIO2 (1 << 11)
#define SYS_CTRL_SET_RST_GPT2 (1 << 12)
#define SYS_CTRL_SET_RST_I2C1 (1 << 13)
#define SYS_CTRL_SET_RST_I2C2 (1 << 14)
#define SYS_CTRL_SET_RST_I2C3 (1 << 15)
#define SYS_CTRL_SET_RST_SDMMC (1 << 16)
#define SYS_CTRL_SET_RST_SEG_LCD (1 << 17)
#define SYS_CTRL_SET_RST_KEYPAD (1 << 18)
#define SYS_CTRL_SET_RST_I2S (1 << 19)
#define SYS_CTRL_SET_RST_PAGE_SPY (1 << 20)
#define SYS_CTRL_SET_RST_GLOBAL (1 << 31)

// sys_rst_clr0
#define SYS_CTRL_CLR_RST_STARMCU (1 << 0)
#define SYS_CTRL_CLR_RST_SYS_DMA (1 << 1)
#define SYS_CTRL_CLR_RST_SYS_DEC1 (1 << 2)
#define SYS_CTRL_CLR_RST_SYS_DEC2 (1 << 3)
#define SYS_CTRL_CLR_RST_TIMER2 (1 << 4)
#define SYS_CTRL_CLR_RST_WDT (1 << 5)
#define SYS_CTRL_CLR_RST_UART3 (1 << 6)
#define SYS_CTRL_CLR_RST_UART4 (1 << 7)
#define SYS_CTRL_CLR_RST_UART5 (1 << 8)
#define SYS_CTRL_CLR_RST_SPI1 (1 << 9)
#define SYS_CTRL_CLR_RST_SPI2 (1 << 10)
#define SYS_CTRL_CLR_RST_GPIO2 (1 << 11)
#define SYS_CTRL_CLR_RST_GPT2 (1 << 12)
#define SYS_CTRL_CLR_RST_I2C1 (1 << 13)
#define SYS_CTRL_CLR_RST_I2C2 (1 << 14)
#define SYS_CTRL_CLR_RST_I2C3 (1 << 15)
#define SYS_CTRL_CLR_RST_SDMMC (1 << 16)
#define SYS_CTRL_CLR_RST_SEG_LCD (1 << 17)
#define SYS_CTRL_CLR_RST_KEYPAD (1 << 18)
#define SYS_CTRL_CLR_RST_I2S (1 << 19)
#define SYS_CTRL_CLR_RST_PAGE_SPY (1 << 20)

// sys_rst_set1
#define SYS_CTRL_SET_RST_EFUSE (1 << 0)
#define SYS_CTRL_SET_RST_IOMUX2 (1 << 1)
#define SYS_CTRL_SET_RST_FLASH (1 << 2)
#define SYS_CTRL_SET_RST_FLASH_EXT (1 << 3)
#define SYS_CTRL_SET_RST_ADI_IF (1 << 4)
#define SYS_CTRL_SET_RST_SCI2 (1 << 5)
#define SYS_CTRL_SET_RST_DBG (1 << 6)
#define SYS_CTRL_SET_RST_PSRAM (1 << 7)
#define SYS_CTRL_SET_RST_CE_TOP (1 << 8)
#define SYS_CTRL_SET_RST_MED (1 << 9)

// sys_rst_clr1
#define SYS_CTRL_CLR_RST_EFUSE (1 << 0)
#define SYS_CTRL_CLR_RST_IOMUX2 (1 << 1)
#define SYS_CTRL_CLR_RST_FLASH (1 << 2)
#define SYS_CTRL_CLR_RST_FLASH_EXT (1 << 3)
#define SYS_CTRL_CLR_RST_ADI_IF (1 << 4)
#define SYS_CTRL_CLR_RST_SCI2 (1 << 5)
#define SYS_CTRL_CLR_RST_DBG (1 << 6)
#define SYS_CTRL_CLR_RST_PSRAM (1 << 7)
#define SYS_CTRL_CLR_RST_CE_TOP (1 << 8)
#define SYS_CTRL_CLR_RST_MED (1 << 9)

// clk_sys_enable0
#define SYS_CTRL_ENABLE_STARMCU (1 << 0)
#define SYS_CTRL_ENABLE_SYS_DMA (1 << 1)
#define SYS_CTRL_ENABLE_SYS_DEC1 (1 << 2)
#define SYS_CTRL_ENABLE_SYS_DEC2 (1 << 3)
#define SYS_CTRL_ENABLE_TIMER2 (1 << 4)
#define SYS_CTRL_ENABLE_WDT (1 << 5)
#define SYS_CTRL_ENABLE_UART3 (1 << 6)
#define SYS_CTRL_ENABLE_UART4 (1 << 7)
#define SYS_CTRL_ENABLE_UART5 (1 << 8)
#define SYS_CTRL_ENABLE_SPI1 (1 << 9)
#define SYS_CTRL_ENABLE_SPI2 (1 << 10)
#define SYS_CTRL_ENABLE_GPIO2 (1 << 11)
#define SYS_CTRL_ENABLE_GPT2 (1 << 12)
#define SYS_CTRL_ENABLE_I2C1 (1 << 13)
#define SYS_CTRL_ENABLE_I2C2 (1 << 14)
#define SYS_CTRL_ENABLE_I2C3 (1 << 15)
#define SYS_CTRL_ENABLE_SDMMC (1 << 16)
#define SYS_CTRL_ENABLE_SEG_LCD (1 << 17)
#define SYS_CTRL_ENABLE_KEYPAD (1 << 18)
#define SYS_CTRL_ENABLE_I2S (1 << 19)
#define SYS_CTRL_ENABLE_PAGE_SPY (1 << 20)

// clk_sys_disable0
#define SYS_CTRL_DISABLE_STARMCU (1 << 0)
#define SYS_CTRL_DISABLE_SYS_DMA (1 << 1)
#define SYS_CTRL_DISABLE_SYS_DEC1 (1 << 2)
#define SYS_CTRL_DISABLE_SYS_DEC2 (1 << 3)
#define SYS_CTRL_DISABLE_TIMER2 (1 << 4)
#define SYS_CTRL_DISABLE_WDT (1 << 5)
#define SYS_CTRL_DISABLE_UART3 (1 << 6)
#define SYS_CTRL_DISABLE_UART4 (1 << 7)
#define SYS_CTRL_DISABLE_UART5 (1 << 8)
#define SYS_CTRL_DISABLE_SPI1 (1 << 9)
#define SYS_CTRL_DISABLE_SPI2 (1 << 10)
#define SYS_CTRL_DISABLE_GPIO2 (1 << 11)
#define SYS_CTRL_DISABLE_GPT2 (1 << 12)
#define SYS_CTRL_DISABLE_I2C1 (1 << 13)
#define SYS_CTRL_DISABLE_I2C2 (1 << 14)
#define SYS_CTRL_DISABLE_I2C3 (1 << 15)
#define SYS_CTRL_DISABLE_SDMMC (1 << 16)
#define SYS_CTRL_DISABLE_SEG_LCD (1 << 17)
#define SYS_CTRL_DISABLE_KEYPAD (1 << 18)
#define SYS_CTRL_DISABLE_I2S (1 << 19)
#define SYS_CTRL_DISABLE_PAGE_SPY (1 << 20)

// clk_sys_enable1
#define SYS_CTRL_ENABLE_EFUSE (1 << 0)
#define SYS_CTRL_ENABLE_IOMUX2 (1 << 1)
#define SYS_CTRL_ENABLE_FLASH (1 << 2)
#define SYS_CTRL_ENABLE_FLASH_EXT (1 << 3)
#define SYS_CTRL_ENABLE_ADI_IF (1 << 4)
#define SYS_CTRL_ENABLE_SCI2 (1 << 5)
#define SYS_CTRL_ENABLE_DBG (1 << 6)
#define SYS_CTRL_ENABLE_PSRAM (1 << 7)
#define SYS_CTRL_ENABLE_CE_TOP (1 << 8)
#define SYS_CTRL_ENABLE_MED (1 << 9)

// clk_sys_disable1
#define SYS_CTRL_DISABLE_EFUSE (1 << 0)
#define SYS_CTRL_DISABLE_IOMUX2 (1 << 1)
#define SYS_CTRL_DISABLE_FLASH (1 << 2)
#define SYS_CTRL_DISABLE_FLASH_EXT (1 << 3)
#define SYS_CTRL_DISABLE_ADI_IF (1 << 4)
#define SYS_CTRL_DISABLE_SCI2 (1 << 5)
#define SYS_CTRL_DISABLE_DBG (1 << 6)
#define SYS_CTRL_DISABLE_PSRAM (1 << 7)
#define SYS_CTRL_DISABLE_CE_TOP (1 << 8)
#define SYS_CTRL_DISABLE_MED (1 << 9)

// pll_ctrl
#define SYS_CTRL_MCU_PLL_PU (1 << 0)
#define SYS_CTRL_MCU_PLL_PD (1 << 1)
#define SYS_CTRL_MCU_PLL_LOCKED (1 << 2)
#define SYS_CTRL_NB_PLL_PU (1 << 4)
#define SYS_CTRL_NB_PLL_PD (1 << 5)
#define SYS_CTRL_NB_PLL_LOCKED (1 << 6)

// sel_clock_sys
#define SYS_CTRL_SEL_CLK_SLOW(n) (((n)&0x3) << 0)
#define SYS_CTRL_SEL_CLK_SYS (1 << 2)
#define SYS_CTRL_SEL_CLK_FLASH (1 << 3)
#define SYS_CTRL_SEL_CLK_FLASH_EXT (1 << 4)
#define SYS_CTRL_SEL_CLK_UART3(n) (((n)&0x3) << 5)
#define SYS_CTRL_SEL_CLK_UART4(n) (((n)&0x3) << 7)
#define SYS_CTRL_SEL_CLK_UART5(n) (((n)&0x3) << 9)
#define SYS_CTRL_SEL_CLK_DBG(n) (((n)&0x3) << 11)
#define SYS_CTRL_SEL_CLK_SPI1 (1 << 13)
#define SYS_CTRL_SEL_CLK_SPI2 (1 << 14)
#define SYS_CTRL_SEL_CLK_RFSPI (1 << 15)
#define SYS_CTRL_SEL_CLK_PSRAM (1 << 16)
#define SYS_CTRL_SEL_CLK_I2S (1 << 17)
#define SYS_CTRL_SEL_CLK_XTAL52M_BAK (1 << 21)
#define SYS_CTRL_SEL_CLK_EFUSE(n) (((n)&0x3) << 22)
#define SYS_CTRL_SEL_SST_CLK(n) (((n)&0x3) << 24)
#define SYS_CTRL_SEL_ST_CLK(n) (((n)&0x3) << 26)
#define SYS_CTRL_SEL_TIMER2_CLK(n) (((n)&0x3) << 28)

// cfg_clk_sys
#define SYS_CTRL_SYS_DIV_NUM(n) (((n)&0x1ff) << 0)
#define SYS_CTRL_SYS_DIV_DENOM(n) (((n)&0x1ff) << 9)
#define SYS_CTRL_SYS_PCLKEN_NUM(n) (((n)&0x3f) << 18)
#define SYS_CTRL_SYS_PCLKEN_DENOM(n) (((n)&0x3f) << 24)

// cfg_clk_spiflash
#define SYS_CTRL_SPIFLASH_FREQ(n) (((n)&0xf) << 0)

// cfg_clk_spiflash_ext
#define SYS_CTRL_SPIFLASH_EXT_FREQ(n) (((n)&0xf) << 0)

// cfg_clk_dbg
#define SYS_CTRL_DBG_DIV_NUM(n) (((n)&0x3ff) << 0)
#define SYS_CTRL_DBG_DIV_DENOM(n) (((n)&0x3fff) << 10)

// cfg_clk_uart3
#define SYS_CTRL_UART3_DIV_NUM(n) (((n)&0x3ff) << 0)
#define SYS_CTRL_UART3_DIV_DENOM(n) (((n)&0x3fff) << 10)

// cfg_clk_uart4
#define SYS_CTRL_UART4_DIV_NUM(n) (((n)&0x3ff) << 0)
#define SYS_CTRL_UART4_DIV_DENOM(n) (((n)&0x3fff) << 10)

// cfg_clk_uart5
#define SYS_CTRL_UART5_DIV_NUM(n) (((n)&0x3ff) << 0)
#define SYS_CTRL_UART5_DIV_DENOM(n) (((n)&0x3fff) << 10)

// cfg_clk_psram
#define SYS_CTRL_PSRAM_FREQ(n) (((n)&0xf) << 0)

// cfg_clk_dblr
#define SYS_CTRL_SYS_DBLR_DIV_DENOM(n) (((n)&0x3f) << 0)
#define SYS_CTRL_SYS_DBLR_DIV_DENOM2(n) (((n)&0x3f) << 6)
#define SYS_CTRL_MPLL_CLK_EN (1 << 12)
#define SYS_CTRL_CLKDBLR_IBIAS_CTRL(n) (((n)&0x3) << 18)
#define SYS_CTRL_CLKDBLR_VBIAS_CTRL(n) (((n)&0x3) << 20)
#define SYS_CTRL_CLKDBLR_R_CTRL(n) (((n)&0x7) << 22)
#define SYS_CTRL_CLKDBLR_CLK_EN (1 << 25)
#define SYS_CTRL_PU_CLKDBLR (1 << 26)
#define SYS_CTRL_DBLR_RSD(n) (((n)&0xf) << 27)

// cfg_clk_i2s
#define SYS_CTRL_I2S_DIV_DENOM(n) (((n)&0xffff) << 0)
#define SYS_CTRL_I2S_MCLK_DIV_DENOM(n) (((n)&0xffff) << 16)

// cfg_clk_out
#define SYS_CTRL_CLKOUT0_SEL(n) (((n)&0x7) << 0)
#define SYS_CTRL_CLKOUT1_SEL(n) (((n)&0x7) << 3)
#define SYS_CTRL_CLKOUT2_SEL(n) (((n)&0x7) << 6)
#define SYS_CTRL_CLKOUT3_SEL(n) (((n)&0x7) << 9)
#define SYS_CTRL_CLKOUT0_EN (1 << 28)
#define SYS_CTRL_CLKOUT1_EN (1 << 29)
#define SYS_CTRL_CLKOUT2_EN (1 << 30)
#define SYS_CTRL_CLKOUT3_EN (1 << 31)

// reset_cause
#define SYS_CTRL_GLOBALSOFT_RESET (1 << 0)
#define SYS_CTRL_DEBUGHOST_RESET (1 << 1)
#define SYS_CTRL_MCU_SYSRESETREQ_RESET (1 << 2)
#define SYS_CTRL_MCU_LOCKUP_RESET (1 << 3)
#define SYS_CTRL_WDT_RESET (1 << 4)
#define SYS_CTRL_CLK_MONITOR_32K_RESET (1 << 5)
#define SYS_CTRL_CLK_MONITOR_26M_RESET (1 << 6)
#define SYS_CTRL_BOOT_MODE(n) (((n)&0xf) << 16)
#define SYS_CTRL_SW_BOOT_MODE(n) (((n)&0xf) << 20)

// sys_wakeup_cause
#define SYS_CTRL_MCU_LPS_WAKEUP (1 << 0)
#define SYS_CTRL_MCU_GPT1_WAKEUP (1 << 1)
#define SYS_CTRL_MCU_UART1_WAKEUP (1 << 2)
#define SYS_CTRL_MCU_UART2_WAKEUP (1 << 3)
#define SYS_CTRL_MCU_TIMER1_WAKEUP (1 << 4)
#define SYS_CTRL_MCU_GPIO1_WAKEUP (1 << 5)
#define SYS_CTRL_MCU_DBG_WAKEUP (1 << 6)
#define SYS_CTRL_MCU_ANA_WAKEUP (1 << 7)
#define SYS_CTRL_MCU_EXT_RST_WAKEUP (1 << 8)

// misc_ctrl
#define SYS_CTRL_LOCKUP_RST_EN (1 << 0)
#define SYS_CTRL_WDT_RSTN_EN (1 << 1)
#define SYS_CTRL_CLK_MONITOR_32K_RSTN_EN (1 << 2)
#define SYS_CTRL_CLK_MONITOR_26M_RSTN_EN (1 << 3)
#define SYS_CTRL_HOST_RESET_L_EN (1 << 4)
#define SYS_CTRL_FORCE_XCPU_RESET_L_EN (1 << 5)
#define SYS_CTRL_PSRAM_ECO_EN (1 << 6)
#define SYS_CTRL_MEM_RETENTION (1 << 7)
#define SYS_CTRL_DBG_OUT_SEL(n) (((n)&0xf) << 8)
#define SYS_CTRL_DBG_TRIG_SEL(n) (((n)&0xf) << 12)
#define SYS_CTRL_DBG_CLK_SEL(n) (((n)&0xf) << 16)
#define SYS_CTRL_DBG_CLK_EN (1 << 20)
#define SYS_CTRL_HRESP_ERR_MASK_SYSIFC1 (1 << 21)
#define SYS_CTRL_HRESP_ERR_MASK_SYSIFC2 (1 << 22)
#define SYS_CTRL_ADI_SEL_PAD (1 << 23)
#define SYS_CTRL_TST_H_WPD (1 << 24)
#define SYS_CTRL_BOOT_MODE_WPD(n) (((n)&0x3) << 25)
#define SYS_CTRL_LOCKSAU (1 << 27)
#define SYS_CTRL_LOCKNSMPU (1 << 28)
#define SYS_CTRL_LOCKSMPU (1 << 29)
#define SYS_CTRL_LOCKNSVTOR (1 << 30)
#define SYS_CTRL_LOCKSVTAIRCR (1 << 31)

// pad_ctrl
#define SYS_CTRL_IBIT_FLASH(n) (((n)&0x7) << 0)
#define SYS_CTRL_IBIT_PSRAM(n) (((n)&0x7) << 3)
#define SYS_CTRL_IBIT_A_DIE_IF(n) (((n)&0x3) << 6)

// cpu_ctrl
#define SYS_CTRL_STARMCU_SLEEPING (1 << 0)
#define SYS_CTRL_STARMCU_SLEEPDEEP (1 << 1)
#define SYS_CTRL_STARMCU_WICENREQ (1 << 2)
#define SYS_CTRL_STARMCU_WICENACK (1 << 3)
#define SYS_CTRL_SOFT_CPU_SLEEP (1 << 4)

// otp_reg
#define SYS_CTRL_FORCE_ENABLE_SWD (1 << 0)
#define SYS_CTRL_FORCE_ENABLE_IDBG (1 << 1)
#define SYS_CTRL_FORCE_ENABLE_NIDBG (1 << 2)
#define SYS_CTRL_FORCE_ENABLE_SIDBG (1 << 3)
#define SYS_CTRL_FORCE_ENABLE_SNIDBG (1 << 4)
#define SYS_CTRL_FORCE_ENABLE_DBGHOST (1 << 5)
#define SYS_CTRL_OTP_REG_RSVD(n) (((n)&0x3f) << 6)

// mpll_setting0
#define SYS_CTRL_MPLL_SDM_CLK_SEL_DR (1 << 0)
#define SYS_CTRL_MPLL_SDM_CLK_SEL_REG (1 << 1)
#define SYS_CTRL_MPLL_REFMULTI2_EN (1 << 2)
#define SYS_CTRL_MPLL_PCON_MODE (1 << 3)
#define SYS_CTRL_MPLL_TEST_EN (1 << 4)
#define SYS_CTRL_MPLL_CP_OFFSET_EN (1 << 5)
#define SYS_CTRL_MPLL_CPBIAS_IBIT(n) (((n)&0x7) << 6)
#define SYS_CTRL_MPLL_CP_OFFSET(n) (((n)&0x3) << 9)
#define SYS_CTRL_MPLL_SDMCLK_DISABLE (1 << 11)
#define SYS_CTRL_MPLL_EN_REG (1 << 14)
#define SYS_CTRL_MPLL_EN_DR (1 << 15)
#define SYS_CTRL_MPLL_SEL_REG (1 << 16)
#define SYS_CTRL_MPLL_SEL_DR (1 << 17)
#define SYS_CTRL_MPLL_RSTB_REG (1 << 18)
#define SYS_CTRL_MPLL_RSTB_DR (1 << 19)
#define SYS_CTRL_MPLL_READY_TIMER(n) (((n)&0x3) << 20)
#define SYS_CTRL_MPLL_BAND_SEL(n) (((n)&0x3) << 22)
#define SYS_CTRL_MPLL_LOCK (1 << 24)
#define SYS_CTRL_MPLL_CLKTEST_OUT (1 << 25)

// mpll_setting1
#define SYS_CTRL_MPLL_SDM_DITHER_BYPASS (1 << 0)
#define SYS_CTRL_MPLL_SDM_INT_DEC_SEL(n) (((n)&0x7) << 1)
#define SYS_CTRL_MPLL_SDM_RESETN_REG (1 << 4)
#define SYS_CTRL_MPLL_SDM_RESETN_DR (1 << 5)
#define SYS_CTRL_MPLL_SDM_CLK_INV (1 << 6)
#define SYS_CTRL_MPLL_SDM_CLK_TEST_EN (1 << 16)
#define SYS_CTRL_MPLL_VCO_HIGH_TEST (1 << 17)
#define SYS_CTRL_MPLL_VCO_LOW_TEST (1 << 18)
#define SYS_CTRL_MPLL_INT_MODE (1 << 19)
#define SYS_CTRL_MPLL_TESTSIG_SEL(n) (((n)&0x3) << 20)
#define SYS_CTRL_MPLL_RSVD_REG(n) (((n)&0xff) << 22)

// mpll_sdm_freq
#define SYS_CTRL_MPLL_SDM_FREQ(n) (((n)&0xffffffff) << 0)

// dbg_disable_acg0
#define SYS_CTRL_DISABLE_ACG0(n) (((n)&0xffffffff) << 0)

// dbg_disable_acg1
#define SYS_CTRL_DISABLE_ACG1(n) (((n)&0xffffffff) << 0)

// dbg_disable_acg2
#define SYS_CTRL_DISABLE_ACG2(n) (((n)&0xffffffff) << 0)

// dbg_disable_acg3
#define SYS_CTRL_DISABLE_ACG3(n) (((n)&0xffffffff) << 0)

// cfg_clk_nbiot_fast
#define SYS_CTRL_NBIOT_FAST_DIV_NUM(n) (((n)&0xff) << 0)
#define SYS_CTRL_NBIOT_FAST_DIV_DENOM(n) (((n)&0xff) << 8)

// calib_th_32k
#define SYS_CTRL_CALIB_TH_32K(n) (((n)&0xffffffff) << 0)

// calib_th_rc26m
#define SYS_CTRL_CALIB_TH_RC26M(n) (((n)&0xffffffff) << 0)

// ctrl_of_32k_calib
#define SYS_CTRL_CALIB_EN_32K (1 << 0)
#define SYS_CTRL_CALIB_INT_EN_32K (1 << 1)
#define SYS_CTRL_CALIB_INT_CLR_32K (1 << 2)

// ctrl_of_rc26m_calib
#define SYS_CTRL_CALIB_EN_RC26M (1 << 0)
#define SYS_CTRL_CALIB_INT_EN_RC26M (1 << 1)
#define SYS_CTRL_CALIB_INT_CLR_RC26M (1 << 2)

// num_of_clk1_32k
#define SYS_CTRL_NUM_OF_CLK1_32K(n) (((n)&0xffffffff) << 0)

// num_of_clk1_rc26m
#define SYS_CTRL_NUM_OF_CLK1_RC26M(n) (((n)&0xffffffff) << 0)

// star_cfg_nsst
#define SYS_CTRL_STAR_NSSTCALIB(n) (((n)&0x3ffffff) << 0)

// star_cfg_sst
#define SYS_CTRL_STAR_SSTCALIB(n) (((n)&0x3ffffff) << 0)

// mem_cfg1
#define SYS_CTRL_MEM_CONFIG_1P(n) (((n)&0xffffffff) << 0)

// timer2_divider
#define SYS_CTRL_TIMER2_DIV_DENOM(n) (((n)&0xff) << 0)
#define SYS_CTRL_TIMER2_DIV_NUM(n) (((n)&0xff) << 8)

// st_divider
#define SYS_CTRL_ST_DIV_DENOM(n) (((n)&0xff) << 0)
#define SYS_CTRL_ST_DIV_NUM(n) (((n)&0xff) << 8)

// sst_divider
#define SYS_CTRL_SST_DIV_DENOM(n) (((n)&0xff) << 0)
#define SYS_CTRL_SST_DIV_NUM(n) (((n)&0xff) << 8)

// sys_ctrl_rsd0
#define SYS_CTRL_SYS_CTRL_RSD0(n) (((n)&0xffffffff) << 0)

// sys_ctrl_rsd1
#define SYS_CTRL_SYS_CTRL_RSD1(n) (((n)&0xffffffff) << 0)

// sys_ctrl_rsd2
#define SYS_CTRL_SYS_CTRL_RSD2(n) (((n)&0xffffffff) << 0)

// sys_ctrl_rsd3
#define SYS_CTRL_SYS_CTRL_RSD3(n) (((n)&0xffffffff) << 0)

// mem_cfg2
#define SYS_CTRL_MEM_CONFIG_2P(n) (((n)&0xffffffff) << 0)

// clk_mnt26m_th0
#define SYS_CTRL_CLK_MNT26M_TH0(n) (((n)&0xff) << 0)

// clk_mnt26m_th1
#define SYS_CTRL_CLK_MNT26M_TH1(n) (((n)&0xffff) << 0)

// clk_mnt26m_th2
#define SYS_CTRL_CLK_MNT26M_TH2(n) (((n)&0x7f) << 0)

// clk_mnt26m_th3
#define SYS_CTRL_CLK_MNT26M_TH3(n) (((n)&0x1ff) << 0)

// clk_mnt32k_th0
#define SYS_CTRL_CLK_MNT32K_TH0(n) (((n)&0xfff) << 0)

// clk_mnt32k_th1
#define SYS_CTRL_CLK_MNT32K_TH1(n) (((n)&0xfff) << 0)

// clk_mnt_ctrl
#define SYS_CTRL_CLK_MNT32K_EN (1 << 0)
#define SYS_CTRL_CLK_MNT26M_EN (1 << 1)
#define SYS_CTRL_EN_INT_CLK_MNT32K (1 << 2)
#define SYS_CTRL_EN_INT_CLK_MNT26M (1 << 3)
#define SYS_CTRL_ST_CLK_MNT32K (1 << 4)
#define SYS_CTRL_ST_CLK_MNT26M (1 << 5)

// clk_mnt26m_st
#define SYS_CTRL_CLR_INT_CLK_MNT26M (1 << 0)

// clk_mnt32k_st
#define SYS_CTRL_CLR_INT_CLK_MNT32K (1 << 0)

// dbg_ctrl
#define SYS_CTRL_CFG_CLKOUT_EN (1 << 0)
#define SYS_CTRL_CFG_CLKOUT_SEL(n) (((n)&0xf) << 1)
#define SYS_CTRL_MED_DEBUG_BUS_SEL(n) (((n)&0x1f) << 8)

// lp_irq
#define SYS_CTRL_TIMER1_OS_IRQ (1 << 0)
#define SYS_CTRL_TIMER1_IRQ (1 << 1)
#define SYS_CTRL_LPS_IRQ (1 << 2)
#define SYS_CTRL_PWR_CTRL_IRQ (1 << 3)
#define SYS_CTRL_GPT1_IRQ (1 << 4)
#define SYS_CTRL_GPIO1_IRQ (1 << 5)
#define SYS_CTRL_UART2_IRQ (1 << 6)
#define SYS_CTRL_UART1_IRQ (1 << 7)

// adi_if_evt
#define SYS_CTRL_ADI_IF_EVT0 (1 << 0)
#define SYS_CTRL_ADI_IF_EVT1 (1 << 1)

// cfg_clk_efuse
#define SYS_CTRL_EFUSE_DIV_DENOM(n) (((n)&0x7) << 0)

// rfspi_apb_sel
#define SYS_CTRL_RFSPI_APB_SEL_SW (1 << 0)
#define SYS_CTRL_RFSPI_APB_SEL_DR (1 << 1)

// cfg_clk_sram
#define SYS_CTRL_SRAM_DIV_DENOM(n) (((n)&0xf) << 0)

// sram_h2h_config
#define SYS_CTRL_SRAM_H2H_INCR_PRE_READ (1 << 0)
#define SYS_CTRL_SRAM_H2H_NOBUF_EARLY_RESP_EN (1 << 1)
#define SYS_CTRL_SRAM_H2H_ACG_EN (1 << 2)

#endif // _SYS_CTRL_H_
