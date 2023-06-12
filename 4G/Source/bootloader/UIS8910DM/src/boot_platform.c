/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#include "boot_app.h"
#include "boot_platform.h"
#include "hwregs.h"
#include "cmsis_core.h"
#include "boot_time.h"

#define DEBUG_EVENT_TIMEOUT_US (500)
#define PSM_WR_PROT_MAGIC (0x454e)

#define OSI_SHUTDOWN_PANIC 0x504e


void bootPanic(void)
{
    while (1)
        ;
}


void bootDebugEvent(uint32_t event)
{
    uint32_t start_time = hwp_timer4->hwtimer_curval_l;

    while (hwp_debugHost->event == 0)
    {
        uint32_t elapsed = hwp_timer4->hwtimer_curval_l - start_time;
        if (elapsed > US2TICK(DEBUG_EVENT_TIMEOUT_US))
            return;
    }

    hwp_debugHost->event = event;
}

void bootReset(bootResetMode_t mode)
{
    hwp_sysCtrl->reg_dbg = SYS_CTRL_PROTECT_UNLOCK;

    REG_SYS_CTRL_RESET_CAUSE_T reset_cause = {};
    if (mode == BOOT_RESET_FORCE_DOWNLOAD)
    {
        reset_cause.v = hwp_sysCtrl->reset_cause;
        reset_cause.b.boot_mode |= 1;
        reset_cause.b.sw_boot_mode = 0;
        hwp_sysCtrl->reset_cause = reset_cause.v;

        REG_SYS_CTRL_APCPU_RST_SET_T apcpu_rst = {};
        apcpu_rst.b.set_global_soft_rst = 1;
        hwp_sysCtrl->apcpu_rst_set = apcpu_rst.v;

        BOOT_DEAD_LOOP;
    }
    else
    {
        REG_RDA2720M_GLOBAL_SWRST_CTRL0_T swrst_ctrl0;
        swrst_ctrl0.b.reg_rst_en = 1;
        halAdiBusWrite(&hwp_rda2720mGlobal->swrst_ctrl0, swrst_ctrl0.v);

        REG_RDA2720M_GLOBAL_SOFT_RST_HW_T soft_rst_hw;
        soft_rst_hw.b.reg_soft_rst = 1;
        halAdiBusWrite(&hwp_rda2720mGlobal->soft_rst_hw, soft_rst_hw.v);
    }
}

