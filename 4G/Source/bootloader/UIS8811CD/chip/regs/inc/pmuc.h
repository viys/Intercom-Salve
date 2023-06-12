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

#ifndef _PMUC_H_
#define _PMUC_H_

// Auto generated (v1.0-38-g167fa99). Don't edit it manually!

#define REG_PWR_CTRL_BASE (0x41804000)

typedef volatile struct
{
    uint32_t clock_select;                // 0x00000000
    uint32_t xtal26m_to_32k_divider_ctrl; // 0x00000004
    uint32_t clk_uart1_cfg;               // 0x00000008
    uint32_t clk_uart2_cfg;               // 0x0000000c
    uint32_t rc26m_div_cfg;               // 0x00000010
    uint32_t ip_clk_disable_ctrl;         // 0x00000014
    uint32_t ip_soft_rst_ctrl;            // 0x00000018
    uint32_t rc26m_ctrl;                  // 0x0000001c
    uint32_t pmu_wakeup_mask;             // 0x00000020
    uint32_t pmu_wakeup_clr;              // 0x00000024
    uint32_t pmu_wakeup_status;           // 0x00000028
    uint32_t pmuc_int_mask;               // 0x0000002c
    uint32_t pmuc_int_clr;                // 0x00000030
    uint32_t pmuc_int_status;             // 0x00000034
    uint32_t power_status;                // 0x00000038
    uint32_t mem_power_status_0;          // 0x0000003c
    uint32_t mem_power_status_1;          // 0x00000040
    uint32_t power_mode_ctrl_0;           // 0x00000044
    uint32_t power_mode_ctrl_1;           // 0x00000048
    uint32_t delay_time_ctrl;             // 0x0000004c
    uint32_t pmu_power_dr_ctrl;           // 0x00000050
    uint32_t mem_power_ctrl_0;            // 0x00000054
    uint32_t mem_power_ctrl_1;            // 0x00000058
    uint32_t mem_power_ctrl_2;            // 0x0000005c
    uint32_t mem_power_ctrl_3;            // 0x00000060
    uint32_t dcxo_ctrl_0;                 // 0x00000064
    uint32_t dcxo_ctrl_1;                 // 0x00000068
    uint32_t dcxo_lp_ctrl_0;              // 0x0000006c
    uint32_t dcxo_lp_ctrl_1;              // 0x00000070
    uint32_t pad_ctrl;                    // 0x00000074
    uint32_t clk_26m_pmic_cfg;            // 0x00000078
    uint32_t boundary_test_ctrl;          // 0x0000007c
    uint32_t pmu_rsvd_reg_0;              // 0x00000080
    uint32_t pmu_rsvd_reg_1;              // 0x00000084
    uint32_t pmu_rsvd_reg_2;              // 0x00000088
    uint32_t pmu_rsvd_reg_3;              // 0x0000008c
    uint32_t pmu_rsvd_reg_4;              // 0x00000090
    uint32_t pmu_rsvd_reg_5;              // 0x00000094
    uint32_t pmu_rsvd_reg_6;              // 0x00000098
    uint32_t pmu_rsvd_reg_7;              // 0x0000009c
    uint32_t pmu_rsvd_reg_8;              // 0x000000a0
    uint32_t rsvd_ports;                  // 0x000000a4
    uint32_t otp_reg;                     // 0x000000a8
    uint32_t clk_xtal52m_div_cfg;         // 0x000000ac
} HWP_PMUC_T;

#define hwp_pwrCtrl ((HWP_PMUC_T *)REG_ACCESS_ADDRESS(REG_PWR_CTRL_BASE))

// clock_select
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sel_32k_src : 1;        // [0]
        uint32_t sel_pclk_lptop : 2;     // [2:1]
        uint32_t pm2_sel_pclk_lptop : 2; // [4:3]
        uint32_t sel_pclk_gpt : 2;       // [6:5]
        uint32_t __31_7 : 25;            // [31:7]
    } b;
} REG_PMUC_CLOCK_SELECT_T;

// xtal26m_to_32k_divider_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t step_offset_update : 1; // [0]
        uint32_t div_lp_mode_h_dr : 1;   // [1]
        uint32_t div_lp_mode_h_reg : 1;  // [2]
        uint32_t step_offset_lp : 13;    // [15:3]
        uint32_t step_offset_nor : 13;   // [28:16]
        uint32_t xtal_clk6m5_en : 1;     // [29]
        uint32_t div_lp_mode_h : 1;      // [30], read only
        uint32_t __31_31 : 1;            // [31]
    } b;
} REG_PMUC_XTAL26M_TO_32K_DIVIDER_CTRL_T;

// clk_uart1_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t uart1_div_num : 10;            // [9:0]
        uint32_t uart1_div_denom : 14;          // [23:10]
        uint32_t uart1_div_update : 1;          // [24], write clear
        uint32_t sel_clk_uart1 : 2;             // [26:25]
        uint32_t dbg_disable_acg_clk_uart1 : 1; // [27]
        uint32_t ip_clk_disable_uart1 : 1;      // [28]
        uint32_t __31_29 : 3;                   // [31:29]
    } b;
} REG_PMUC_CLK_UART1_CFG_T;

// clk_uart2_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t uart2_div_num : 10;            // [9:0]
        uint32_t uart2_div_denom : 14;          // [23:10]
        uint32_t uart2_div_update : 1;          // [24], write clear
        uint32_t sel_clk_uart2 : 2;             // [26:25]
        uint32_t dbg_disable_acg_clk_uart2 : 1; // [27]
        uint32_t ip_clk_disable_uart2 : 1;      // [28]
        uint32_t __31_29 : 3;                   // [31:29]
    } b;
} REG_PMUC_CLK_UART2_CFG_T;

