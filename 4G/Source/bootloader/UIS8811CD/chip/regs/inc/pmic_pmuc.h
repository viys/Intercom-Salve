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

#ifndef _PMIC_PMUC_H_
#define _PMIC_PMUC_H_

// Auto generated (v1.0-43-g14cf228). Don't edit it manually!

#define REG_PMIC_PMUC_BASE (0x41a48600)

typedef volatile struct
{
    uint32_t clock_select;           // 0x00000000
    uint32_t clock_32k_div_cfg;      // 0x00000004
    uint32_t ip_clk_disable_ctrl;    // 0x00000008
    uint32_t ip_soft_rst_ctrl;       // 0x0000000c
    uint32_t wakeup_mask;            // 0x00000010
    uint32_t wakeup_clr;             // 0x00000014
    uint32_t wakeup_status;          // 0x00000018
    uint32_t int_clr;                // 0x0000001c
    uint32_t power_mode_ctrl_0;      // 0x00000020
    uint32_t power_mode_ctrl_1;      // 0x00000024
    uint32_t power_mode_ctrl_2;      // 0x00000028
    uint32_t power_mode_status_0;    // 0x0000002c
    uint32_t power_mode_status_1;    // 0x00000030
    uint32_t power_mode_status_2;    // 0x00000034
    uint32_t direct_ctrl_0;          // 0x00000038
    uint32_t direct_ctrl_1;          // 0x0000003c
    uint32_t direct_ctrl_2;          // 0x00000040
    uint32_t vcore_vosel_ctrl_0;     // 0x00000044
    uint32_t vcore_vosel_ctrl_1;     // 0x00000048
    uint32_t efs_rd_data_0;          // 0x0000004c
    uint32_t efs_rd_data_1;          // 0x00000050
    uint32_t efs_rd_data_2;          // 0x00000054
    uint32_t efs_rd_data_3;          // 0x00000058
    uint32_t trim_ctrl_0;            // 0x0000005c
    uint32_t trim_ctrl_1;            // 0x00000060
    uint32_t trim_ctrl_2;            // 0x00000064
    uint32_t trim_ctrl_3;            // 0x00000068
    uint32_t ana_clk32k_ctrl;        // 0x0000006c
    uint32_t ana_rtc_ctrl_0;         // 0x00000070
    uint32_t ana_rtc_ctrl_1;         // 0x00000074
    uint32_t ana_powerdet_ctrl;      // 0x00000078
    uint32_t ana_ldo_vbat_ctrl_0;    // 0x0000007c
    uint32_t ana_ldo_vbat_ctrl_1;    // 0x00000080
    uint32_t ana_ldo_vext_ctrl_0;    // 0x00000084
    uint32_t ana_ldo_vext_ctrl_1;    // 0x00000088
    uint32_t ana_dcdc_ctrl_0;        // 0x0000008c
    uint32_t ana_dcdc_ctrl_1;        // 0x00000090
    uint32_t chgr_ctrl;              // 0x00000094
    uint32_t boundary_test_ctrl;     // 0x00000098
    uint32_t timer_clk_div_cfg;      // 0x0000009c
    uint32_t pad_ctrl_0;             // 0x000000a0
    uint32_t pad_ctrl_1;             // 0x000000a4
    uint32_t pad_ctrl_2;             // 0x000000a8
    uint32_t pwrkey_dbnc_time_cfg;   // 0x000000ac
    uint32_t pin_rst_dbnc_time_cfg;  // 0x000000b0
    uint32_t chgr_int_dbnc_time_cfg; // 0x000000b4
    uint32_t otp_dbnc_time_cfg;      // 0x000000b8
    uint32_t ovlo_dbnc_time_cfg;     // 0x000000bc
    uint32_t uvlo_dbnc_time_cfg;     // 0x000000c0
    uint32_t smpl_cfg;               // 0x000000c4
    uint32_t abnormal_occur_status;  // 0x000000c8
    uint32_t abnormal_occur_clr;     // 0x000000cc
    uint32_t bonding_option;         // 0x000000d0
    uint32_t rsvd_reg_0;             // 0x000000d4
    uint32_t rsvd_reg_1;             // 0x000000d8
    uint32_t rsvd_reg_2;             // 0x000000dc
    uint32_t rsvd_reg_3;             // 0x000000e0
    uint32_t rsvd_reg_4;             // 0x000000e4
    uint32_t rsvd_reg_5;             // 0x000000e8
    uint32_t rsvd_reg_6;             // 0x000000ec
    uint32_t rsvd_reg_7;             // 0x000000f0
    uint32_t vcore_vosel_ctrl_2;     // 0x000000f4
    uint32_t vcore_vosel_ctrl_3;     // 0x000000f8
    uint32_t abnormal_rst_time_cfg;  // 0x000000fc
    uint32_t rsvd_ports;             // 0x00000100
} HWP_PMIC_PMUC_T;

#define hwp_pmicPmuc ((HWP_PMIC_PMUC_T *)REG_ACCESS_ADDRESS(REG_PMIC_PMUC_BASE))

// clock_select
typedef union {
    uint32_t v;
    struct
    {
        uint32_t sel_32k_src : 1;     // [0]
        uint32_t sel_clk_wdt_src : 2; // [2:1]
        uint32_t __3_3 : 1;           // [3]
        uint32_t sel_rc32k_div : 2;   // [5:4]
        uint32_t __31_6 : 26;         // [31:6]
    } b;
} REG_PMIC_PMUC_CLOCK_SELECT_T;

// clock_32k_div_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t update : 1;       // [0], write clear
        uint32_t denom : 6;        // [6:1]
        uint32_t wakeup_denom : 6; // [12:7]
        uint32_t __31_13 : 19;     // [31:13]
    } b;
} REG_PMIC_PMUC_CLOCK_32K_DIV_CFG_T;

// ip_clk_disable_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efs_clk_disable : 1;   // [0]
        uint32_t wdt_clk_disable : 1;   // [1]
        uint32_t timer_clk_disable : 1; // [2]
        uint32_t gpt_clk_disable : 1;   // [3]
        uint32_t gpio_clk_disable : 1;  // [4]
        uint32_t iomux_clk_disable : 1; // [5]
        uint32_t __31_6 : 26;           // [31:6]
    } b;
} REG_PMIC_PMUC_IP_CLK_DISABLE_CTRL_T;

// ip_soft_rst_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t efs_soft_rst : 1;      // [0]
        uint32_t wdt_soft_rst : 1;      // [1]
        uint32_t timer_soft_rst : 1;    // [2]
        uint32_t gpt_soft_rst : 1;      // [3]
        uint32_t gpio_soft_rst : 1;     // [4]
        uint32_t iomux_soft_rst : 1;    // [5]
        uint32_t pmuc_reg_soft_rst : 1; // [6]
        uint32_t __31_7 : 25;           // [31:7]
    } b;
} REG_PMIC_PMUC_IP_SOFT_RST_CTRL_T;

