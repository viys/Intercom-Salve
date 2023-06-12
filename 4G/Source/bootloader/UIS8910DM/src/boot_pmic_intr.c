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

#include "boot_pmic_intr.h"
#include "boot_gpio.h"
#include "boot_adi_bus.h"
#include "boot_platform.h"
#include "hwregs.h"
#include <string.h>

typedef struct
{
    signed char bit;
} bootPmicEicConfig_t;

typedef struct
{
    signed char bit;
} bootPmicIntrConfig_t;


typedef struct
{
    bootPmicIntrConfig_t intr[BOOT_PMIC_INTR_COUNT];
    bootPmicEicConfig_t eic[BOOT_PMIC_EIC_COUNT];
} bootPmicIntrContext_t;

static bootPmicIntrContext_t gDrvPmicIntrCtx;


void bootPmicInit(void)
{
    bootPmicIntrContext_t *p = &gDrvPmicIntrCtx;
    memset(p, 0, sizeof(*p));

    for (int n = 0; n < BOOT_PMIC_INTR_COUNT; n++)
        p->intr[n].bit = n;
    for (int n = 0; n < BOOT_PMIC_EIC_COUNT; n++)
        p->eic[n].bit = n;

    REG_RDA2720M_GLOBAL_MODULE_EN0_T module_en0;
    REG_RDA2720M_GLOBAL_RTC_CLK_EN0_T rtc_clk_en0;
    REG_RDA2720M_INT_INT_EN_T int_en;
    halAdiBusBatchChange(
        // disable all interrupts
        &hwp_rda2720mInt->int_en, HAL_ADI_BUS_OVERWITE(0),
        // enable EIC
        &hwp_rda2720mGlobal->module_en0,
        REG_FIELD_MASKVAL1(module_en0, eic_en, 1),
        // enable EIC RTC clock
        &hwp_rda2720mGlobal->rtc_clk_en0,
        REG_FIELD_MASKVAL1(rtc_clk_en0, rtc_eic_en, 1),
        // disable all EIC interrupts
        &hwp_rda2720mEic->eicie, HAL_ADI_BUS_OVERWITE(0),
        // enable EIC interrupt
        &hwp_rda2720mInt->int_en,
        REG_FIELD_MASKVAL1(int_en, eic_int_en, 1),
        HAL_ADI_BUS_CHANGE_END);
}


bool bootPmicEicGetLevel(unsigned eic)
{
    if (eic >= BOOT_PMIC_EIC_COUNT)
        return false;

    bootPmicIntrContext_t *p = &gDrvPmicIntrCtx;
    bootPmicEicConfig_t *ec = &p->eic[eic];
    if (ec->bit < 0)
        return false;

    unsigned mask = 1 << ec->bit;
    halAdiBusWrite(&hwp_rda2720mEic->eicdmsk, mask);
    unsigned data = halAdiBusRead(&hwp_rda2720mEic->eicdata);
    return (data & mask) != 0;
}