// rc26m_div_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rc26m_div_num : 10;   // [9:0]
        uint32_t rc26m_div_denom : 14; // [23:10]
        uint32_t rc26m_div_update : 1; // [24], write clear
        uint32_t __31_25 : 7;          // [31:25]
    } b;
} REG_PMUC_RC26M_DIV_CFG_T;

// ip_clk_disable_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t timer_clk_disable : 1;    // [0]
        uint32_t lps_clk_disable : 1;      // [1]
        uint32_t uart1_clk_disable : 1;    // [2]
        uint32_t uart2_clk_disable : 1;    // [3]
        uint32_t gpt1_clk_disable : 1;     // [4]
        uint32_t gpio_clk_disable : 1;     // [5]
        uint32_t iomux_clk_disable : 1;    // [6]
        uint32_t dbg_disable_acg_pclk : 7; // [13:7]
        uint32_t __31_14 : 18;             // [31:14]
    } b;
} REG_PMUC_IP_CLK_DISABLE_CTRL_T;

// ip_soft_rst_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t timer_soft_rst : 1;   // [0]
        uint32_t lps_soft_rst : 1;     // [1]
        uint32_t uart1_soft_rst : 1;   // [2]
        uint32_t uart2_soft_rst : 1;   // [3]
        uint32_t gpt1_soft_rst : 1;    // [4]
        uint32_t gpio_soft_rst : 1;    // [5]
        uint32_t iomux_soft_rst : 1;   // [6]
        uint32_t pmu_reg_soft_rst : 1; // [7]
        uint32_t __31_8 : 24;          // [31:8]
    } b;
} REG_PMUC_IP_SOFT_RST_CTRL_T;

// rc26m_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rg_nb_rc_osc_cap_tune : 3; // [2:0]
        uint32_t rg_nb_rc_osc_res_ctrl : 4; // [6:3]
        uint32_t rg_nb_ibias_ctrl : 1;      // [7]
        uint32_t rg_nb_rc_osc_pu : 1;       // [8]
        uint32_t pu_rc26m_dr : 1;           // [9]
        uint32_t rg_nb_rc_osc_rsvd : 4;     // [13:10]
        uint32_t __31_14 : 18;              // [31:14]
    } b;
} REG_PMUC_RC26M_CTRL_T;

// pmu_wakeup_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bypass_gpio_wakeup : 1;  // [0]
        uint32_t bypass_uart1_wakeup : 1; // [1]
        uint32_t bypass_uart2_wakeup : 1; // [2]
        uint32_t bypass_timer_wakeup : 1; // [3]
        uint32_t bypass_gpt_wakeup : 1;   // [4]
        uint32_t bypass_lps_wakeup : 1;   // [5]
        uint32_t bypass_dbg_wakeup : 1;   // [6]
        uint32_t bypass_ana_wakeup : 1;   // [7]
        uint32_t __31_8 : 24;             // [31:8]
    } b;
} REG_PMUC_PMU_WAKEUP_MASK_T;

// pmu_wakeup_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpio_wakeup_clr : 1;    // [0], write clear
        uint32_t uart1_wakeup_clr : 1;   // [1], write clear
        uint32_t uart2_wakeup_clr : 1;   // [2], write clear
        uint32_t timer_wakeup_clr : 1;   // [3], write clear
        uint32_t gpt_wakeup_clr : 1;     // [4], write clear
        uint32_t lps_wakeup_clr : 1;     // [5], write clear
        uint32_t dbg_wakeup_clr : 1;     // [6], write clear
        uint32_t ana_wakeup_clr : 1;     // [7], write clear
        uint32_t ext_rst_wakeup_clr : 1; // [8], write clear
        uint32_t __31_9 : 23;            // [31:9]
    } b;
} REG_PMUC_PMU_WAKEUP_CLR_T;

// pmu_wakeup_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t gpio_wakeup_status : 1;    // [0], read only
        uint32_t uart1_wakeup_status : 1;   // [1], read only
        uint32_t uart2_wakeup_status : 1;   // [2], read only
        uint32_t timer_wakeup_status : 1;   // [3], read only
        uint32_t gpt_wakeup_status : 1;     // [4], read only
        uint32_t lps_wakeup_status : 1;     // [5], read only
        uint32_t dbg_wakeup_status : 1;     // [6], read only
        uint32_t ana_wakeup_status : 1;     // [7], read only
        uint32_t ext_rst_wakeup_status : 1; // [8], read only
        uint32_t __31_9 : 23;               // [31:9]
    } b;
} REG_PMUC_PMU_WAKEUP_STATUS_T;

// pmuc_int_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pmu_pu_done_int_mask : 1;  // [0]
        uint32_t pmu_pu_ready_int_mask : 1; // [1]
        uint32_t bua_det_int_mask : 1;      // [2]
        uint32_t __31_3 : 29;               // [31:3]
    } b;
} REG_PMUC_PMUC_INT_MASK_T;

// pmuc_int_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pmu_pu_done_int_clr : 1;  // [0], write clear
        uint32_t pmu_pu_ready_int_clr : 1; // [1], write clear
        uint32_t bua_det_int_clr : 1;      // [2], write clear
        uint32_t __31_3 : 29;              // [31:3]
    } b;
} REG_PMUC_PMUC_INT_CLR_T;