// wakeup_mask
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bypass_wdt_wakeup : 1;    // [0]
        uint32_t bypass_timer_wakeup : 1;  // [1]
        uint32_t bypass_gpt_wakeup : 1;    // [2]
        uint32_t bypass_gpio_wakeup : 1;   // [3]
        uint32_t bypass_chg_on_wakeup : 1; // [4]
        uint32_t bypass_pwrkey_wakeup : 1; // [5]
        uint32_t bypass_wdt_reset : 1;     // [6]
        uint32_t bypass_otp_reset : 1;     // [7]
        uint32_t bypass_ovlo_reset : 1;    // [8]
        uint32_t bypass_uvlo_reset : 1;    // [9]
        uint32_t bypass_vbatlow_reset : 1; // [10]
        uint32_t bypass_pin_rst : 1;       // [11]
        uint32_t __31_12 : 20;             // [31:12]
    } b;
} REG_PMIC_PMUC_WAKEUP_MASK_T;

// wakeup_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wakeup_status_clr : 1; // [0], write clear
        uint32_t __31_1 : 31;           // [31:1]
    } b;
} REG_PMIC_PMUC_WAKEUP_CLR_T;

// wakeup_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wdt_wakeup_status : 1;    // [0], read only
        uint32_t timer_wakeup_status : 1;  // [1], read only
        uint32_t gpt_wakeup_status : 1;    // [2], read only
        uint32_t gpio_wakeup_status : 1;   // [3], read only
        uint32_t chg_on_wakeup_status : 1; // [4], read only
        uint32_t pwrkey_wakeup_status : 1; // [5], read only
        uint32_t wdt_rst_pwron_status : 1; // [6], read only
        uint32_t pin_rst_pwron_status : 1; // [7], read only
        uint32_t smpl_pwron_status : 1;    // [8], read only
        uint32_t __31_9 : 23;              // [31:9]
    } b;
} REG_PMIC_PMUC_WAKEUP_STATUS_T;

// int_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t chg_on_int_clr : 1;  // [0], write clear
        uint32_t chg_off_int_clr : 1; // [1], write clear
        uint32_t pwrkey_int_clr : 1;  // [2], write clear
        uint32_t __31_3 : 29;         // [31:3]
    } b;
} REG_PMIC_PMUC_INT_CLR_T;

// power_mode_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_reg : 2;                 // [1:0]
        uint32_t vcore_ret_wrap_val_sel : 4; // [5:2]
        uint32_t __6_6 : 1;                  // [6]
        uint32_t vbat_det_delay_time : 8;    // [14:7]
        uint32_t __31_15 : 17;               // [31:15]
    } b;
} REG_PMIC_PMUC_POWER_MODE_CTRL_0_T;

// power_mode_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm0_ldo_lp18_mode : 2;  // [1:0]
        uint32_t pm0_ldo_vio33_mode : 2; // [3:2]
        uint32_t pm0_ldo_vio18_mode : 2; // [5:4]
        uint32_t pm0_ldo_dcxo_mode : 2;  // [7:6]
        uint32_t pm2_ldo_lp18_mode : 1;  // [8]
        uint32_t pm2_ldo_vio33_mode : 1; // [9]
        uint32_t pm2_ldo_vio18_mode : 1; // [10]
        uint32_t pm2_ldo_dcxo_mode : 1;  // [11]
        uint32_t pm3_ldo_lp18_mode : 1;  // [12]
        uint32_t pm3_ldo_vio33_mode : 1; // [13]
        uint32_t pm3_ldo_vio18_mode : 1; // [14]
        uint32_t pm3_ldo_dcxo_mode : 1;  // [15]
        uint32_t __31_16 : 16;           // [31:16]
    } b;
} REG_PMIC_PMUC_POWER_MODE_CTRL_1_T;

// power_mode_ctrl_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_emm_mode : 1; // [0]
        uint32_t pm0_vrf_mode : 1; // [1]
        uint32_t pm2_vrf_mode : 1; // [2]
        uint32_t pm3_vrf_mode : 1; // [3]
        uint32_t __31_4 : 28;      // [31:4]
    } b;
} REG_PMIC_PMUC_POWER_MODE_CTRL_2_T;

// power_mode_status_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t first_pwron_state : 5; // [4:0], read only
        uint32_t pm02_sw_state : 5;     // [9:5], read only
        uint32_t pm03_sw_state : 5;     // [14:10], read only
        uint32_t __31_15 : 17;          // [31:15]
    } b;
} REG_PMIC_PMUC_POWER_MODE_STATUS_0_T;

// power_mode_status_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pm_state : 3;         // [2:0], read only
        uint32_t pu_done : 1;          // [3], read only
        uint32_t pd_dig_top : 1;       // [4], read only
        uint32_t iso_aon : 1;          // [5], read only
        uint32_t resetb_dig_top : 1;   // [6], read only
        uint32_t resetb_efs : 1;       // [7], read only
        uint32_t ext_resetb : 1;       // [8], read only
        uint32_t ldo_lp18_pd : 1;      // [9], read only
        uint32_t ldo_lp18_ulp_en : 1;  // [10], read only
        uint32_t ldo_vio18_pd : 1;     // [11], read only
        uint32_t ldo_vio18_lp_en : 1;  // [12], read only
        uint32_t ldo_vio33_pd : 1;     // [13], read only
        uint32_t ldo_vio33_ulp_en : 1; // [14], read only
        uint32_t __31_15 : 17;         // [31:15]
    } b;
} REG_PMIC_PMUC_POWER_MODE_STATUS_1_T;

// power_mode_status_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bg_pd : 1;          // [0], read only
        uint32_t osc3m_en : 1;       // [1], read only
        uint32_t power_det_en : 1;   // [2], read only
        uint32_t vrf_pd : 1;         // [3], read only
        uint32_t vcore_pd : 1;       // [4], read only
        uint32_t vcore_ulp_en : 1;   // [5], read only
        uint32_t ldo_sim0_pd : 1;    // [6], read only
        uint32_t ldo_sim0_lp_en : 1; // [7], read only
        uint32_t ldo_sim1_pd : 1;    // [8], read only
        uint32_t ldo_sim1_lp_en : 1; // [9], read only
        uint32_t ldo_emm_pd : 1;     // [10], read only
        uint32_t ldo_vibr_lp_en : 1; // [11], read only
        uint32_t ldo_dcxo_pd : 1;    // [12], read only
        uint32_t ldo_dcxo_lp_en : 1; // [13], read only
        uint32_t psm_vref_pd : 1;    // [14], read only
        uint32_t dvdd_iso : 1;       // [15], read only
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_PMIC_PMUC_POWER_MODE_STATUS_2_T;

// direct_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_lp18_dr : 1;      // [0]
        uint32_t ldo_lp18_pd : 1;      // [1]
        uint32_t ldo_lp18_ulp_en : 1;  // [2]
        uint32_t ldo_dcxo_dr : 1;      // [3]
        uint32_t ldo_dcxo_lp_en : 1;   // [4]
        uint32_t ldo_dcxo_pd : 1;      // [5]
        uint32_t ldo_vio18_dr : 1;     // [6]
        uint32_t ldo_vio18_lp_en : 1;  // [7]
        uint32_t ldo_vio18_pd : 1;     // [8]
        uint32_t ldo_vio33_dr : 1;     // [9]
        uint32_t ldo_vio33_pd : 1;     // [10]
        uint32_t ldo_vio33_ulp_en : 1; // [11]
        uint32_t ldo_sim0_dr : 1;      // [12]
        uint32_t ldo_sim0_lp_en : 1;   // [13]
        uint32_t ldo_sim1_dr : 1;      // [14]
        uint32_t ldo_sim1_lp_en : 1;   // [15]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_PMIC_PMUC_DIRECT_CTRL_0_T;

