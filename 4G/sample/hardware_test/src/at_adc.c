#include "app_main.h"
#include "uart_init.h"
#include "at.h"

#define ADC_CHANNEL_VALUE 4096

/**
 *    ADC��ȡ
 *    
 *    @param[in]  data  adcͨ����2��3��
 *    @param[in]  dataLen ���ݳ��� 
 *    @return 0 SUCCESS  -1 FAIL
 */

int AtAdcValue(char* data, uint32 dataLen)
{
    int ret = SUCCESS;
    uint32 adcValue = 0;
    uint32 adcChannel = 0;
    uint32 adc_mv = 0;

    ADC_CFG_S adcCfg;
    char *padcChannel;
    char *pvdd_ref;
    char *pend;
    uint32 vdd_ref;
    //�ж����޲���
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;
    
    if (*data == '?')
    {
        ATPrintf("+ADCVALUE: (2-3),(0-3)\r\n");
        return SUCCESS;
    }

    //�����ж�
    padcChannel = strtok(data,",");
    pvdd_ref = strtok(NULL, ",");
    pend = strtok(NULL, ",");
    if ((NULL == padcChannel) || (NULL == pvdd_ref) || (NULL != pend))
    {
     LogPrintf("param error\r\n");
     return FAIL;
    }

    //ͨ���ж�
    adcChannel = (uint32)atoi(padcChannel);
    if ((2 != adcChannel) && (3 != adcChannel))
    {
        LogPrintf("ADC Channel param error\r\n");
        return FAIL;
    }

    //�ο���ѹ�ж�
    vdd_ref = (uint32)atoi(pvdd_ref);
    if ((vdd_ref < 0) || (vdd_ref > 3))
    {
        LogPrintf("vdd reference param error\r\n");
        return FAIL;
    }

    
    //adc����
    memset(&adcCfg,0x00,sizeof(ADC_CFG_S));
    adcCfg.vdd_ref = vdd_ref;
    ret = KING_AdcConfig(adcChannel, &adcCfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_AdcConfig() errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;

    }
    KING_Sleep(20);

    
    //adc��ȡ
    ret = KING_AdcGet(adcChannel, &adcValue);
    if (FAIL == ret)
    {
        LogPrintf("KING_AdcGet() errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
       
    }
    
    switch (vdd_ref)
    {
        case ADC_VDD_REF_0:adc_mv = 1250;break;
        case ADC_VDD_REF_1:adc_mv = 2444;break;
        case ADC_VDD_REF_2:adc_mv = 3233;break;
        case ADC_VDD_REF_3:adc_mv = 5000;break;
    }
    ATPrintf("ADC%d VALUE = %d\r\n", adcChannel, adcValue);//ADCֵ
    LogPrintf("ADC[%d] value = %d\r\n", adcChannel, adcValue);//ADCֵ
    LogPrintf("reference voltage = %dmv\r\n", adc_mv);//�ο���ѹ
    LogPrintf("ADC[%d] calculate voltage value = %dmv\r\n", adcChannel, adcValue);//��ѹֵ

    return SUCCESS;
}

