#ifndef __KING_CHARGE_H__
#define __KING_CHARGE_H__

typedef struct _tagChgConfig
{
    //充电电压门限(mv),大于该值停止充电 -- UIS8910DM平台，范围4200--4500
    uint16 ChargingEndVoltage;
    //充电电流(ma) -- UIS8910DM平台，范围300--1300
    uint16 ChargingCurrent;
    //充电温度门限(℃* 10),大与该值停止充电，范围0-450,即0-45.0℃
    uint16  ChargingEndTemperature;
    //恢复充电电压门限(mv),电压低于该值恢复充电 -- UIS8910平台，范围3650--4500,与电池相关，默认4150
    uint16 ReChargingVoltage;
}CHG_CONFIG_S;

typedef enum _tagChgMode
{
    CHG_MODE_NONE,
    CHG_MODE_CC,
    CHG_MODE_CV,
    CHG_MODE_PRECHG,
    CHG_MODE_UNKNOWN,
    CHG_MODE_MAX = 0x7ffffff,
}CHG_MODE_E;

typedef enum _tagChgAdapter
{
    CHG_ADP_UNKNOW,
    CHG_ADP_STANDARD,
    CHG_ADP_NONSTANDARD,
    CHG_ADP_USB,
    CHG_ADP_MAX = 0x7ffffff,
} CHG_ADAPT_E;

typedef struct _tagChgState
{
    uint32 bCharging: 1;
    uint32 bChargerConnected: 1;
    CHG_MODE_E chargeMode;
    CHG_ADAPT_E chargeAdapt;
}CHG_STATE_S;

typedef enum
{
    // Charge message.
    CHG_CAP_IND = 0x1, // Notify the battery's capacity
    CHG_CHARGE_START_IND, // start the charge process.
    CHG_CHARGE_END_IND,   // the charge ended.
    CHG_WARNING_IND,  // the capacity is low, should charge.
    CHG_SHUTDOWN_IND, // the capacity is very low and must shutdown.
    CHG_BATTERY_OFF_IND, // the battery is off
    CHG_CHARGE_FINISH,     // the charge has been completed.
    CHG_CHARGE_DISCONNECT, // the charge be disconnect
    CHG_CHARGE_FAULT,      // the charge fault, maybe the voltage of charge is too low.
    CHG_EVENT_MAX = 0x7FFFFFFF,
} CHG_EVENT_E;

/**
 * 充电开关
 * 
 * @param[in]  on  true-开启 false-关闭
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ChargeSwitch(bool on);

/**
 * 充电参数配置
 * 
 * @param[in]  pCfg 充电参数
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ChargeConfigSet(CHG_CONFIG_S* pCfg);

/**
 * 充电参数读取
 * 
 * @param[in]  pCfg 充电参数
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ChargeConfigGet(CHG_CONFIG_S* pCfg);

/**
 * 获取充电状态
 * 
 * @param[in]  pState 当前充电状态信息
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ChargeStateGet(CHG_STATE_S* pState);

/**
 * 获取当前电池电压
 * 
 * @param[in]  nValue 当前电池电压(mv)
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BattVoltageGet(uint32* nValue);

/**
 * 获取当前电池温度
 * 
 * @param[in]  nTemp 当前电池温度,℃ * 10,如351 = 35.1℃
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BattTemperatureGet(uint32* nTemp);

#endif