// direct_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t osc3m_en_dr : 1;         // [0]
        uint32_t osc3m_en : 1;            // [1]
        uint32_t power_det_en_dr : 1;     // [2]
        uint32_t power_det_en : 1;        // [3]
        uint32_t bg_pd_dr : 1;            // [4]
        uint32_t bg_pd : 1;               // [5]
        uint32_t vcore_dr : 1;            // [6]
        uint32_t vcore_pd : 1;            // [7]
        uint32_t vcore_ulp_en : 1;        // [8]
        uint32_t vrf_dr : 1;              // [9]
        uint32_t vrf_pd : 1;              // [10]
        uint32_t ldo_vibr_dr : 1;         // [11]
        uint32_t ldo_vibr_lp_en : 1;      // [12]
        uint32_t soft_efs_read_start : 1; // [13], write clear
        uint32_t __31_14 : 18;            // [31:14]
    } b;
} REG_PMIC_PMUC_DIRECT_CTRL_1_T;

// direct_ctrl_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pd_dig_top_dr : 1;     // [0]
        uint32_t pd_dig_top : 1;        // [1]
        uint32_t iso_aon_dr : 1;        // [2]
        uint32_t iso_aon : 1;           // [3]
        uint32_t resetb_dig_top_dr : 1; // [4]
        uint32_t resetb_dig_top : 1;    // [5]
        uint32_t resetb_efs_dr : 1;     // [6]
        uint32_t resetb_efs : 1;        // [7]
        uint32_t ext_resetb_dr : 1;     // [8]
        uint32_t ext_resetb : 1;        // [9]
        uint32_t dvdd_iso_dr : 1;       // [10]
        uint32_t dvdd_iso : 1;          // [11]
        uint32_t psm_vref_pd_dr : 1;    // [12]
        uint32_t psm_vref_pd : 1;       // [13]
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_PMIC_PMUC_DIRECT_CTRL_2_T;

// vcore_vosel_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t soft_vosel_load : 1;  // [0], write clear
        uint32_t dest_vcore_vosel : 9; // [9:1]
        uint32_t step_number : 5;      // [14:10]
        uint32_t __31_15 : 17;         // [31:15]
    } b;
} REG_PMIC_PMUC_VCORE_VOSEL_CTRL_0_T;

// vcore_vosel_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t step_interval : 4;    // [3:0]
        uint32_t step_value : 9;       // [12:4]
        uint32_t volt_up : 1;          // [13]
        uint32_t soft_start_pulse : 1; // [14], write clear
        uint32_t __31_15 : 17;         // [31:15]
    } b;
} REG_PMIC_PMUC_VCORE_VOSEL_CTRL_1_T;

// efs_rd_data_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_vbat_reftrim : 5; // [4:0], read only
        uint32_t ldo_vext_reftrim : 5; // [9:5], read only
        uint32_t vio33_ulp_trim : 5;   // [14:10], read only
        uint32_t emm_pro : 1;          // [15], read only
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_PMIC_PMUC_EFS_RD_DATA_0_T;

// efs_rd_data_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtcbg_trim : 5;      // [4:0], read only
        uint32_t dcdc_osc3m_freq : 5; // [9:5], read only
        uint32_t lp18_ulp_trim : 5;   // [14:10], read only
        uint32_t __31_15 : 17;        // [31:15]
    } b;
} REG_PMIC_PMUC_EFS_RD_DATA_1_T;

// efs_rd_data_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vcore_votrim : 5; // [4:0], read only
        uint32_t vrf_votrim : 5;   // [9:5], read only
        uint32_t vpa_votrim : 5;   // [14:10], read only
        uint32_t __31_15 : 17;     // [31:15]
    } b;
} REG_PMIC_PMUC_EFS_RD_DATA_2_T;

// efs_rd_data_3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vcore_votrim_lp : 5; // [4:0], read only
        uint32_t __31_5 : 27;         // [31:5]
    } b;
} REG_PMIC_PMUC_EFS_RD_DATA_3_T;

// trim_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_vbat_reftrim : 5; // [4:0]
        uint32_t ldo_vext_reftrim : 5; // [9:5]
        uint32_t vio33_ulp_trim : 5;   // [14:10]
        uint32_t trim_dr_ctrl : 1;     // [15]
        uint32_t __31_16 : 16;         // [31:16]
    } b;
} REG_PMIC_PMUC_TRIM_CTRL_0_T;

// trim_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtcbg_trim : 5;      // [4:0]
        uint32_t dcdc_osc3m_freq : 5; // [9:5]
        uint32_t lp18_ulp_trim : 5;   // [14:10]
        uint32_t __31_15 : 17;        // [31:15]
    } b;
} REG_PMIC_PMUC_TRIM_CTRL_1_T;

// trim_ctrl_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vcore_votrim : 5; // [4:0]
        uint32_t vrf_votrim : 5;   // [9:5]
        uint32_t vpa_votrim : 5;   // [14:10]
        uint32_t __31_15 : 17;     // [31:15]
    } b;
} REG_PMIC_PMUC_TRIM_CTRL_2_T;

// trim_ctrl_3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vcore_votrim_lp : 5; // [4:0]
        uint32_t __31_5 : 27;         // [31:5]
    } b;
} REG_PMIC_PMUC_TRIM_CTRL_3_T;

// ana_clk32k_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rc32k_pu : 1;       // [0]
        uint32_t xtal32k_fine : 3;   // [3:1]
        uint32_t xtal32k_coarse : 3; // [6:4]
        uint32_t xtal32k_pu : 1;     // [7]
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_PMIC_PMUC_ANA_CLK32K_CTRL_T;

// ana_rtc_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vbatbk_vosel : 3; // [2:0]
        uint32_t rtc_vosel : 3;    // [5:3]
        uint32_t __31_6 : 26;      // [31:6]
    } b;
} REG_PMIC_PMUC_ANA_RTC_CTRL_0_T;

// ana_rtc_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rtc_rsvd0 : 8; // [7:0]
        uint32_t rtc_rsvd1 : 8; // [15:8]
        uint32_t __31_16 : 16;  // [31:16]
    } b;
} REG_PMIC_PMUC_ANA_RTC_CTRL_1_T;

// ana_powerdet_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pbint_pullh_enb : 1; // [0]
        uint32_t buadet_en : 1;       // [1]
        uint32_t vbat_crash_v : 2;    // [3:2]
        uint32_t uvlo_v : 2;          // [5:4]
        uint32_t ovlo_v : 2;          // [7:6]
        uint32_t ovlo_t : 2;          // [9:8]
        uint32_t ovlo_en : 1;         // [10]
        uint32_t vbatlow_en : 1;      // [11]
        uint32_t uvlo_en : 1;         // [12]
        uint32_t __31_13 : 19;        // [31:13]
    } b;
} REG_PMIC_PMUC_ANA_POWERDET_CTRL_T;

// ana_ldo_vbat_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_lp18_vosel : 6;      // [5:0]
        uint32_t ldo_lp18_lp_en : 1;      // [6]
        uint32_t ldo_dcxo_vosel : 6;      // [12:7]
        uint32_t ldo_lp18_ulp_itrim : 2;  // [14:13]
        uint32_t ldo_lp18_ulp_ifb_en : 1; // [15]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_PMIC_PMUC_ANA_LDO_VBAT_CTRL_0_T;