// pmuc_int_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pmu_pu_done_int_status : 1;  // [0], read only
        uint32_t pmu_pu_ready_int_status : 1; // [1], read only
        uint32_t bua_det_int_status : 1;      // [2], read only
        uint32_t pmu_pu_done_int_cause : 1;   // [3], read only
        uint32_t pmu_pu_ready_int_cause : 1;  // [4], read only
        uint32_t bua_det_int_cause : 1;       // [5], read only
        uint32_t __31_6 : 26;                 // [31:6]
    } b;
} REG_PMUC_PMUC_INT_STATUS_T;

// power_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pmu_pm_state : 3;      // [2:0], read only
        uint32_t first_pwron_state : 3; // [5:3], read only
        uint32_t pmu_pm02_sw_state : 5; // [10:6], read only
        uint32_t pmu_wakeup_exist : 1;  // [11], read only
        uint32_t pmu_pu_ready : 1;      // [12], read only
        uint32_t pmu_pu_done : 1;       // [13], read only
        uint32_t chip_sleep : 1;        // [14], read only
        uint32_t pu_xtal26m : 1;        // [15], read only
        uint32_t xtal26m_lp_mode : 1;   // [16], read only
        uint32_t pu_rc26m : 1;          // [17], read only
        uint32_t pu_rf_ana : 1;         // [18], read only
        uint32_t pu_bb_top : 1;         // [19], read only
        uint32_t resetb_bb_top : 1;     // [20], read only
        uint32_t iso_bb_top : 1;        // [21], read only
        uint32_t resetb_dbb : 1;        // [22], read only
        uint32_t iso_lptop : 1;         // [23], read only
        uint32_t pu_dbb : 1;            // [24], read only
        uint32_t __31_25 : 7;           // [31:25]
    } b;
} REG_PMUC_POWER_STATUS_T;

// mem_power_status_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ram_ret1n : 15; // [14:0], read only
        uint32_t ram_ret2n : 15; // [29:15], read only
        uint32_t __31_30 : 2;    // [31:30]
    } b;
} REG_PMUC_MEM_POWER_STATUS_0_T;

// mem_power_status_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ram_vce_pu : 15; // [14:0], read only
        uint32_t ram_pgen : 15;   // [29:15], read only
        uint32_t __31_30 : 2;     // [31:30]
    } b;
} REG_PMUC_MEM_POWER_STATUS_1_T;

// power_mode_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t power_mode : 2;                    // [1:0]
        uint32_t pm2_pd_rc26m : 1;                  // [2]
        uint32_t pm2_xtal26m_lp_mode : 1;           // [3]
        uint32_t pm2_wakeup_xtal26m_mode : 2;       // [5:4]
        uint32_t pm2_wakeup_pu_rf : 1;              // [6]
        uint32_t pm2_wakeup_pu_bb : 1;              // [7]
        uint32_t reg_pd_rc26m : 1;                  // [8], write clear
        uint32_t reg_pu_rc26m : 1;                  // [9], write clear
        uint32_t reg_lp_xtal26m : 1;                // [10], write clear
        uint32_t reg_pd_xtal26m : 1;                // [11], write clear
        uint32_t reg_pu_xtal26m : 1;                // [12], write clear
        uint32_t reg_pu_rf_ana : 1;                 // [13], write clear
        uint32_t reg_pd_rf_ana : 1;                 // [14], write clear
        uint32_t reg_pu_bb_top : 1;                 // [15], write clear
        uint32_t reg_pd_bb_top : 1;                 // [16], write clear
        uint32_t reg_pu_done : 1;                   // [17]
        uint32_t mem_ret_mode : 1;                  // [18]
        uint32_t efuse_read_disable : 1;            // [19]
        uint32_t bua_det_normal_func_enable : 1;    // [20]
        uint32_t bua_det_latch : 1;                 // [21], read only
        uint32_t pm2_skip_wait_xtal_before_dbb : 1; // [22]
        uint32_t __31_23 : 9;                       // [31:23]
    } b;
} REG_PMUC_POWER_MODE_CTRL_0_T;

// power_mode_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm2_pd_ram : 15; // [14:0]
        uint32_t __31_15 : 17;    // [31:15]
    } b;
} REG_PMUC_POWER_MODE_CTRL_1_T;

// delay_time_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xtal_rdy_time : 7;    // [6:0]
        uint32_t dbb_rst_rls_time : 7; // [13:7]
        uint32_t adie_pu_time : 8;     // [21:14]
        uint32_t vrf_pd_time : 4;      // [25:22]
        uint32_t __31_26 : 6;          // [31:26]
    } b;
} REG_PMUC_DELAY_TIME_CTRL_T;

