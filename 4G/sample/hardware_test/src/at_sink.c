#include "app_main.h"
#include "uart_init.h"
#include "at.h"


int AtSink(char* data, uint32 dataLen)
{

    int ret = SUCCESS;
    uint32 sink_type;
    uint32 sink_level;
    //static uint32 uA_num = 1;
    char *psink_type;
    char *psink_level;
    char *pend;
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;

    if (*data == '?')
    {
        ATPrintf("+SINK: (0-6),(0-64)\r\n");
        return SUCCESS;
    }

    psink_type = strtok(data,",");
    psink_level = strtok(NULL, ",");
    pend = strtok(NULL, ",");
    //²ÎÊýÅÐ¶Ï
    if((NULL == psink_type) || (NULL == psink_level) || (NULL != pend))
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }
    sink_type = atoi(psink_type);
    sink_level = atoi(psink_level);
    if((sink_type < ST_CURRENT_LED_R) || (sink_type > ST_VOLTAGE_VIB))
    {
        LogPrintf("sink type param error\r\n");
        return FAIL;
    }

    
    if(ST_VOLTAGE_VIB == sink_type)
    {
        if((sink_level < 0) || (sink_level > 8))
        {
            LogPrintf("sink 0-8  param error\r\n");
            return FAIL;
        }
    }
    else
    {
        if((sink_level < 0) || (sink_level > 64))
        {
            LogPrintf("sink 0-64  param error\r\n");
            return FAIL;
        }
    }
    

    /*sink_level = uA_num;
    uA_num++;*/
    LogPrintf("sink type: %d, level: %d\r\n", sink_type, sink_level);
    
    ret = KING_SinkConfig(sink_type,sink_level);
    if (FAIL == ret)
    {
        LogPrintf("KING_SinkConfig() errcode=0x%x\r\n", KING_GetLastErrCode());
        return ret;
    }
    return SUCCESS;
    
}