// ana_ldo_vbat_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_vio18_vosel : 6; // [5:0]
        uint32_t __31_6 : 26;         // [31:6]
    } b;
} REG_PMIC_PMUC_ANA_LDO_VBAT_CTRL_1_T;

// ana_ldo_vext_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_sim1_vosel : 6;       // [5:0]
        uint32_t ldo_sim1_pd : 1;          // [6]
        uint32_t ldo_sim0_vosel : 6;       // [12:7]
        uint32_t ldo_sim0_pd : 1;          // [13]
        uint32_t ldo_vio33_ulp_ifb_en : 1; // [14]
        uint32_t __31_15 : 17;             // [31:15]
    } b;
} REG_PMIC_PMUC_ANA_LDO_VEXT_CTRL_0_T;

// ana_ldo_vext_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ldo_vibr_vosel : 6;      // [5:0]
        uint32_t ldo_vibr_pd : 1;         // [6]
        uint32_t ldo_vio33_vosel : 6;     // [12:7]
        uint32_t ldo_vio33_lp_en : 1;     // [13]
        uint32_t ldo_vio33_ulp_itrim : 2; // [15:14]
        uint32_t __31_16 : 16;            // [31:16]
    } b;
} REG_PMIC_PMUC_ANA_LDO_VEXT_CTRL_1_T;

// ana_dcdc_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vcore_lp_en : 1; // [0]
        uint32_t vrf_lp_en : 1;   // [1]
        uint32_t vrf_vosel : 9;   // [10:2]
        uint32_t __31_11 : 21;    // [31:11]
    } b;
} REG_PMIC_PMUC_ANA_DCDC_CTRL_0_T;

// ana_dcdc_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vcore_vosel : 9; // [8:0], read only
        uint32_t __31_9 : 23;     // [31:9]
    } b;
} REG_PMIC_PMUC_ANA_DCDC_CTRL_1_T;

// chgr_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t chgr_pd : 1;  // [0]
        uint32_t chgr_int : 1; // [1], read only
        uint32_t __31_2 : 30;  // [31:2]
    } b;
} REG_PMIC_PMUC_CHGR_CTRL_T;

// boundary_test_ctrl
typedef union {
    uint32_t v;
    struct
    {
        uint32_t enable : 1;               // [0]
        uint32_t clk_32k_ext : 1;          // [1]
        uint32_t pmic_int : 1;             // [2]
        uint32_t bua_det : 1;              // [3]
        uint32_t chip_sleep : 1;           // [4], read only
        uint32_t clk_26m : 1;              // [5], read only
        uint32_t psm_indicator_enable : 1; // [6]
        uint32_t __31_7 : 25;              // [31:7]
    } b;
} REG_PMIC_PMUC_BOUNDARY_TEST_CTRL_T;

// timer_clk_div_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t update : 1;            // [0], write clear
        uint32_t denom : 6;             // [6:1]
        uint32_t sel_timer_32k_src : 1; // [7]
        uint32_t __31_8 : 24;           // [31:8]
    } b;
} REG_PMIC_PMUC_TIMER_CLK_DIV_CFG_T;

// pad_ctrl_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t adi_d_wpd : 1;      // [0]
        uint32_t adi_d_wpu : 1;      // [1]
        uint32_t adi_sck_wpd : 1;    // [2]
        uint32_t adi_sck_wpu : 1;    // [3]
        uint32_t clk_32k_wpd : 1;    // [4]
        uint32_t clk_32k_wpu : 1;    // [5]
        uint32_t resetb_ext_wpd : 1; // [6]
        uint32_t resetb_ext_wpu : 1; // [7]
        uint32_t bua_det_wpd : 1;    // [8]
        uint32_t bua_det_wpu : 1;    // [9]
        uint32_t pmic_int_wpd : 1;   // [10]
        uint32_t pmic_int_wpu : 1;   // [11]
        uint32_t clk_26m_wpd : 1;    // [12]
        uint32_t clk_26m_wpu : 1;    // [13]
        uint32_t sleep_wpd : 1;      // [14]
        uint32_t sleep_wpu : 1;      // [15]
        uint32_t __31_16 : 16;       // [31:16]
    } b;
} REG_PMIC_PMUC_PAD_CTRL_0_T;

// pad_ctrl_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ibit_die_if : 2; // [1:0]
        uint32_t gpio_ie : 8;     // [9:2]
        uint32_t __31_10 : 22;    // [31:10]
    } b;
} REG_PMIC_PMUC_PAD_CTRL_1_T;

// pad_ctrl_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ibit_gpio0 : 8; // [7:0]
        uint32_t ibit_gpio1 : 8; // [15:8]
        uint32_t __31_16 : 16;   // [31:16]
    } b;
} REG_PMIC_PMUC_PAD_CTRL_2_T;

// pwrkey_dbnc_time_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pwrkey_dbnc_time : 14; // [13:0]
        uint32_t __31_14 : 18;          // [31:14]
    } b;
} REG_PMIC_PMUC_PWRKEY_DBNC_TIME_CFG_T;

// pin_rst_dbnc_time_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t pin_rst_dbnc_time : 14; // [13:0]
        uint32_t __31_14 : 18;           // [31:14]
    } b;
} REG_PMIC_PMUC_PIN_RST_DBNC_TIME_CFG_T;

// chgr_int_dbnc_time_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t chgr_int_dbnc_time : 14; // [13:0]
        uint32_t __31_14 : 18;            // [31:14]
    } b;
} REG_PMIC_PMUC_CHGR_INT_DBNC_TIME_CFG_T;

// otp_dbnc_time_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t otp_dbnc_time : 12; // [11:0]
        uint32_t __31_12 : 20;       // [31:12]
    } b;
} REG_PMIC_PMUC_OTP_DBNC_TIME_CFG_T;

// ovlo_dbnc_time_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t ovlo_dbnc_time : 12; // [11:0]
        uint32_t __31_12 : 20;        // [31:12]
    } b;
} REG_PMIC_PMUC_OVLO_DBNC_TIME_CFG_T;

// uvlo_dbnc_time_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t uvlo_dbnc_time : 12; // [11:0]
        uint32_t __31_12 : 20;        // [31:12]
    } b;
} REG_PMIC_PMUC_UVLO_DBNC_TIME_CFG_T;

// smpl_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t smpl_en : 1;         // [0]
        uint32_t smpl_threshold : 12; // [12:1]
        uint32_t __31_13 : 19;        // [31:13]
    } b;
} REG_PMIC_PMUC_SMPL_CFG_T;

// abnormal_occur_status
typedef union {
    uint32_t v;
    struct
    {
        uint32_t vbatlow : 1; // [0], read only
        uint32_t uvlo : 1;    // [1], read only
        uint32_t ovlo : 1;    // [2], read only
        uint32_t otp : 1;     // [3], read only
        uint32_t wdt_rst : 1; // [4], read only
        uint32_t pin_rst : 1; // [5], read only
        uint32_t __31_6 : 26; // [31:6]
    } b;
} REG_PMIC_PMUC_ABNORMAL_OCCUR_STATUS_T;

// abnormal_occur_clr
typedef union {
    uint32_t v;
    struct
    {
        uint32_t abnormal_status_clr : 1; // [0], write clear
        uint32_t __31_1 : 31;             // [31:1]
    } b;
} REG_PMIC_PMUC_ABNORMAL_OCCUR_CLR_T;