// pmu_power_dr_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_chip_sleep : 1;         // [0]
        uint32_t chip_sleep_dr : 1;          // [1]
        uint32_t reg_pmu_pu_done : 1;        // [2]
        uint32_t pmu_pu_done_dr : 1;         // [3]
        uint32_t reg_pmu_pu_ready : 1;       // [4]
        uint32_t pmu_pu_ready_dr : 1;        // [5]
        uint32_t reg_pu_xtal26m : 1;         // [6]
        uint32_t pu_xtal26m_dr : 1;          // [7]
        uint32_t reg_xtal26m_lp_mode_en : 1; // [8]
        uint32_t xtal26m_lp_mode_dr : 1;     // [9]
        uint32_t pu_bb_top_dr : 1;           // [10]
        uint32_t reg_pu_bb_top : 1;          // [11]
        uint32_t iso_bb_top_dr : 1;          // [12]
        uint32_t reg_iso_bb_top : 1;         // [13]
        uint32_t resetb_bb_top_dr : 1;       // [14]
        uint32_t reg_resetb_bb_top : 1;      // [15]
        uint32_t reg_pu_rf_ana : 1;          // [16]
        uint32_t pu_rf_ana_dr : 1;           // [17]
        uint32_t reg_resetb_dbb : 1;         // [18]
        uint32_t resetb_dbb_dr : 1;          // [19]
        uint32_t reg_iso_lptop : 1;          // [20]
        uint32_t iso_lptop_dr : 1;           // [21]
        uint32_t reg_pu_dbb : 1;             // [22]
        uint32_t pu_dbb_dr : 1;              // [23]
        uint32_t resetb_efs_dr : 1;          // [24]
        uint32_t reg_resetb_efs : 1;         // [25]
        uint32_t pclk_sw_dr : 1;             // [26]
        uint32_t psram_io_ret : 1;           // [27]
        uint32_t sim_io_ret : 2;             // [29:28]
        uint32_t __31_30 : 2;                // [31:30]
    } b;
} REG_PMUC_PMU_POWER_DR_CTRL_T;

// mem_power_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_ram_vce_pu : 15; // [14:0]
        uint32_t ram_vce_pu_dr : 15;  // [29:15]
        uint32_t __31_30 : 2;         // [31:30]
    } b;
} REG_PMUC_MEM_POWER_CTRL_0_T;

// mem_power_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_ram_pgen : 15; // [14:0]
        uint32_t ram_pgen_dr : 15;  // [29:15]
        uint32_t __31_30 : 2;       // [31:30]
    } b;
} REG_PMUC_MEM_POWER_CTRL_1_T;

// mem_power_ctrl_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_ram_ret1n : 15; // [14:0]
        uint32_t ram_ret_dr : 15;    // [29:15]
        uint32_t __31_30 : 2;        // [31:30]
    } b;
} REG_PMUC_MEM_POWER_CTRL_2_T;

// mem_power_ctrl_3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reg_ram_ret2n : 15; // [14:0]
        uint32_t __31_15 : 17;       // [31:15]
    } b;
} REG_PMUC_MEM_POWER_CTRL_3_T;

// dcxo_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xtal_clk1_en : 1;   // [0]
        uint32_t xtal_clk2_en : 1;   // [1]
        uint32_t xtal_clk3_en : 1;   // [2]
        uint32_t xtal_clk4_en : 1;   // [3]
        uint32_t xtal_clk5_en : 1;   // [4]
        uint32_t xtal_clk6_en : 1;   // [5]
        uint32_t xtal_clk7_en : 1;   // [6]
        uint32_t xtal_clk8_en : 1;   // [7]
        uint32_t xtal_clk9_en : 1;   // [8]
        uint32_t xtal_clk10_en : 1;  // [9]
        uint32_t xtal_clk11_en : 1;  // [10]
        uint32_t xtal_capbank : 8;   // [18:11]
        uint32_t xtal_fix_ibit : 5;  // [23:19]
        uint32_t xtal_vamp_ibit : 4; // [27:24]
        uint32_t xtal_reg_bit : 4;   // [31:28]
    } b;
} REG_PMUC_DCXO_CTRL_0_T;

// dcxo_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xdrv_drv_en : 4;    // [3:0]
        uint32_t xdrv_pu_gps : 1;    // [4]
        uint32_t xdrv_reg_bit : 4;   // [8:5]
        uint32_t xdrv_pu : 1;        // [9]
        uint32_t xtal_rsvd : 4;      // [13:10]
        uint32_t xtal_ck26m_en : 1;  // [14]
        uint32_t xtal_ck6m5_en : 1;  // [15]
        uint32_t xtal_rda_hlsel : 2; // [17:16]
        uint32_t xtal_din : 14;      // [31:18]
    } b;
} REG_PMUC_DCXO_CTRL_1_T;

// dcxo_lp_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xtal_din : 14; // [13:0]
        uint32_t __31_14 : 18;  // [31:14]
    } b;
} REG_PMUC_DCXO_LP_CTRL_0_T;

// dcxo_lp_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t xtal_rda_hlsel : 2; // [1:0]
        uint32_t xtal_capbank : 8;   // [9:2]
        uint32_t xtal_fix_ibit : 5;  // [14:10]
        uint32_t xtal_vamp_ibit : 4; // [18:15]
        uint32_t __31_19 : 13;       // [31:19]
    } b;
} REG_PMUC_DCXO_LP_CTRL_1_T;

// pad_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adi_scl_pd : 1;   // [0]
        uint32_t adi_scl_pu : 2;   // [2:1]
        uint32_t adi_sda_se : 1;   // [3]
        uint32_t adi_sda_ie : 1;   // [4]
        uint32_t adi_sda_pd : 1;   // [5]
        uint32_t adi_sda_pu : 2;   // [7:6]
        uint32_t sleep_wpd : 1;    // [8]
        uint32_t sleep_wpu : 2;    // [10:9]
        uint32_t clk_26m_wpd : 1;  // [11]
        uint32_t clk_26m_wpu : 2;  // [13:12]
        uint32_t ibit_adie_if : 2; // [15:14]
        uint32_t __31_16 : 16;     // [31:16]
    } b;
} REG_PMUC_PAD_CTRL_T;

// clk_26m_pmic_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t div_denom : 14; // [13:0]
        uint32_t div_num : 10;   // [23:14]
        uint32_t div_update : 1; // [24], write clear
        uint32_t src_sel : 1;    // [25]
        uint32_t __31_26 : 6;    // [31:26]
    } b;
} REG_PMUC_CLK_26M_PMIC_CFG_T;

