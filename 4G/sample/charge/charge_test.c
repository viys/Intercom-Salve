/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "app_main.h"
#include "Kingcharge.h"
#include "kingcstd.h"
#include "kingplat.h"
#include "kingrtos.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("charge: "fmt, ##args); } while(0)

/*******************************************************************************
 **  Function 
 ******************************************************************************/
static void charge_stateGet(void)
{
    int ret = -1;
    CHG_STATE_S chgState;
    
    memset(&chgState, 0x00 , sizeof(CHG_STATE_S));
    ret = KING_ChargeStateGet(&chgState);
    if (FAIL == ret)
    {
        LogPrintf("KING_ChargeStateGet fail%x\r\n",KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("bChargerConnected=%d, bCharging=%d, chargeAdapt=%d, chargeMode=%d",
                chgState.bChargerConnected, chgState.bCharging,
                chgState.chargeAdapt, chgState.chargeMode);
    }

}

static void charge_configGet(void)
{
    int ret = -1;
    CHG_CONFIG_S chgCfg;
    
    memset(&chgCfg, 0x00 , sizeof(CHG_CONFIG_S));
    ret = KING_ChargeConfigGet(&chgCfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_ChargeConfigGet fail%x\r\n",KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("ChargingCurrent=%d, ChargingEndTemperature=%d, ChargingEndVoltage=%d, ReChargingVoltage=%d",
                chgCfg.ChargingCurrent, chgCfg.ChargingEndTemperature,
                chgCfg.ChargingEndVoltage, chgCfg.ReChargingVoltage);
    }
}

static void charge_configSet(void)
{
    int ret = -1;
    CHG_CONFIG_S chgCfg;
    
    memset(&chgCfg, 0x00 , sizeof(CHG_CONFIG_S));
    chgCfg.ChargingEndVoltage = 200;
    chgCfg.ChargingCurrent = 400;
    chgCfg.ChargingEndTemperature = 40;
    chgCfg.ReChargingVoltage = 3600;
    
    ret = KING_ChargeConfigSet(&chgCfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_ChargeConfigSet fail%x\r\n",KING_GetLastErrCode());
    }
    
    memset(&chgCfg, 0x00 , sizeof(CHG_CONFIG_S));
    chgCfg.ChargingEndVoltage = 4300;
    chgCfg.ChargingCurrent = 500;
    chgCfg.ChargingEndTemperature = 200;
    chgCfg.ReChargingVoltage = 3700;
    ret = KING_ChargeConfigSet(&chgCfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_ChargeConfigSet fail%x\r\n",KING_GetLastErrCode());
    }
    charge_configGet();
}

static void charge_switch(uint8 isOnOff)
{
    int ret = -1;
    
    ret = KING_ChargeSwitch(isOnOff);
    if (FAIL == ret)
    {
        LogPrintf("KING_ChargeSwitch fail%x\r\n", KING_GetLastErrCode());
        return;
    }
}

static void charge_EventCb(uint32 eventID, void* pData, uint32 len)
{
    LogPrintf("charge_EventCb eventID %u\r\n", eventID);
    switch(eventID)
    {
        case CHG_CAP_IND:
            break;

        case CHG_CHARGE_START_IND:
        case CHG_CHARGE_END_IND:
        case CHG_WARNING_IND:
        case CHG_SHUTDOWN_IND:
            break;

#ifdef BATTERY_DETECT_SUPPORT
        case CHG_BATTERY_OFF_IND:
            break;
#endif
        case CHG_CHARGE_FINISH:
            break;

        case CHG_CHARGE_DISCONNECT:
            break;

        case CHG_CHARGE_FAULT:
            break;

        default:
            LogPrintf("chagetEventCb unknown enentId");
            break;
    }
}

void charge_test(void)
{
    int ret = -1;
    uint32 voltageVal = 0, temperatureVal = 0;

    KING_RegNotifyFun(DEV_CLASS_CHARGE, 0, charge_EventCb);

    charge_stateGet();

    ret = KING_BattVoltageGet(&voltageVal);
    if (FAIL == ret)
    {
        LogPrintf("KING_BattVoltageGet fail%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_BattVoltageGet voltageVal %u\r\n", voltageVal);
    }
    
    ret = KING_BattTemperatureGet(&temperatureVal);
    if (FAIL == ret)
    {
        LogPrintf("KING_BattTemperatureGet fail%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_BattTemperatureGet temperatureVal %u\r\n", temperatureVal);
    } 

    charge_configGet();

    charge_configSet();

    charge_switch(1);

    KING_Sleep(10000);
    //charge_switch(0);
}