// bonding_option
typedef union {
    uint32_t v;
    struct
    {
        uint32_t bond_opt : 1; // [0], read only
        uint32_t powerkey : 1; // [1], read only
        uint32_t __31_2 : 30;  // [31:2]
    } b;
} REG_PMIC_PMUC_BONDING_OPTION_T;

// rsvd_reg_0
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data : 16;    // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_PMIC_PMUC_RSVD_REG_0_T;

// rsvd_reg_1
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data : 16;    // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_PMIC_PMUC_RSVD_REG_1_T;

// rsvd_reg_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data : 16;    // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_PMIC_PMUC_RSVD_REG_2_T;

// rsvd_reg_3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data : 16;    // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_PMIC_PMUC_RSVD_REG_3_T;

// rsvd_reg_4
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data : 16;    // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_PMIC_PMUC_RSVD_REG_4_T;

// rsvd_reg_5
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data : 16;    // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_PMIC_PMUC_RSVD_REG_5_T;

// rsvd_reg_6
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data : 16;    // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_PMIC_PMUC_RSVD_REG_6_T;

// rsvd_reg_7
typedef union {
    uint32_t v;
    struct
    {
        uint32_t data : 16;    // [15:0]
        uint32_t __31_16 : 16; // [31:16]
    } b;
} REG_PMIC_PMUC_RSVD_REG_7_T;

// vcore_vosel_ctrl_2
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wakeup_step_interval : 4; // [3:0]
        uint32_t wakeup_step_number : 5;   // [8:4]
        uint32_t __31_9 : 23;              // [31:9]
    } b;
} REG_PMIC_PMUC_VCORE_VOSEL_CTRL_2_T;

// vcore_vosel_ctrl_3
typedef union {
    uint32_t v;
    struct
    {
        uint32_t wakeup_step_value : 9; // [8:0]
        uint32_t __31_9 : 23;           // [31:9]
    } b;
} REG_PMIC_PMUC_VCORE_VOSEL_CTRL_3_T;

// abnormal_rst_time_cfg
typedef union {
    uint32_t v;
    struct
    {
        uint32_t reset_extend_time : 11; // [10:0]
        uint32_t __31_11 : 21;           // [31:11]
    } b;
} REG_PMIC_PMUC_ABNORMAL_RST_TIME_CFG_T;

// rsvd_ports
typedef union {
    uint32_t v;
    struct
    {
        uint32_t rsvd_ports_out : 4; // [3:0]
        uint32_t rsvd_ports_in : 4;  // [7:4], read only
        uint32_t __31_8 : 24;        // [31:8]
    } b;
} REG_PMIC_PMUC_RSVD_PORTS_T;

// clock_select
#define PMIC_PMUC_SEL_32K_SRC (1 << 0)
#define PMIC_PMUC_SEL_CLK_WDT_SRC(n) (((n)&0x3) << 1)
#define PMIC_PMUC_SEL_RC32K_DIV(n) (((n)&0x3) << 4)

// clock_32k_div_cfg
#define PMIC_PMUC_UPDATE (1 << 0)
#define PMIC_PMUC_DENOM(n) (((n)&0x3f) << 1)
#define PMIC_PMUC_WAKEUP_DENOM(n) (((n)&0x3f) << 7)

// ip_clk_disable_ctrl
#define PMIC_PMUC_EFS_CLK_DISABLE (1 << 0)
#define PMIC_PMUC_WDT_CLK_DISABLE (1 << 1)
#define PMIC_PMUC_TIMER_CLK_DISABLE (1 << 2)
#define PMIC_PMUC_GPT_CLK_DISABLE (1 << 3)
#define PMIC_PMUC_GPIO_CLK_DISABLE (1 << 4)
#define PMIC_PMUC_IOMUX_CLK_DISABLE (1 << 5)

// ip_soft_rst_ctrl
#define PMIC_PMUC_EFS_SOFT_RST (1 << 0)
#define PMIC_PMUC_WDT_SOFT_RST (1 << 1)
#define PMIC_PMUC_TIMER_SOFT_RST (1 << 2)
#define PMIC_PMUC_GPT_SOFT_RST (1 << 3)
#define PMIC_PMUC_GPIO_SOFT_RST (1 << 4)
#define PMIC_PMUC_IOMUX_SOFT_RST (1 << 5)
#define PMIC_PMUC_PMUC_REG_SOFT_RST (1 << 6)

// wakeup_mask
#define PMIC_PMUC_BYPASS_WDT_WAKEUP (1 << 0)
#define PMIC_PMUC_BYPASS_TIMER_WAKEUP (1 << 1)
#define PMIC_PMUC_BYPASS_GPT_WAKEUP (1 << 2)
#define PMIC_PMUC_BYPASS_GPIO_WAKEUP (1 << 3)
#define PMIC_PMUC_BYPASS_CHG_ON_WAKEUP (1 << 4)
#define PMIC_PMUC_BYPASS_PWRKEY_WAKEUP (1 << 5)
#define PMIC_PMUC_BYPASS_WDT_RESET (1 << 6)
#define PMIC_PMUC_BYPASS_OTP_RESET (1 << 7)
#define PMIC_PMUC_BYPASS_OVLO_RESET (1 << 8)
#define PMIC_PMUC_BYPASS_UVLO_RESET (1 << 9)
#define PMIC_PMUC_BYPASS_VBATLOW_RESET (1 << 10)
#define PMIC_PMUC_BYPASS_PIN_RST (1 << 11)

// wakeup_clr
#define PMIC_PMUC_WAKEUP_STATUS_CLR (1 << 0)

// wakeup_status
#define PMIC_PMUC_WDT_WAKEUP_STATUS (1 << 0)
#define PMIC_PMUC_TIMER_WAKEUP_STATUS (1 << 1)
#define PMIC_PMUC_GPT_WAKEUP_STATUS (1 << 2)
#define PMIC_PMUC_GPIO_WAKEUP_STATUS (1 << 3)
#define PMIC_PMUC_CHG_ON_WAKEUP_STATUS (1 << 4)
#define PMIC_PMUC_PWRKEY_WAKEUP_STATUS (1 << 5)
#define PMIC_PMUC_WDT_RST_PWRON_STATUS (1 << 6)
#define PMIC_PMUC_PIN_RST_PWRON_STATUS (1 << 7)
#define PMIC_PMUC_SMPL_PWRON_STATUS (1 << 8)

// int_clr
#define PMIC_PMUC_CHG_ON_INT_CLR (1 << 0)
#define PMIC_PMUC_CHG_OFF_INT_CLR (1 << 1)
#define PMIC_PMUC_PWRKEY_INT_CLR (1 << 2)

// power_mode_ctrl_0
#define PMIC_PMUC_PM_REG(n) (((n)&0x3) << 0)
#define PMIC_PMUC_VCORE_RET_WRAP_VAL_SEL(n) (((n)&0xf) << 2)
#define PMIC_PMUC_VBAT_DET_DELAY_TIME(n) (((n)&0xff) << 7)

