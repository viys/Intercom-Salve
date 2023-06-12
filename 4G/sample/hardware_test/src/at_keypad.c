#include "app_main.h"
#include "uart_init.h"
#include "device_init.h"
#include "at.h"

/**
 *    keypad开启关闭（默认开启） 
 *    
 *    @param[in]  data  AT命令之后的包含等号以及后面数据（0/1）
 *    @param[in]  dataLen 数据长度 
 *    @return 0 SUCCESS  -1 FAIL
 */
int AtKeypadCmd(char* data, uint32 dataLen)
{
    static int keyflag = 1;
    int ret = SUCCESS;
    
    //判断是否有参数
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
    
    //判断参数
    if(('0' != *data) && ('1' != *data))
    {
        LogPrintf("0/1 param error\r\n");
        return FAIL;
    }

    //判断是否重复开启或者关闭
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