// boundary_test_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;       // [0]
        uint32_t clk_26m_pmic : 1; // [1]
        uint32_t bua_det : 1;      // [2], read only
        uint32_t ana_int : 1;      // [3], read only
        uint32_t resetb_ext : 1;   // [4], read only
        uint32_t clk_32k_ext : 1;  // [5], read only
        uint32_t __31_6 : 26;      // [31:6]
    } b;
} REG_PMUC_BOUNDARY_TEST_CTRL_T;

// rsvd_ports
typedef union {
    uint32_t v;
    struct
    {
        uint32_t out : 8;      // [7:0]
        uint32_t in : 8;       // [15:8], read only
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_PMUC_RSVD_PORTS_T;

// otp_reg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t otp_programmed : 1;       // [0]
        uint32_t force_enable_swd : 1;     // [1]
        uint32_t force_enable_idbg : 1;    // [2]
        uint32_t force_enable_nidbg : 1;   // [3]
        uint32_t force_enable_sidbg : 1;   // [4]
        uint32_t force_enable_snidbg : 1;  // [5]
        uint32_t force_enable_dbghost : 1; // [6]
        uint32_t otp_reg_rsvd : 6;         // [12:7]
        uint32_t __31_13 : 19;             // [31:13]
    } b;
} REG_PMUC_OTP_REG_T;

// clk_xtal52m_div_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t denom : 14;  // [13:0]
        uint32_t num : 10;    // [23:14]
        uint32_t update : 1;  // [24], write clear
        uint32_t __31_25 : 7; // [31:25]
    } b;
} REG_PMUC_CLK_XTAL52M_DIV_CFG_T;

// clock_select
#define PMUC_SEL_32K_SRC (1 << 0)
#define PMUC_SEL_PCLK_LPTOP(n) (((n)&0x3) << 1)
#define PMUC_PM2_SEL_PCLK_LPTOP(n) (((n)&0x3) << 3)
#define PMUC_SEL_PCLK_GPT(n) (((n)&0x3) << 5)

// xtal26m_to_32k_divider_ctrl
#define PMUC_STEP_OFFSET_UPDATE (1 << 0)
#define PMUC_DIV_LP_MODE_H_DR (1 << 1)
#define PMUC_DIV_LP_MODE_H_REG (1 << 2)
#define PMUC_STEP_OFFSET_LP(n) (((n)&0x1fff) << 3)
#define PMUC_STEP_OFFSET_NOR(n) (((n)&0x1fff) << 16)
#define PMUC_XTAL_CLK6M5_EN (1 << 29)
#define PMUC_DIV_LP_MODE_H (1 << 30)

// clk_uart1_cfg
#define PMUC_UART1_DIV_NUM(n) (((n)&0x3ff) << 0)
#define PMUC_UART1_DIV_DENOM(n) (((n)&0x3fff) << 10)
#define PMUC_UART1_DIV_UPDATE (1 << 24)
#define PMUC_SEL_CLK_UART1(n) (((n)&0x3) << 25)
#define PMUC_DBG_DISABLE_ACG_CLK_UART1 (1 << 27)
#define PMUC_IP_CLK_DISABLE_UART1 (1 << 28)

// clk_uart2_cfg
#define PMUC_UART2_DIV_NUM(n) (((n)&0x3ff) << 0)
#define PMUC_UART2_DIV_DENOM(n) (((n)&0x3fff) << 10)
#define PMUC_UART2_DIV_UPDATE (1 << 24)
#define PMUC_SEL_CLK_UART2(n) (((n)&0x3) << 25)
#define PMUC_DBG_DISABLE_ACG_CLK_UART2 (1 << 27)
#define PMUC_IP_CLK_DISABLE_UART2 (1 << 28)

// rc26m_div_cfg
#define PMUC_RC26M_DIV_NUM(n) (((n)&0x3ff) << 0)
#define PMUC_RC26M_DIV_DENOM(n) (((n)&0x3fff) << 10)
#define PMUC_RC26M_DIV_UPDATE (1 << 24)

// ip_clk_disable_ctrl
#define PMUC_TIMER_CLK_DISABLE (1 << 0)
#define PMUC_LPS_CLK_DISABLE (1 << 1)
#define PMUC_UART1_CLK_DISABLE (1 << 2)
#define PMUC_UART2_CLK_DISABLE (1 << 3)
#define PMUC_GPT1_CLK_DISABLE (1 << 4)
#define PMUC_GPIO_CLK_DISABLE (1 << 5)
#define PMUC_IOMUX_CLK_DISABLE (1 << 6)
#define PMUC_DBG_DISABLE_ACG_PCLK(n) (((n)&0x7f) << 7)

// ip_soft_rst_ctrl
#define PMUC_TIMER_SOFT_RST (1 << 0)
#define PMUC_LPS_SOFT_RST (1 << 1)
#define PMUC_UART1_SOFT_RST (1 << 2)
#define PMUC_UART2_SOFT_RST (1 << 3)
#define PMUC_GPT1_SOFT_RST (1 << 4)
#define PMUC_GPIO_SOFT_RST (1 << 5)
#define PMUC_IOMUX_SOFT_RST (1 << 6)
#define PMUC_PMU_REG_SOFT_RST (1 << 7)