// power_mode_ctrl_1
#define PMIC_PMUC_PM0_LDO_LP18_MODE(n) (((n)&0x3) << 0)
#define PMIC_PMUC_PM0_LDO_VIO33_MODE(n) (((n)&0x3) << 2)
#define PMIC_PMUC_PM0_LDO_VIO18_MODE(n) (((n)&0x3) << 4)
#define PMIC_PMUC_PM0_LDO_DCXO_MODE(n) (((n)&0x3) << 6)
#define PMIC_PMUC_PM2_LDO_LP18_MODE (1 << 8)
#define PMIC_PMUC_PM2_LDO_VIO33_MODE (1 << 9)
#define PMIC_PMUC_PM2_LDO_VIO18_MODE (1 << 10)
#define PMIC_PMUC_PM2_LDO_DCXO_MODE (1 << 11)
#define PMIC_PMUC_PM3_LDO_LP18_MODE (1 << 12)
#define PMIC_PMUC_PM3_LDO_VIO33_MODE (1 << 13)
#define PMIC_PMUC_PM3_LDO_VIO18_MODE (1 << 14)
#define PMIC_PMUC_PM3_LDO_DCXO_MODE (1 << 15)

// power_mode_ctrl_2
#define PMIC_PMUC_LDO_EMM_MODE (1 << 0)
#define PMIC_PMUC_PM0_VRF_MODE (1 << 1)
#define PMIC_PMUC_PM2_VRF_MODE (1 << 2)
#define PMIC_PMUC_PM3_VRF_MODE (1 << 3)

// power_mode_status_0
#define PMIC_PMUC_FIRST_PWRON_STATE(n) (((n)&0x1f) << 0)
#define PMIC_PMUC_PM02_SW_STATE(n) (((n)&0x1f) << 5)
#define PMIC_PMUC_PM03_SW_STATE(n) (((n)&0x1f) << 10)

// power_mode_status_1
#define PMIC_PMUC_PM_STATE(n) (((n)&0x7) << 0)
#define PMIC_PMUC_PU_DONE (1 << 3)
#define PMIC_PMUC_POWER_MODE_STATUS_1_PD_DIG_TOP (1 << 4)
#define PMIC_PMUC_POWER_MODE_STATUS_1_ISO_AON (1 << 5)
#define PMIC_PMUC_POWER_MODE_STATUS_1_RESETB_DIG_TOP (1 << 6)
#define PMIC_PMUC_RESETB_EFS (1 << 7)
#define PMIC_PMUC_POWER_MODE_STATUS_1_EXT_RESETB (1 << 8)
#define PMIC_PMUC_POWER_MODE_STATUS_1_LDO_LP18_PD (1 << 9)
#define PMIC_PMUC_POWER_MODE_STATUS_1_LDO_LP18_ULP_EN (1 << 10)
#define PMIC_PMUC_POWER_MODE_STATUS_1_LDO_VIO18_PD (1 << 11)
#define PMIC_PMUC_POWER_MODE_STATUS_1_LDO_VIO18_LP_EN (1 << 12)
#define PMIC_PMUC_POWER_MODE_STATUS_1_LDO_VIO33_PD (1 << 13)
#define PMIC_PMUC_POWER_MODE_STATUS_1_LDO_VIO33_ULP_EN (1 << 14)

// power_mode_status_2
#define PMIC_PMUC_POWER_MODE_STATUS_2_BG_PD (1 << 0)
#define PMIC_PMUC_OSC3M_EN (1 << 1)
#define PMIC_PMUC_POWER_MODE_STATUS_2_POWER_DET_EN (1 << 2)
#define PMIC_PMUC_POWER_MODE_STATUS_2_VRF_PD (1 << 3)
#define PMIC_PMUC_POWER_MODE_STATUS_2_VCORE_PD (1 << 4)
#define PMIC_PMUC_POWER_MODE_STATUS_2_VCORE_ULP_EN (1 << 5)
#define PMIC_PMUC_POWER_MODE_STATUS_2_LDO_SIM0_PD (1 << 6)
#define PMIC_PMUC_POWER_MODE_STATUS_2_LDO_SIM0_LP_EN (1 << 7)
#define PMIC_PMUC_POWER_MODE_STATUS_2_LDO_SIM1_PD (1 << 8)
#define PMIC_PMUC_POWER_MODE_STATUS_2_LDO_SIM1_LP_EN (1 << 9)
#define PMIC_PMUC_LDO_EMM_PD (1 << 10)
#define PMIC_PMUC_POWER_MODE_STATUS_2_LDO_VIBR_LP_EN (1 << 11)
#define PMIC_PMUC_POWER_MODE_STATUS_2_LDO_DCXO_PD (1 << 12)
#define PMIC_PMUC_POWER_MODE_STATUS_2_LDO_DCXO_LP_EN (1 << 13)
#define PMIC_PMUC_POWER_MODE_STATUS_2_PSM_VREF_PD (1 << 14)
#define PMIC_PMUC_POWER_MODE_STATUS_2_DVDD_ISO (1 << 15)

// direct_ctrl_0
#define PMIC_PMUC_LDO_LP18_DR (1 << 0)
#define PMIC_PMUC_DIRECT_CTRL_0_LDO_LP18_PD (1 << 1)
#define PMIC_PMUC_DIRECT_CTRL_0_LDO_LP18_ULP_EN (1 << 2)
#define PMIC_PMUC_LDO_DCXO_DR (1 << 3)
#define PMIC_PMUC_DIRECT_CTRL_0_LDO_DCXO_LP_EN (1 << 4)
#define PMIC_PMUC_DIRECT_CTRL_0_LDO_DCXO_PD (1 << 5)
#define PMIC_PMUC_LDO_VIO18_DR (1 << 6)
#define PMIC_PMUC_DIRECT_CTRL_0_LDO_VIO18_LP_EN (1 << 7)
#define PMIC_PMUC_DIRECT_CTRL_0_LDO_VIO18_PD (1 << 8)
#define PMIC_PMUC_LDO_VIO33_DR (1 << 9)
#define PMIC_PMUC_DIRECT_CTRL_0_LDO_VIO33_PD (1 << 10)
#define PMIC_PMUC_DIRECT_CTRL_0_LDO_VIO33_ULP_EN (1 << 11)
#define PMIC_PMUC_LDO_SIM0_DR (1 << 12)
#define PMIC_PMUC_DIRECT_CTRL_0_LDO_SIM0_LP_EN (1 << 13)
#define PMIC_PMUC_LDO_SIM1_DR (1 << 14)
#define PMIC_PMUC_DIRECT_CTRL_0_LDO_SIM1_LP_EN (1 << 15)

// direct_ctrl_1
#define PMIC_PMUC_OSC3M_EN_DR (1 << 0)
#define PMIC_PMUC_OSC3M_EN (1 << 1)
#define PMIC_PMUC_POWER_DET_EN_DR (1 << 2)
#define PMIC_PMUC_DIRECT_CTRL_1_POWER_DET_EN (1 << 3)
#define PMIC_PMUC_BG_PD_DR (1 << 4)
#define PMIC_PMUC_DIRECT_CTRL_1_BG_PD (1 << 5)
#define PMIC_PMUC_VCORE_DR (1 << 6)
#define PMIC_PMUC_DIRECT_CTRL_1_VCORE_PD (1 << 7)
#define PMIC_PMUC_DIRECT_CTRL_1_VCORE_ULP_EN (1 << 8)
#define PMIC_PMUC_VRF_DR (1 << 9)
#define PMIC_PMUC_DIRECT_CTRL_1_VRF_PD (1 << 10)
#define PMIC_PMUC_LDO_VIBR_DR (1 << 11)
#define PMIC_PMUC_DIRECT_CTRL_1_LDO_VIBR_LP_EN (1 << 12)
#define PMIC_PMUC_SOFT_EFS_READ_START (1 << 13)

