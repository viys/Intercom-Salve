#include "app_main.h"
#include "uart_init.h"
#include "at.h"

/**
 *    ��翪���رգ�Ĭ�Ϲرգ�
 *    
 *    @param[in]  data  AT����֮��İ����Ⱥ��Լ��������ݣ�0/1��
 *    @param[in]  dataLen ���ݳ��� 
 *    @return 0 SUCCESS  -1 FAIL
 */
int AtChargeCmd(char* data, uint32 dataLen)
{
    static int chargeflag = 0;
    int ret = SUCCESS;
    
    //�ж��Ƿ��в���
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;
    dataLen--;

    if (*data == '?')
    {
        ATPrintf("+CHARGECMD: (0-1)\r\n");
        return SUCCESS;
    }

    //�жϲ���
    if (('0' != *data) && ('1' != *data))
    {
        LogPrintf("0/1 param error\r\n");
        return FAIL;
    }

    //�ж��Ƿ��ظ��������߹ر�
    if (('0'  == *data) && (1 == chargeflag))
    {
        
        ret = KING_ChargeSwitch(FALSE);
        if (FAIL == ret)
        {
            LogPrintf("KING_ChargeSwitch fail%x\r\n",KING_GetLastErrCode());
            return FAIL;
        }
    
        chargeflag = 0;
    }
    else if (('1'  == *data) && (0 == chargeflag))
    {
        ret = KING_ChargeSwitch(TRUE);
        if (FAIL == ret)
        {

            LogPrintf("KING_ChargeSwitch fail%x\r\n",KING_GetLastErrCode());
            return FAIL;
        }
        chargeflag = 1;
    }
    else
    {
        LogPrintf("repetition on-off error\r\n");
        return FAIL;
    }
    
    return SUCCESS;
}
/**
 *    ������� 
 *    
 *    @param[in]  data  ���ò���������ѹ���ޣ�������������¶����ޣ��ָ�����ѹ����
 *    @param[in]  dataLen ���ݳ��� 
 *    @return 0 SUCCESS  -1 FAIL
 */
int AtChargeCfg(char* data, uint32 dataLen)
{
    int ret = SUCCESS;
    CHG_CONFIG_S chgCfg;
    char *pCurrent;
    char *pEndVoltage;
    char *pEndTemperature;
    char *pVoltage;
    char *pend;
    
    //�ж��Ƿ��в���
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;
    dataLen--;

    if (*data == '?')
    {
        ATPrintf("+CHARGECFG: (4200-4500),(300-1300),(0-450),(3650-4500)\r\n");
        return SUCCESS;
    }

    //������ȡ
    pEndVoltage = strtok(data,",");
    pCurrent = strtok(NULL, ",");
    pEndTemperature = strtok(NULL, ",");
    pVoltage = strtok(NULL, ",");
    pend = strtok(NULL, ",");
    if((NULL == pCurrent) || (NULL == pEndVoltage) || (NULL == pEndTemperature) || (NULL == pVoltage) || (NULL != pend))
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }

    //���������ж�
    memset(&chgCfg, 0x00 , sizeof(CHG_CONFIG_S));
    chgCfg.ChargingEndVoltage = (uint16)atoi(pEndVoltage);
    chgCfg.ChargingCurrent = (uint16)atoi(pCurrent);
    chgCfg.ChargingEndTemperature = (uint16)atoi(pEndTemperature);
    chgCfg.ReChargingVoltage = (uint16)atoi(pVoltage);

    if ((chgCfg.ChargingEndVoltage < 4200) || (chgCfg.ChargingEndVoltage > 4500))
    {
        LogPrintf("ChargingEndVoltage 4200~4500 param error\r\n");
        return FAIL;
    }
    if ((chgCfg.ChargingCurrent < 300) || (chgCfg.ChargingCurrent > 1300))
    {
        LogPrintf("ChargingCurrent 300~1300 param error\r\n");
        return FAIL;
    }
    if ((chgCfg.ChargingEndTemperature < 0) || (chgCfg.ChargingEndTemperature > 450))
    {
        LogPrintf("ChargingEndTemperature 0~450 param error\r\n");
        return FAIL;
    }
    if ((chgCfg.ReChargingVoltage < 3650) || (chgCfg.ReChargingVoltage > 4500))
    {
        LogPrintf("ReChargingVoltage 3650~4500 param error\r\n");
        return FAIL;
    }
    LogPrintf("EndVoltagep %d\r\n", chgCfg.ChargingEndVoltage);
    LogPrintf("Current %d\r\n", chgCfg.ChargingCurrent);
    LogPrintf("EndTemperature %d\r\n", chgCfg.ChargingEndTemperature);
    LogPrintf("Voltage %d\r\n", chgCfg.ReChargingVoltage);

    //�������
    ret = KING_ChargeConfigSet(&chgCfg);
    if (FAIL == ret)
    {

        LogPrintf("KING_ChargeSwitch fail%x\r\n",KING_GetLastErrCode());
        return FAIL;
    }
    return SUCCESS;
    
}