void _psmPrepare(void)
{
    REG_RDA2720M_GLOBAL_SOFT_RST0_T soft_rst0;
    REG_RDA2720M_GLOBAL_MODULE_EN0_T module_en0;
    REG_RDA2720M_PSM_PSM_CTRL_T psm_ctrl;
    REG_RDA2720M_PSM_PSM_RC_CLK_DIV_T psm_rc_clk_div;
    REG_RDA2720M_PSM_PSM_STATUS_T psm_status;
    REG_RDA2720M_PSM_PSM_32K_CAL_TH_T psm_32k_cal_th;
    REG_RDA2720M_PSM_RTC_PWR_OFF_TH1_T rtc_pwr_off_th1;
    REG_RDA2720M_PSM_RTC_PWR_OFF_TH2_T rtc_pwr_off_th2;
    REG_RDA2720M_PSM_RTC_PWR_OFF_TH3_T rtc_pwr_off_th3;
    REG_RDA2720M_PSM_RTC_PWR_ON_TH1_T rtc_pwr_on_th1;
    REG_RDA2720M_PSM_RTC_PWR_ON_TH2_T rtc_pwr_on_th2;
    REG_RDA2720M_PSM_RTC_PWR_ON_TH3_T rtc_pwr_on_th3;

    // psm pclk enable
    REG_ADI_CHANGE1(hwp_rda2720mGlobal->module_en0, module_en0, psm_en, 1);

    // psm apb soft_reset
    REG_ADI_CHANGE1(hwp_rda2720mGlobal->soft_rst0, soft_rst0, psm_soft_rst, 1);
    bootDelayUS(10);
    REG_ADI_CHANGE1(hwp_rda2720mGlobal->soft_rst0, soft_rst0, psm_soft_rst, 0);

    // open PSM protect register
    halAdiBusWrite(&hwp_rda2720mPsm->psm_reg_wr_protect, PSM_WR_PROT_MAGIC);

    // psm module soft_reset, auto clear
    REG_ADI_WRITE1(hwp_rda2720mPsm->psm_ctrl, psm_ctrl, psm_software_reset, 1);
    bootDelayUS(10);

    // open PSM protect register
    halAdiBusWrite(&hwp_rda2720mPsm->psm_reg_wr_protect, PSM_WR_PROT_MAGIC);

    // clear psm status
    REG_ADI_WRITE1(hwp_rda2720mPsm->psm_ctrl, psm_ctrl, psm_status_clr, 1);

    // configure psm clk divider
    REG_ADI_WRITE3(hwp_rda2720mPsm->psm_rc_clk_div, psm_rc_clk_div,
                   rc_32k_cal_cnt_p, 4,
                   clk_cal_64k_div_th, 7,
                   wdg_rst_clk_sel_en, 0);

    // enable psm cnt
    REG_ADI_CHANGE1(hwp_rda2720mPsm->psm_ctrl, psm_ctrl, psm_cnt_en, 1);
    bootDelayUS(10);

    // update cnt value enable
    REG_ADI_CHANGE1(hwp_rda2720mPsm->psm_ctrl, psm_ctrl, psm_cnt_update, 1);

    // check psm update is ok
    REG_ADI_WAIT_FIELD_NEZ(psm_status, hwp_rda2720mPsm->psm_status, psm_cnt_update_vld);

    // update cnt value disable
    REG_ADI_CHANGE1(hwp_rda2720mPsm->psm_ctrl, psm_ctrl, psm_cnt_update, 0);

    // configure psm cal N and pre_cal time
    REG_ADI_WRITE2(hwp_rda2720mPsm->psm_32k_cal_th, psm_32k_cal_th,
                   rc_32k_cal_cnt_n, 7,
                   rc_32k_cal_pre_th, 2);

    // configure psm cal dn filter
    halAdiBusWrite(&hwp_rda2720mPsm->psm_26m_cal_dn_th, 0);

    // configure psm cal up filter
    halAdiBusWrite(&hwp_rda2720mPsm->psm_26m_cal_up_th, 0xffff);

    // configure rtc power off clk_en ,hold,
    REG_ADI_WRITE2(hwp_rda2720mPsm->rtc_pwr_off_th1, rtc_pwr_off_th1,
                   rtc_pwr_off_clk_en_th, 0x10,
                   rtc_pwr_off_hold_th, 0x18);

    // configure rtc power off reset, pd
    REG_ADI_WRITE2(hwp_rda2720mPsm->rtc_pwr_off_th2, rtc_pwr_off_th2,
                   rtc_pwr_off_rstn_th, 0x20,
                   rtc_pwr_off_pd_th, 0x28);

    // configure rtc power off pd done,
    REG_ADI_WRITE1(hwp_rda2720mPsm->rtc_pwr_off_th3, rtc_pwr_off_th3,
                   rtc_pwr_off_done_th, 0x40);

    // configure rtc power on pd , reset,
    REG_ADI_WRITE2(hwp_rda2720mPsm->rtc_pwr_on_th1, rtc_pwr_on_th1,
                   rtc_pwr_on_pd_th, 0x04,
                   rtc_pwr_on_rstn_th, 0x08);

    // configure rtc power on clk_en, ,hold,
    REG_ADI_WRITE2(hwp_rda2720mPsm->rtc_pwr_on_th2, rtc_pwr_on_th2,
                   rtc_pwr_on_hold_th, 0x10,
                   rtc_pwr_on_clk_en_th, 0x06);

    // configure rtc power on pd done,timeout
    REG_ADI_WRITE2(hwp_rda2720mPsm->rtc_pwr_on_th3, rtc_pwr_on_th3,
                   rtc_pwr_on_done_th, 0x40,
                   rtc_pwr_on_timeout_th, 0x48);

    // configure psm cal interval
    halAdiBusWrite(&hwp_rda2720mPsm->psm_cnt_interval_th, 6);

    // configure psm cal phase, <=psm_cnt_interval_th
    halAdiBusWrite(&hwp_rda2720mPsm->psm_cnt_interval_phase, 6);
}

void bootPowerOff(void)
{
    bool pwrkey_en = false;
    bool wakeup_en = false;

    pwrkey_en = true;
    //wakeup_en = true;

    REG_RDA2720M_PSM_PSM_CTRL_T psm_ctrl = {
        .b.psm_en = 1,
        .b.rtc_pwr_on_timeout_en = 0,
        .b.ext_int_pwr_en = wakeup_en ? 1 : 0,
        .b.pbint1_pwr_en = pwrkey_en ? 1 : 0,
        .b.pbint2_pwr_en = 0,
        .b.charger_pwr_en = 1,
        .b.psm_cnt_alarm_en = 0,
        .b.psm_cnt_alm_en = 0,
        .b.psm_software_reset = 0,
        .b.psm_cnt_update = 1,
        .b.psm_cnt_en = 1,
        .b.psm_status_clr = 0,
        .b.psm_cal_en = 1,
        .b.rtc_32k_clk_sel = 0, // 32k less
    };

    _psmPrepare();

    halAdiBusWrite(&hwp_rda2720mPsm->psm_ctrl, psm_ctrl.v);

    REG_RDA2720M_PSM_PSM_STATUS_T psm_status;
    REG_ADI_WAIT_FIELD_NEZ(psm_status, hwp_rda2720mPsm->psm_status, psm_cnt_update_vld);

    // power off
    halAdiBusWrite(&hwp_rda2720mGlobal->power_pd_hw, 1);
}