// direct_ctrl_2
#define PMIC_PMUC_PD_DIG_TOP_DR (1 << 0)
#define PMIC_PMUC_DIRECT_CTRL_2_PD_DIG_TOP (1 << 1)
#define PMIC_PMUC_ISO_AON_DR (1 << 2)
#define PMIC_PMUC_DIRECT_CTRL_2_ISO_AON (1 << 3)
#define PMIC_PMUC_RESETB_DIG_TOP_DR (1 << 4)
#define PMIC_PMUC_DIRECT_CTRL_2_RESETB_DIG_TOP (1 << 5)
#define PMIC_PMUC_RESETB_EFS_DR (1 << 6)
#define PMIC_PMUC_RESETB_EFS (1 << 7)
#define PMIC_PMUC_EXT_RESETB_DR (1 << 8)
#define PMIC_PMUC_DIRECT_CTRL_2_EXT_RESETB (1 << 9)
#define PMIC_PMUC_DVDD_ISO_DR (1 << 10)
#define PMIC_PMUC_DIRECT_CTRL_2_DVDD_ISO (1 << 11)
#define PMIC_PMUC_PSM_VREF_PD_DR (1 << 12)
#define PMIC_PMUC_DIRECT_CTRL_2_PSM_VREF_PD (1 << 13)

// vcore_vosel_ctrl_0
#define PMIC_PMUC_SOFT_VOSEL_LOAD (1 << 0)
#define PMIC_PMUC_DEST_VCORE_VOSEL(n) (((n)&0x1ff) << 1)
#define PMIC_PMUC_STEP_NUMBER(n) (((n)&0x1f) << 10)

// vcore_vosel_ctrl_1
#define PMIC_PMUC_STEP_INTERVAL(n) (((n)&0xf) << 0)
#define PMIC_PMUC_STEP_VALUE(n) (((n)&0x1ff) << 4)
#define PMIC_PMUC_VOLT_UP (1 << 13)
#define PMIC_PMUC_SOFT_START_PULSE (1 << 14)

// efs_rd_data_0
#define PMIC_PMUC_LDO_VBAT_REFTRIM(n) (((n)&0x1f) << 0)
#define PMIC_PMUC_LDO_VEXT_REFTRIM(n) (((n)&0x1f) << 5)
#define PMIC_PMUC_VIO33_ULP_TRIM(n) (((n)&0x1f) << 10)
#define PMIC_PMUC_EMM_PRO (1 << 15)

// efs_rd_data_1
#define PMIC_PMUC_RTCBG_TRIM(n) (((n)&0x1f) << 0)
#define PMIC_PMUC_DCDC_OSC3M_FREQ(n) (((n)&0x1f) << 5)
#define PMIC_PMUC_LP18_ULP_TRIM(n) (((n)&0x1f) << 10)

// efs_rd_data_2
#define PMIC_PMUC_VCORE_VOTRIM(n) (((n)&0x1f) << 0)
#define PMIC_PMUC_VRF_VOTRIM(n) (((n)&0x1f) << 5)
#define PMIC_PMUC_VPA_VOTRIM(n) (((n)&0x1f) << 10)

// efs_rd_data_3
#define PMIC_PMUC_VCORE_VOTRIM_LP(n) (((n)&0x1f) << 0)

// trim_ctrl_0
#define PMIC_PMUC_LDO_VBAT_REFTRIM(n) (((n)&0x1f) << 0)
#define PMIC_PMUC_LDO_VEXT_REFTRIM(n) (((n)&0x1f) << 5)
#define PMIC_PMUC_VIO33_ULP_TRIM(n) (((n)&0x1f) << 10)
#define PMIC_PMUC_TRIM_DR_CTRL (1 << 15)

// trim_ctrl_1
#define PMIC_PMUC_RTCBG_TRIM(n) (((n)&0x1f) << 0)
#define PMIC_PMUC_DCDC_OSC3M_FREQ(n) (((n)&0x1f) << 5)
#define PMIC_PMUC_LP18_ULP_TRIM(n) (((n)&0x1f) << 10)

// trim_ctrl_2
#define PMIC_PMUC_VCORE_VOTRIM(n) (((n)&0x1f) << 0)
#define PMIC_PMUC_VRF_VOTRIM(n) (((n)&0x1f) << 5)
#define PMIC_PMUC_VPA_VOTRIM(n) (((n)&0x1f) << 10)

// trim_ctrl_3
#define PMIC_PMUC_VCORE_VOTRIM_LP(n) (((n)&0x1f) << 0)

// ana_clk32k_ctrl
#define PMIC_PMUC_RC32K_PU (1 << 0)
#define PMIC_PMUC_XTAL32K_FINE(n) (((n)&0x7) << 1)
#define PMIC_PMUC_XTAL32K_COARSE(n) (((n)&0x7) << 4)
#define PMIC_PMUC_XTAL32K_PU (1 << 7)

// ana_rtc_ctrl_0
#define PMIC_PMUC_VBATBK_VOSEL(n) (((n)&0x7) << 0)
#define PMIC_PMUC_RTC_VOSEL(n) (((n)&0x7) << 3)

// ana_rtc_ctrl_1
#define PMIC_PMUC_RTC_RSVD0(n) (((n)&0xff) << 0)
#define PMIC_PMUC_RTC_RSVD1(n) (((n)&0xff) << 8)

// ana_powerdet_ctrl
#define PMIC_PMUC_PBINT_PULLH_ENB (1 << 0)
#define PMIC_PMUC_BUADET_EN (1 << 1)
#define PMIC_PMUC_VBAT_CRASH_V(n) (((n)&0x3) << 2)
#define PMIC_PMUC_UVLO_V(n) (((n)&0x3) << 4)
#define PMIC_PMUC_OVLO_V(n) (((n)&0x3) << 6)
#define PMIC_PMUC_OVLO_T(n) (((n)&0x3) << 8)
#define PMIC_PMUC_OVLO_EN (1 << 10)
#define PMIC_PMUC_VBATLOW_EN (1 << 11)
#define PMIC_PMUC_UVLO_EN (1 << 12)

// ana_ldo_vbat_ctrl_0
#define PMIC_PMUC_LDO_LP18_VOSEL(n) (((n)&0x3f) << 0)
#define PMIC_PMUC_LDO_LP18_LP_EN (1 << 6)
#define PMIC_PMUC_LDO_DCXO_VOSEL(n) (((n)&0x3f) << 7)
#define PMIC_PMUC_LDO_LP18_ULP_ITRIM(n) (((n)&0x3) << 13)
#define PMIC_PMUC_LDO_LP18_ULP_IFB_EN (1 << 15)

// ana_ldo_vbat_ctrl_1
#define PMIC_PMUC_LDO_VIO18_VOSEL(n) (((n)&0x3f) << 0)

// ana_ldo_vext_ctrl_0
#define PMIC_PMUC_LDO_SIM1_VOSEL(n) (((n)&0x3f) << 0)
#define PMIC_PMUC_ANA_LDO_VEXT_CTRL_0_LDO_SIM1_PD (1 << 6)
#define PMIC_PMUC_LDO_SIM0_VOSEL(n) (((n)&0x3f) << 7)
#define PMIC_PMUC_ANA_LDO_VEXT_CTRL_0_LDO_SIM0_PD (1 << 13)
#define PMIC_PMUC_LDO_VIO33_ULP_IFB_EN (1 << 14)