// rc26m_ctrl
#define PMUC_RG_NB_RC_OSC_CAP_TUNE(n) (((n)&0x7) << 0)
#define PMUC_RG_NB_RC_OSC_RES_CTRL(n) (((n)&0xf) << 3)
#define PMUC_RG_NB_IBIAS_CTRL (1 << 7)
#define PMUC_RG_NB_RC_OSC_PU (1 << 8)
#define PMUC_PU_RC26M_DR (1 << 9)
#define PMUC_RG_NB_RC_OSC_RSVD(n) (((n)&0xf) << 10)

// pmu_wakeup_mask
#define PMUC_BYPASS_GPIO_WAKEUP (1 << 0)
#define PMUC_BYPASS_UART1_WAKEUP (1 << 1)
#define PMUC_BYPASS_UART2_WAKEUP (1 << 2)
#define PMUC_BYPASS_TIMER_WAKEUP (1 << 3)
#define PMUC_BYPASS_GPT_WAKEUP (1 << 4)
#define PMUC_BYPASS_LPS_WAKEUP (1 << 5)
#define PMUC_BYPASS_DBG_WAKEUP (1 << 6)
#define PMUC_BYPASS_ANA_WAKEUP (1 << 7)

// pmu_wakeup_clr
#define PMUC_GPIO_WAKEUP_CLR (1 << 0)
#define PMUC_UART1_WAKEUP_CLR (1 << 1)
#define PMUC_UART2_WAKEUP_CLR (1 << 2)
#define PMUC_TIMER_WAKEUP_CLR (1 << 3)
#define PMUC_GPT_WAKEUP_CLR (1 << 4)
#define PMUC_LPS_WAKEUP_CLR (1 << 5)
#define PMUC_DBG_WAKEUP_CLR (1 << 6)
#define PMUC_ANA_WAKEUP_CLR (1 << 7)
#define PMUC_EXT_RST_WAKEUP_CLR (1 << 8)

// pmu_wakeup_status
#define PMUC_GPIO_WAKEUP_STATUS (1 << 0)
#define PMUC_UART1_WAKEUP_STATUS (1 << 1)
#define PMUC_UART2_WAKEUP_STATUS (1 << 2)
#define PMUC_TIMER_WAKEUP_STATUS (1 << 3)
#define PMUC_GPT_WAKEUP_STATUS (1 << 4)
#define PMUC_LPS_WAKEUP_STATUS (1 << 5)
#define PMUC_DBG_WAKEUP_STATUS (1 << 6)
#define PMUC_ANA_WAKEUP_STATUS (1 << 7)
#define PMUC_EXT_RST_WAKEUP_STATUS (1 << 8)

// pmuc_int_mask
#define PMUC_PMU_PU_DONE_INT_MASK (1 << 0)
#define PMUC_PMU_PU_READY_INT_MASK (1 << 1)
#define PMUC_BUA_DET_INT_MASK (1 << 2)

// pmuc_int_clr
#define PMUC_PMU_PU_DONE_INT_CLR (1 << 0)
#define PMUC_PMU_PU_READY_INT_CLR (1 << 1)
#define PMUC_BUA_DET_INT_CLR (1 << 2)

// pmuc_int_status
#define PMUC_PMU_PU_DONE_INT_STATUS (1 << 0)
#define PMUC_PMU_PU_READY_INT_STATUS (1 << 1)
#define PMUC_BUA_DET_INT_STATUS (1 << 2)
#define PMUC_PMU_PU_DONE_INT_CAUSE (1 << 3)
#define PMUC_PMU_PU_READY_INT_CAUSE (1 << 4)
#define PMUC_BUA_DET_INT_CAUSE (1 << 5)

// power_status
#define PMUC_PMU_PM_STATE(n) (((n)&0x7) << 0)
#define PMUC_FIRST_PWRON_STATE(n) (((n)&0x7) << 3)
#define PMUC_PMU_PM02_SW_STATE(n) (((n)&0x1f) << 6)
#define PMUC_PMU_WAKEUP_EXIST (1 << 11)
#define PMUC_PMU_PU_READY (1 << 12)
#define PMUC_PMU_PU_DONE (1 << 13)
#define PMUC_CHIP_SLEEP (1 << 14)
#define PMUC_PU_XTAL26M (1 << 15)
#define PMUC_XTAL26M_LP_MODE (1 << 16)
#define PMUC_PU_RC26M (1 << 17)
#define PMUC_PU_RF_ANA (1 << 18)
#define PMUC_PU_BB_TOP (1 << 19)
#define PMUC_RESETB_BB_TOP (1 << 20)
#define PMUC_ISO_BB_TOP (1 << 21)
#define PMUC_RESETB_DBB (1 << 22)
#define PMUC_ISO_LPTOP (1 << 23)
#define PMUC_PU_DBB (1 << 24)

// mem_power_status_0
#define PMUC_RAM_RET1N(n) (((n)&0x7fff) << 0)
#define PMUC_RAM_RET2N(n) (((n)&0x7fff) << 15)

// mem_power_status_1
#define PMUC_RAM_VCE_PU(n) (((n)&0x7fff) << 0)
#define PMUC_RAM_PGEN(n) (((n)&0x7fff) << 15)

