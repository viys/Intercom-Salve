#include "app_main.h"
#include "uart_init.h"
#include "device_init.h"
#include "at.h"

/**
 *    keypad�����رգ�Ĭ�Ͽ����� 
 *    
 *    @param[in]  data  AT����֮��İ����Ⱥ��Լ��������ݣ�0/1��
 *    @param[in]  dataLen ���ݳ��� 
 *    @return 0 SUCCESS  -1 FAIL
 */
int AtKeypadCmd(char* data, uint32 dataLen)
{
    static int keyflag = 1;
    int ret = SUCCESS;
    
    //�ж��Ƿ��в���
    if((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;
    dataLen--;

    if (*data == '?')
    {
        ATPrintf("+KEYPADCMD: (0-1)\r\n");
        return SUCCESS;
    }
    
    //�жϲ���
    if(('0' != *data) && ('1' != *data))
    {
        LogPrintf("0/1 param error\r\n");
        return FAIL;
    }

    //�ж��Ƿ��ظ��������߹ر�
    if(('0'  == *data) && (1 == keyflag))
    {
        
        ret = Keypad_DeInit();
        if(FAIL == ret)
        {
            return FAIL;
        }
        keyflag = 0;
    }
    else if(('1'  == *data) && (0 == keyflag))
    {
        ret = Keypad_Init();
        if(FAIL == ret)
        {
            return FAIL;
        }
        keyflag = 1;
    }
    else
    {
        LogPrintf("repetition on-off error\r\n");
        return FAIL;
    }
    
    return SUCCESS;
}