int bootPowerOnCause(void)
{
    REG_RDA2720M_GLOBAL_SWRST_CTRL0_T swrst_ctrl0;
    REG_RDA2720M_GLOBAL_CHGR_STATUS_T chgr_status;
    REG_RDA2720M_GLOBAL_POR_SRC_FLAG_T por_src_flag;

    uint32_t boot_mode_val = halAdiBusRead(&hwp_rda2720mGlobal->por_rst_monitor);
    bootLog("bootPowerOnCause:boot_mode_val=0x%x", boot_mode_val);
    if (boot_mode_val == OSI_SHUTDOWN_PANIC)
    {
        return BOOTCAUSE_PANIC;
    }

    REG_RDA2720M_WDG_WDG_INT_RAW_T wdg_int_raw;
    wdg_int_raw.v = halAdiBusRead(&hwp_rda2720mWdg->wdg_int_raw);
    if (wdg_int_raw.b.wdg_int_raw)
    {
        bootLog("bootPowerOnCause:BOOTCAUSE_WDG");
        return BOOTCAUSE_WDG;
    }

    chgr_status.v = halAdiBusRead(&hwp_rda2720mGlobal->chgr_status);
    swrst_ctrl0.v = halAdiBusRead(&hwp_rda2720mGlobal->swrst_ctrl0);
    por_src_flag.v = halAdiBusRead(&hwp_rda2720mGlobal->por_src_flag);

    bootLog("bootPowerOnCause:chgr_status=0x%x, swrst_ctrl0=0x%x, por_src_flag=0x%x", chgr_status.v, swrst_ctrl0.v, por_src_flag.v);
    if (swrst_ctrl0.b.reg_rst_en == 0)
    {
        if (por_src_flag.v & (1 << 11))
            return BOOTCAUSE_PIN_RESET;

        if (por_src_flag.v & (1 << 6))
            return BOOTCAUSE_ALARM;

        if (chgr_status.b.chgr_int && chgr_status.b.chgr_on)
        {
            return BOOTCAUSE_CHARGE;
        }

        if (por_src_flag.v & ((1 << 12) | (1 << 9) | (1 << 8)))
            return BOOTCAUSE_PWRKEY;

    }

    /*if (chgr_status.b.chgr_int && chgr_status.b.chgr_on)
    {
        return BOOTCAUSE_CHARGE;
    }*/

    return BOOTCAUSE_UNKNOWN;
}


void bootDCacheClean(const void *address, size_t size)
{
    __DMB();
    intptr_t p = (intptr_t)address;
    intptr_t end = p + size;
    p &= ~(CONFIG_CACHE_LINE_SIZE - 1);
    for (; p < end; p += CONFIG_CACHE_LINE_SIZE)
        __set_DCCMVAC(p);
    __DSB();
}

void bootDCacheCleanAll(void)
{
    L1C_CleanDCacheAll();
    __DSB();
}

void bootDCacheInvalidate(const void *address, size_t size)
{
    intptr_t p = (intptr_t)address;
    intptr_t end = p + size;
    p &= ~(CONFIG_CACHE_LINE_SIZE - 1);
    for (; p < end; p += CONFIG_CACHE_LINE_SIZE)
        __set_DCIMVAC(p);
    __DSB();
}

uint32_t BOOT_SECTION_SRAM_TEXT bootContextSave(void)
{
    uint32_t flags;
    asm volatile(
        " mrs   %0, cpsr\n"
        " cpsid aif\n"
        " dsb\n"
        " isb\n"
        : "=r"(flags)
        :
        : "memory", "cc");
    return flags;
}

void BOOT_SECTION_SRAM_TEXT bootContextRestore(uint32_t flags)
{
    asm volatile(
        " msr   cpsr_c, %0\n"
        " dsb\n"
        " isb\n"
        :
        : "r"(flags)
        : "memory", "cc");
}

BOOT_STRONG_ALIAS(bootEnterCritical, bootContextSave);
BOOT_STRONG_ALIAS(bootExitCritical, bootContextRestore);