// power_mode_ctrl_0
#define PMUC_POWER_MODE(n) (((n)&0x3) << 0)
#define PMUC_PM2_PD_RC26M (1 << 2)
#define PMUC_PM2_XTAL26M_LP_MODE (1 << 3)
#define PMUC_PM2_WAKEUP_XTAL26M_MODE(n) (((n)&0x3) << 4)
#define PMUC_PM2_WAKEUP_PU_RF (1 << 6)
#define PMUC_PM2_WAKEUP_PU_BB (1 << 7)
#define PMUC_REG_PD_RC26M (1 << 8)
#define PMUC_REG_PU_RC26M (1 << 9)
#define PMUC_REG_LP_XTAL26M (1 << 10)
#define PMUC_REG_PD_XTAL26M (1 << 11)
#define PMUC_POWER_MODE_CTRL_0_REG_PU_XTAL26M (1 << 12)
#define PMUC_POWER_MODE_CTRL_0_REG_PU_RF_ANA (1 << 13)
#define PMUC_REG_PD_RF_ANA (1 << 14)
#define PMUC_POWER_MODE_CTRL_0_REG_PU_BB_TOP (1 << 15)
#define PMUC_REG_PD_BB_TOP (1 << 16)
#define PMUC_REG_PU_DONE (1 << 17)
#define PMUC_MEM_RET_MODE (1 << 18)
#define PMUC_EFUSE_READ_DISABLE (1 << 19)
#define PMUC_BUA_DET_NORMAL_FUNC_ENABLE (1 << 20)
#define PMUC_BUA_DET_LATCH (1 << 21)
#define PMUC_PM2_SKIP_WAIT_XTAL_BEFORE_DBB (1 << 22)

// power_mode_ctrl_1
#define PMUC_PM2_PD_RAM(n) (((n)&0x7fff) << 0)

// delay_time_ctrl
#define PMUC_XTAL_RDY_TIME(n) (((n)&0x7f) << 0)
#define PMUC_DBB_RST_RLS_TIME(n) (((n)&0x7f) << 7)
#define PMUC_ADIE_PU_TIME(n) (((n)&0xff) << 14)
#define PMUC_VRF_PD_TIME(n) (((n)&0xf) << 22)

// pmu_power_dr_ctrl
#define PMUC_REG_CHIP_SLEEP (1 << 0)
#define PMUC_CHIP_SLEEP_DR (1 << 1)
#define PMUC_REG_PMU_PU_DONE (1 << 2)
#define PMUC_PMU_PU_DONE_DR (1 << 3)
#define PMUC_REG_PMU_PU_READY (1 << 4)
#define PMUC_PMU_PU_READY_DR (1 << 5)
#define PMUC_PMU_POWER_DR_CTRL_REG_PU_XTAL26M (1 << 6)
#define PMUC_PU_XTAL26M_DR (1 << 7)
#define PMUC_REG_XTAL26M_LP_MODE_EN (1 << 8)
#define PMUC_XTAL26M_LP_MODE_DR (1 << 9)
#define PMUC_PU_BB_TOP_DR (1 << 10)
#define PMUC_PMU_POWER_DR_CTRL_REG_PU_BB_TOP (1 << 11)
#define PMUC_ISO_BB_TOP_DR (1 << 12)
#define PMUC_REG_ISO_BB_TOP (1 << 13)
#define PMUC_RESETB_BB_TOP_DR (1 << 14)
#define PMUC_REG_RESETB_BB_TOP (1 << 15)
#define PMUC_PMU_POWER_DR_CTRL_REG_PU_RF_ANA (1 << 16)
#define PMUC_PU_RF_ANA_DR (1 << 17)
#define PMUC_REG_RESETB_DBB (1 << 18)
#define PMUC_RESETB_DBB_DR (1 << 19)
#define PMUC_REG_ISO_LPTOP (1 << 20)
#define PMUC_ISO_LPTOP_DR (1 << 21)
#define PMUC_REG_PU_DBB (1 << 22)
#define PMUC_PU_DBB_DR (1 << 23)
#define PMUC_RESETB_EFS_DR (1 << 24)
#define PMUC_REG_RESETB_EFS (1 << 25)
#define PMUC_PCLK_SW_DR (1 << 26)
#define PMUC_PSRAM_IO_RET (1 << 27)
#define PMUC_SIM_IO_RET(n) (((n)&0x3) << 28)

// mem_power_ctrl_0
#define PMUC_REG_RAM_VCE_PU(n) (((n)&0x7fff) << 0)
#define PMUC_RAM_VCE_PU_DR(n) (((n)&0x7fff) << 15)

// mem_power_ctrl_1
#define PMUC_REG_RAM_PGEN(n) (((n)&0x7fff) << 0)
#define PMUC_RAM_PGEN_DR(n) (((n)&0x7fff) << 15)

// mem_power_ctrl_2
#define PMUC_REG_RAM_RET1N(n) (((n)&0x7fff) << 0)
#define PMUC_RAM_RET_DR(n) (((n)&0x7fff) << 15)

// mem_power_ctrl_3
#define PMUC_REG_RAM_RET2N(n) (((n)&0x7fff) << 0)

// dcxo_ctrl_0
#define PMUC_XTAL_CLK1_EN (1 << 0)
#define PMUC_XTAL_CLK2_EN (1 << 1)
#define PMUC_XTAL_CLK3_EN (1 << 2)
#define PMUC_XTAL_CLK4_EN (1 << 3)
#define PMUC_XTAL_CLK5_EN (1 << 4)
#define PMUC_XTAL_CLK6_EN (1 << 5)
#define PMUC_XTAL_CLK7_EN (1 << 6)
#define PMUC_XTAL_CLK8_EN (1 << 7)
#define PMUC_XTAL_CLK9_EN (1 << 8)
#define PMUC_XTAL_CLK10_EN (1 << 9)
#define PMUC_XTAL_CLK11_EN (1 << 10)
#define PMUC_DCXO_CTRL_0_XTAL_CAPBANK(n) (((n)&0xff) << 11)
#define PMUC_DCXO_CTRL_0_XTAL_FIX_IBIT(n) (((n)&0x1f) << 19)
#define PMUC_DCXO_CTRL_0_XTAL_VAMP_IBIT(n) (((n)&0xf) << 24)
#define PMUC_XTAL_REG_BIT(n) (((n)&0xf) << 28)

