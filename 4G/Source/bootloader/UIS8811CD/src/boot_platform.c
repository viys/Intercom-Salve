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

#include "boot_adi_bus.h"
#include "boot_platform.h"
#include "hwregs.h"
#include "cmsis_core.h"

void bootReset(bootResetMode_t mode)
{
    if (mode == BOOT_RESET_FORCE_DOWNLOAD)
    {
        REG_SYS_CTRL_RESET_CAUSE_T reset_cause = {hwp_sysCtrl->reset_cause};
        reset_cause.b.boot_mode = 4; // force download
        reset_cause.b.sw_boot_mode = 0;
        hwp_sysCtrl->reset_cause = reset_cause.v;

        REG_SYS_CTRL_SYS_RST_SET0_T sys_rst_set0 = {.b.set_rst_global = 1};
        hwp_sysCtrl->sys_rst_set0 = sys_rst_set0.v;
    }
    else
    {
        REG_SYS_CTRL_RESET_CAUSE_T reset_cause = {hwp_sysCtrl->reset_cause};
        reset_cause.b.boot_mode = 0;
        reset_cause.b.sw_boot_mode = 0;
        hwp_sysCtrl->reset_cause = reset_cause.v;

        REG_SYS_CTRL_SYS_RST_SET0_T sys_rst_set0 = {.b.set_rst_global = 1};
        hwp_sysCtrl->sys_rst_set0 = sys_rst_set0.v;
    }
}

void bootPowerOff(void)
{
    REG_PMUC_POWER_MODE_CTRL_0_T pmuc_mode_ctrl0 = {hwp_pwrCtrl->power_mode_ctrl_0};
    pmuc_mode_ctrl0.b.power_mode = 3;
    hwp_pwrCtrl->power_mode_ctrl_0 = pmuc_mode_ctrl0.v;

    REG_PMIC_PMUC_POWER_MODE_CTRL_0_T pmic_mode_ctrl0;
    bootAdiBusChange(&hwp_pmicPmuc->power_mode_ctrl_0,
                    REG_FIELD_MASKVAL1(pmic_mode_ctrl0, pm_reg, 3));

    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __DSB();
    __ISB();
    __WFI();
}


void bootPanic(void)
{
    while (1)
        ;
}

uint32_t BOOT_SECTION_SRAM_TEXT bootContextSave(void)
{
    uint32_t flags;
    asm volatile("MRS   %0, primask \n"
                 "CPSID i           \n"
                 "DSB               \n"
                 "ISB               \n"
                 : "=r"(flags)
                 :
                 : "memory", "cc");
    return flags;
}

void BOOT_SECTION_SRAM_TEXT bootContextRestore(uint32_t flags)
{
    asm volatile("MSR   primask, %0 \n"
                 "DSB               \n"
                 "ISB               \n"
                 :
                 : "r"(flags)
                 : "memory", "cc");
}

BOOT_STRONG_ALIAS(bootEnterCritical, bootContextSave);
BOOT_STRONG_ALIAS(bootExitCritical, bootContextRestore);