// ana_ldo_vext_ctrl_1
#define PMIC_PMUC_LDO_VIBR_VOSEL(n) (((n)&0x3f) << 0)
#define PMIC_PMUC_LDO_VIBR_PD (1 << 6)
#define PMIC_PMUC_LDO_VIO33_VOSEL(n) (((n)&0x3f) << 7)
#define PMIC_PMUC_LDO_VIO33_LP_EN (1 << 13)
#define PMIC_PMUC_LDO_VIO33_ULP_ITRIM(n) (((n)&0x3) << 14)

// ana_dcdc_ctrl_0
#define PMIC_PMUC_VCORE_LP_EN (1 << 0)
#define PMIC_PMUC_VRF_LP_EN (1 << 1)
#define PMIC_PMUC_VRF_VOSEL(n) (((n)&0x1ff) << 2)

// ana_dcdc_ctrl_1
#define PMIC_PMUC_VCORE_VOSEL(n) (((n)&0x1ff) << 0)

// chgr_ctrl
#define PMIC_PMUC_CHGR_PD (1 << 0)
#define PMIC_PMUC_CHGR_INT (1 << 1)

// boundary_test_ctrl
#define PMIC_PMUC_ENABLE (1 << 0)
#define PMIC_PMUC_CLK_32K_EXT (1 << 1)
#define PMIC_PMUC_PMIC_INT (1 << 2)
#define PMIC_PMUC_BUA_DET (1 << 3)
#define PMIC_PMUC_CHIP_SLEEP (1 << 4)
#define PMIC_PMUC_CLK_26M (1 << 5)
#define PMIC_PMUC_PSM_INDICATOR_ENABLE (1 << 6)

// timer_clk_div_cfg
#define PMIC_PMUC_UPDATE (1 << 0)
#define PMIC_PMUC_DENOM(n) (((n)&0x3f) << 1)
#define PMIC_PMUC_SEL_TIMER_32K_SRC (1 << 7)

// pad_ctrl_0
#define PMIC_PMUC_ADI_D_WPD (1 << 0)
#define PMIC_PMUC_ADI_D_WPU (1 << 1)
#define PMIC_PMUC_ADI_SCK_WPD (1 << 2)
#define PMIC_PMUC_ADI_SCK_WPU (1 << 3)
#define PMIC_PMUC_CLK_32K_WPD (1 << 4)
#define PMIC_PMUC_CLK_32K_WPU (1 << 5)
#define PMIC_PMUC_RESETB_EXT_WPD (1 << 6)
#define PMIC_PMUC_RESETB_EXT_WPU (1 << 7)
#define PMIC_PMUC_BUA_DET_WPD (1 << 8)
#define PMIC_PMUC_BUA_DET_WPU (1 << 9)
#define PMIC_PMUC_PMIC_INT_WPD (1 << 10)
#define PMIC_PMUC_PMIC_INT_WPU (1 << 11)
#define PMIC_PMUC_CLK_26M_WPD (1 << 12)
#define PMIC_PMUC_CLK_26M_WPU (1 << 13)
#define PMIC_PMUC_SLEEP_WPD (1 << 14)
#define PMIC_PMUC_SLEEP_WPU (1 << 15)

// pad_ctrl_1
#define PMIC_PMUC_IBIT_DIE_IF(n) (((n)&0x3) << 0)
#define PMIC_PMUC_GPIO_IE(n) (((n)&0xff) << 2)

// pad_ctrl_2
#define PMIC_PMUC_IBIT_GPIO0(n) (((n)&0xff) << 0)
#define PMIC_PMUC_IBIT_GPIO1(n) (((n)&0xff) << 8)

// pwrkey_dbnc_time_cfg
#define PMIC_PMUC_PWRKEY_DBNC_TIME(n) (((n)&0x3fff) << 0)

// pin_rst_dbnc_time_cfg
#define PMIC_PMUC_PIN_RST_DBNC_TIME(n) (((n)&0x3fff) << 0)

// chgr_int_dbnc_time_cfg
#define PMIC_PMUC_CHGR_INT_DBNC_TIME(n) (((n)&0x3fff) << 0)

// otp_dbnc_time_cfg
#define PMIC_PMUC_OTP_DBNC_TIME(n) (((n)&0xfff) << 0)

// ovlo_dbnc_time_cfg
#define PMIC_PMUC_OVLO_DBNC_TIME(n) (((n)&0xfff) << 0)

// uvlo_dbnc_time_cfg
#define PMIC_PMUC_UVLO_DBNC_TIME(n) (((n)&0xfff) << 0)

// smpl_cfg
#define PMIC_PMUC_SMPL_EN (1 << 0)
#define PMIC_PMUC_SMPL_THRESHOLD(n) (((n)&0xfff) << 1)

// abnormal_occur_status
#define PMIC_PMUC_VBATLOW (1 << 0)
#define PMIC_PMUC_UVLO (1 << 1)
#define PMIC_PMUC_OVLO (1 << 2)
#define PMIC_PMUC_OTP (1 << 3)
#define PMIC_PMUC_WDT_RST (1 << 4)
#define PMIC_PMUC_PIN_RST (1 << 5)

// abnormal_occur_clr
#define PMIC_PMUC_ABNORMAL_STATUS_CLR (1 << 0)

// bonding_option
#define PMIC_PMUC_BOND_OPT (1 << 0)
#define PMIC_PMUC_POWERKEY (1 << 1)

// rsvd_reg_0
#define PMIC_PMUC_DATA(n) (((n)&0xffff) << 0)

// rsvd_reg_1
#define PMIC_PMUC_DATA(n) (((n)&0xffff) << 0)

// rsvd_reg_2
#define PMIC_PMUC_DATA(n) (((n)&0xffff) << 0)

// rsvd_reg_3
#define PMIC_PMUC_DATA(n) (((n)&0xffff) << 0)

// rsvd_reg_4
#define PMIC_PMUC_DATA(n) (((n)&0xffff) << 0)

// rsvd_reg_5
#define PMIC_PMUC_DATA(n) (((n)&0xffff) << 0)

// rsvd_reg_6
#define PMIC_PMUC_DATA(n) (((n)&0xffff) << 0)

// rsvd_reg_7
#define PMIC_PMUC_DATA(n) (((n)&0xffff) << 0)

// vcore_vosel_ctrl_2
#define PMIC_PMUC_WAKEUP_STEP_INTERVAL(n) (((n)&0xf) << 0)
#define PMIC_PMUC_WAKEUP_STEP_NUMBER(n) (((n)&0x1f) << 4)

// vcore_vosel_ctrl_3
#define PMIC_PMUC_WAKEUP_STEP_VALUE(n) (((n)&0x1ff) << 0)

// abnormal_rst_time_cfg
#define PMIC_PMUC_RESET_EXTEND_TIME(n) (((n)&0x7ff) << 0)

// rsvd_ports
#define PMIC_PMUC_RSVD_PORTS_OUT(n) (((n)&0xf) << 0)
#define PMIC_PMUC_RSVD_PORTS_IN(n) (((n)&0xf) << 4)

#endif // _PMIC_PMUC_H_