// dcxo_ctrl_1
#define PMUC_XDRV_DRV_EN(n) (((n)&0xf) << 0)
#define PMUC_XDRV_PU_GPS (1 << 4)
#define PMUC_XDRV_REG_BIT(n) (((n)&0xf) << 5)
#define PMUC_XDRV_PU (1 << 9)
#define PMUC_XTAL_RSVD(n) (((n)&0xf) << 10)
#define PMUC_XTAL_CK26M_EN (1 << 14)
#define PMUC_XTAL_CK6M5_EN (1 << 15)
#define PMUC_DCXO_CTRL_1_XTAL_RDA_HLSEL(n) (((n)&0x3) << 16)
#define PMUC_DCXO_CTRL_1_XTAL_DIN(n) (((n)&0x3fff) << 18)

// dcxo_lp_ctrl_0
#define PMUC_DCXO_LP_CTRL_0_XTAL_DIN(n) (((n)&0x3fff) << 0)

// dcxo_lp_ctrl_1
#define PMUC_DCXO_LP_CTRL_1_XTAL_RDA_HLSEL(n) (((n)&0x3) << 0)
#define PMUC_DCXO_LP_CTRL_1_XTAL_CAPBANK(n) (((n)&0xff) << 2)
#define PMUC_DCXO_LP_CTRL_1_XTAL_FIX_IBIT(n) (((n)&0x1f) << 10)
#define PMUC_DCXO_LP_CTRL_1_XTAL_VAMP_IBIT(n) (((n)&0xf) << 15)

// pad_ctrl
#define PMUC_ADI_SCL_PD (1 << 0)
#define PMUC_ADI_SCL_PU(n) (((n)&0x3) << 1)
#define PMUC_ADI_SDA_SE (1 << 3)
#define PMUC_ADI_SDA_IE (1 << 4)
#define PMUC_ADI_SDA_PD (1 << 5)
#define PMUC_ADI_SDA_PU(n) (((n)&0x3) << 6)
#define PMUC_SLEEP_WPD (1 << 8)
#define PMUC_SLEEP_WPU(n) (((n)&0x3) << 9)
#define PMUC_CLK_26M_WPD (1 << 11)
#define PMUC_CLK_26M_WPU(n) (((n)&0x3) << 12)
#define PMUC_IBIT_ADIE_IF(n) (((n)&0x3) << 14)

// clk_26m_pmic_cfg
#define PMUC_DIV_DENOM(n) (((n)&0x3fff) << 0)
#define PMUC_DIV_NUM(n) (((n)&0x3ff) << 14)
#define PMUC_DIV_UPDATE (1 << 24)
#define PMUC_SRC_SEL (1 << 25)

// boundary_test_ctrl
#define PMUC_ENABLE (1 << 0)
#define PMUC_CLK_26M_PMIC (1 << 1)
#define PMUC_BUA_DET (1 << 2)
#define PMUC_ANA_INT (1 << 3)
#define PMUC_RESETB_EXT (1 << 4)
#define PMUC_CLK_32K_EXT (1 << 5)

// pmu_rsvd_reg_0
#define PMUC_DATA(n) (((n)&0xffffffff) << 0)

// pmu_rsvd_reg_1
#define PMUC_DATA(n) (((n)&0xffffffff) << 0)

// pmu_rsvd_reg_2
#define PMUC_DATA(n) (((n)&0xffffffff) << 0)

// pmu_rsvd_reg_3
#define PMUC_DATA(n) (((n)&0xffffffff) << 0)

// pmu_rsvd_reg_4
#define PMUC_DATA(n) (((n)&0xffffffff) << 0)

// pmu_rsvd_reg_5
#define PMUC_DATA(n) (((n)&0xffffffff) << 0)

// pmu_rsvd_reg_6
#define PMUC_DATA(n) (((n)&0xffffffff) << 0)

// pmu_rsvd_reg_7
#define PMUC_DATA(n) (((n)&0xffffffff) << 0)

// pmu_rsvd_reg_8
#define PMUC_DATA(n) (((n)&0xffffffff) << 0)

// rsvd_ports
#define PMUC_OUT(n) (((n)&0xff) << 0)
#define PMUC_IN(n) (((n)&0xff) << 8)

// otp_reg
#define PMUC_OTP_PROGRAMMED (1 << 0)
#define PMUC_FORCE_ENABLE_SWD (1 << 1)
#define PMUC_FORCE_ENABLE_IDBG (1 << 2)
#define PMUC_FORCE_ENABLE_NIDBG (1 << 3)
#define PMUC_FORCE_ENABLE_SIDBG (1 << 4)
#define PMUC_FORCE_ENABLE_SNIDBG (1 << 5)
#define PMUC_FORCE_ENABLE_DBGHOST (1 << 6)
#define PMUC_OTP_REG_RSVD(n) (((n)&0x3f) << 7)

// clk_xtal52m_div_cfg
#define PMUC_DENOM(n) (((n)&0x3fff) << 0)
#define PMUC_NUM(n) (((n)&0x3ff) << 14)
#define PMUC_UPDATE (1 << 24)

#endif // _PMUC_H_
