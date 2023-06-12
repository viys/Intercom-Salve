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
#include "hwregs.h"
#include "boot_adc.h"
#include "boot_charge.h"
#include "boot_adi_bus.h"
#include "boot_platform.h"
#include "boot_pmic_intr.h"
#include "boot_efuse_pmic.h"
#include "hal_chip.h"

#define CHARGER_DEBOUNCE (10)
// 0x00: 4.2v, 0x01: 4.3v, 0x02: 4.4v, 0x03: 4.5v
#define CHGR_END_V_4200 4200
#define CHGR_END_V_4500 4500
#define VIBR_VOLTAGE_MIN 2800
static void _bootChargetPhyInit(void)
{
    REG_RDA2720M_GLOBAL_CHGR_CTRL0_T chargCtrol0;
    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_ctrl0, REG_FIELD_MASKVAL1(chargCtrol0, chgr_cc_en, 1),
        HAL_ADI_BUS_CHANGE_END);
}

static uint32_t _bootChgGetEffuseBits(int32_t bit_index)
{
    uint32_t val = 0;
    bootEfusePmicOpen();
    bootEfusePmicRead(bit_index, &val);
    bootEfusePmicClose();
    return val;
}

/*****************************************************************************/
//  Description:     This function sets the lowest switchover point between constant-current
//                   and constant-voltage modes.  default 0x10
//*****************************************************************************/
static void _bootChargerSetSwitchOverPoint(uint16_t eswitchpoint)
{
    BOOT_ASSERT(eswitchpoint <= 63, "chg: eswitchpoint invalid.");
    bootLog("chg: SetSwitchoverPoint %d", eswitchpoint);

    REG_RDA2720M_GLOBAL_CHGR_CTRL0_T chargCtrol0;
    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_ctrl0, REG_FIELD_MASKVAL1(chargCtrol0, chgr_cv_v, eswitchpoint),
        HAL_ADI_BUS_CHANGE_END);
}

static uint32_t _bootChgGetStatus(void)
{
    return halAdiBusRead(&hwp_rda2720mGlobal->chgr_status);
}

void bootChargerInit(void)
{
    REG_RDA2720M_GLOBAL_CHGR_STATUS_T chgr_status;
    uint32_t cv_v;
    bootLog("chg: bootChargerInit!");
    _bootChargetPhyInit(); //Charger control initial setup
    cv_v = _bootChgGetEffuseBits(13);
    bootLog("chg: get cv_v is 0x%x", cv_v);

    _bootChargerSetSwitchOverPoint(cv_v & 0x3f); //Set hardware cc-cv switch point hw_switch_point
    bootChargerSetEndVolt(4210);
    bootChargerSetChangerCurrent(CHARGER_CURRENT_700MA); // Set default current
    
    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_status,
        REG_FIELD_MASKVAL2(chgr_status, chgr_int_en, 1, dcp_switch_en, 1),
        HAL_ADI_BUS_CHANGE_END);
}
/*****************************************************************************/
//  Description:      This function is used to turn on the charger.
/*****************************************************************************/
void bootChargerTurnOn(void)
{
    REG_RDA2720M_GLOBAL_CHGR_CTRL0_T chargCtrol0;
    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_ctrl0, REG_FIELD_MASKVAL1(chargCtrol0, chgr_pd, 0),
        HAL_ADI_BUS_CHANGE_END);

    bootLog("chg: Turn On charger");
}

void bootChargerTurnOff(void)
{
    REG_RDA2720M_GLOBAL_CHGR_CTRL0_T chargCtrol0;
    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_ctrl0, REG_FIELD_MASKVAL1(chargCtrol0, chgr_pd, 1),
        HAL_ADI_BUS_CHANGE_END);
    bootLog("chg: Turn Off charger");
}

