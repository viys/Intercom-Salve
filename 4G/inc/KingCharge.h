#ifndef __KING_CHARGE_H__
#define __KING_CHARGE_H__

typedef struct _tagChgConfig
{
    //����ѹ����(mv),���ڸ�ֵֹͣ��� -- UIS8910DMƽ̨����Χ4200--4500
    uint16 ChargingEndVoltage;
    //������(ma) -- UIS8910DMƽ̨����Χ300--1300
    uint16 ChargingCurrent;
    //����¶�����(��* 10),�����ֵֹͣ��磬��Χ0-450,��0-45.0��
    uint16  ChargingEndTemperature;
    //�ָ�����ѹ����(mv),��ѹ���ڸ�ֵ�ָ���� -- UIS8910ƽ̨����Χ3650--4500,������أ�Ĭ��4150
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
 * ��翪��
 * 
 * @param[in]  on  true-���� false-�ر�
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ChargeSwitch(bool on);

/**
 * ����������
 * 
 * @param[in]  pCfg ������
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ChargeConfigSet(CHG_CONFIG_S* pCfg);

/**
 * ��������ȡ
 * 
 * @param[in]  pCfg ������
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ChargeConfigGet(CHG_CONFIG_S* pCfg);

/**
 * ��ȡ���״̬
 * 
 * @param[in]  pState ��ǰ���״̬��Ϣ
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ChargeStateGet(CHG_STATE_S* pState);

/**
 * ��ȡ��ǰ��ص�ѹ
 * 
 * @param[in]  nValue ��ǰ��ص�ѹ(mv)
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BattVoltageGet(uint32* nValue);

/**
 * ��ȡ��ǰ����¶�
 * 
 * @param[in]  nTemp ��ǰ����¶�,�� * 10,��351 = 35.1��
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_BattTemperatureGet(uint32* nTemp);

#endif