/*****************************************************************************/
//  Description:    Get Vbat voltage
/*****************************************************************************/
uint32_t bootChargerGetVbatVol(void)
{
    bootLog("bootChargerGetVbatVol");
    uint32_t vol;
    vol = (uint32_t)bootAdcGetChannelVolt(ADC_CHANNEL_VBATSENSE, ADC_SCALE_5V000);
    bootLog("bootChargerGetVbatVol vol:%d", vol);
    return vol;
}

/*****************************************************************************/
//  Description:    Get charge plug status
/*****************************************************************************/
bool bootChargePlugged()
{
    bool level = bootPmicEicGetLevel(BOOT_PMIC_EIC_CHGR_INT);
    return level;
 }

void bootChargerSetEndVolt(uint32_t mv)
{
    uint32_t b;
    bootLog("chg: SetChargeEndVolt %d", mv);

    REG_RDA2720M_GLOBAL_CHGR_CTRL0_T chargCtrol_0;
    BOOT_ASSERT((mv >= CHGR_END_V_4200) && (mv <= CHGR_END_V_4500), "mv error");
    b = (mv - CHGR_END_V_4200) / 100;

    b &= 3;
    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_ctrl0, REG_FIELD_MASKVAL1(chargCtrol_0, chgr_end_v, b),
        HAL_ADI_BUS_CHANGE_END);
}

void bootChargerSetChangerCurrent(uint16_t current)
{
    uint32_t temp = 0;
    REG_RDA2720M_GLOBAL_CHGR_CTRL1_T cc_reg;

    bootLog("chg: SetChgCurrent=%d", current);
    if ((current < CHARGER_CURRENT_300MA) || (current > CHARGER_CURRENT_MAX) || (current % 50))
    {
        BOOT_ASSERT(0, "chg: current invlid.");
    }

    switch (current)
    {
    case CHARGER_CURRENT_300MA:
        temp = 0;
        break;
    case CHARGER_CURRENT_350MA:
        temp = 1;
        break;
    case CHARGER_CURRENT_400MA:
        temp = 2;
        break;
    case CHARGER_CURRENT_450MA:
        temp = 3;
        break;
    case CHARGER_CURRENT_500MA:
        temp = 4;
        break;
    case CHARGER_CURRENT_550MA:
        temp = 5;
        break;
    case CHARGER_CURRENT_600MA:
        temp = 6;
        break;
    case CHARGER_CURRENT_650MA:
        temp = 7;
        break;
    case CHARGER_CURRENT_700MA:
        temp = 8;
        break;
    case CHARGER_CURRENT_750MA:
        temp = 9;
        break;
    case CHARGER_CURRENT_800MA:
        temp = 10;
        break;
    case CHARGER_CURRENT_900MA:
        temp = 11;
        break;
    case CHARGER_CURRENT_1000MA:
        temp = 12;
        break;
    case CHARGER_CURRENT_1100MA:
        temp = 13;
        break;
    case CHARGER_CURRENT_1200MA:
        temp = 14;
        break;
    case CHARGER_CURRENT_MAX:
        temp = 15;
        break;
    default:
        BOOT_ASSERT(0, "chg: current errro");
    }

    halAdiBusBatchChange(
        &hwp_rda2720mGlobal->chgr_ctrl1, REG_FIELD_MASKVAL1(cc_reg, chgr_cc_i, temp),
        HAL_ADI_BUS_CHANGE_END);
}

bool bootChargerGetCVStatus(void)
{
    REG_RDA2720M_GLOBAL_CHGR_STATUS_T chg_status;
    chg_status = (REG_RDA2720M_GLOBAL_CHGR_STATUS_T)_bootChgGetStatus();

    return chg_status.b.chgr_cv_status & 0x1;
}

void bootSinkVIBRCfg(bool level)
{
    uint32_t step = 0xFFFFFFFF;
    if (0 == level)
    {
        halPmuSwitchPower(HAL_POWER_VIBR, false, false);
    }
    else
    {
        step = VIBR_VOLTAGE_MIN + (level - 1) * 100;
        halPmuSetPowerLevel(HAL_POWER_VIBR, step);            
        halPmuSwitchPower(HAL_POWER_VIBR, true, false);
    }
}

