#include "app_main.h"
#include "uart_init.h"
#include "at.h"

#define GPIO_7      7
#define GPIO_8      8
#define GPIO_10     10
#define GPIO_13     13

static void gpioIntCallback(void)
{
    MSG_S msg;
    if (NULL != msgHandle)
    {
        memset(&msg,0x00,sizeof(MSG_S));
        msg.messageID = MSG_EVT_GPIO_INT;
        KING_MsgSend(&msg, msgHandle);
    }
}

int AtGpioCfgGet(char* data, uint32 dataLen)
{
    int ret = SUCCESS;
    uint32 gpio;
    GPIO_CFG_S gpio_cfg;
    
    char *pgpio;
    char *pend;
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;

    pgpio = strtok(data,",");
    pend = strtok(NULL, ",");
    
    //参数判断
    if((NULL == pgpio) || (NULL != pend))
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }
    memset(&gpio_cfg, 0x00, sizeof(GPIO_CFG_S));   
    gpio = atoi(pgpio);
    
    ret = KING_GpioModeGet(gpio, &gpio_cfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_GpioModeGet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    ATPrintf("GPIO%d CFG GET,%d,%d,%d\r\n", 
        gpio,gpio_cfg.int_type,gpio_cfg.pull_mode,gpio_cfg.dir);
    LogPrintf("gpio get:%d,int_type:%d,pull_mode:%d,dir:%d.\r\n",
        gpio,gpio_cfg.int_type,gpio_cfg.pull_mode,gpio_cfg.dir);
    return SUCCESS;
}


int AtGpioCfgSet(char* data, uint32 dataLen)
{
    int ret = SUCCESS;
    uint32 gpio;
    GPIO_CFG_S gpio_cfg;
    
    char *pgpio;
    char *pint_type;
    char *ppull_mode;
    char *pdir;
    char *pend;
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;

    pgpio = strtok(data,",");
    pint_type = strtok(NULL, ",");//触发方式
    ppull_mode = strtok(NULL,",");//01上下拉
    pdir = strtok(NULL, ",");//01//输入输出
    pend = strtok(NULL, ",");
    
    //参数判断
    if((NULL == pgpio) || (NULL == pint_type) || (NULL == ppull_mode) || 
       (NULL == pdir) || (NULL != pend))
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }
    memset(&gpio_cfg, 0x00, sizeof(GPIO_CFG_S));   
    gpio = atoi(pgpio);
    gpio_cfg.int_type = atoi(pint_type);
    gpio_cfg.pull_mode = atoi(ppull_mode);
    gpio_cfg.dir = atoi(pdir);
    
    if((gpio_cfg.int_type < GPIO_INT_DISABLE) || (gpio_cfg.int_type  > GPIO_INT_EDGE_FALLING))
    {
        LogPrintf("int type param error\r\n");
        return FAIL;
    }
    if((gpio_cfg.pull_mode < GPIO_PULL_UP) || (gpio_cfg.pull_mode  > GPIO_PULL_NONE))
    {
        LogPrintf("pull mode param error\r\n");
        return FAIL;
    }
    if((gpio_cfg.dir < GPIO_DIR_INPUT) || (gpio_cfg.dir  > GPIO_DIR_OUTPUT_OPENDRAIN))
    {
        LogPrintf("dir param error\r\n");
        return FAIL;
    }

    if(GPIO_INT_DISABLE != gpio_cfg.int_type)
    {
        gpio_cfg.int_callback = gpioIntCallback;
    }
    gpio_cfg.default_val = 0;
    LogPrintf("gpio set:%d,int_type:%d,pull_mode:%d,dir:%d.\r\n", 
        gpio,gpio_cfg.int_type,gpio_cfg.pull_mode,gpio_cfg.dir);

    ret = KING_GpioModeSet(gpio, &gpio_cfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_GpioModeSet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    return SUCCESS;
}

/**
 *    GPIO 设置
 *    
 *    @param[in]  data  GPIO(7,8,10,13),高/低电平(0/1)
 *    @param[in]  dataLen 
 *    @return 0 SUCCESS  -1 FAIL
 */

int AtGpioSet(char* data, uint32 dataLen)
{
    int ret = 0;
    char* pgpio = NULL;
    char* pvalue = NULL;
    char* pend = NULL;
    uint32 gpio;
    uint32 gpio_value;

    if((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    
    //参数处理
    data++;

    if (*data == '?')
    {
        ATPrintf("+GPIOSET: (0-n),(0-1)\r\n");
        return SUCCESS;
    }

    pgpio = strtok(data,",");
    pvalue = strtok(NULL, ",");
    pend = strtok(NULL, ",");
    if((NULL == pgpio) || (NULL == pvalue) || (NULL != pend))
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }

    //转化为整型
    gpio = atoi(pgpio);
    gpio_value = atoi(pvalue);
    if((1 != gpio_value) && (0 != gpio_value))
    {

        LogPrintf("GPIO set 0/1\r\n");
        return FAIL;
    }

    //gpio set
    LogPrintf("Gpio[%d] Set = %d\r\n", gpio,gpio_value);
    ret = KING_GpioSet(gpio,gpio_value);
    if (FAIL == ret)
    {
       LogPrintf("KING_GpioSet() errcode=0x%x\r\n", KING_GetLastErrCode());
       return FAIL;
    }
    return SUCCESS;

}
/**
 *    GPIO 获取
 *    
 *    @param[in]  data  GPIO(7,8,10,13),高/低电平(0/1)
 *    @param[in]  dataLen 
 *    @return 0 SUCCESS  -1 FAIL
 */

int AtGpioGet(char* data, uint32 dataLen)
{
    int ret = 0;
    char* pgpio = NULL;
    char* pend = NULL;
    uint32 gpio;
    uint32 gpio_value = 0;
    
    if((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    
    //参数处理
    data++;
    
    if (*data == '?')
    {
        ATPrintf("+GPIOGET: (0-n)\r\n");
        return SUCCESS;
    }
    
    pgpio = strtok(data,",");
    pend = strtok(NULL, ",");
    if((NULL == pgpio) || (NULL != pend))
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }

    //转化为整型
    gpio = atoi(pgpio);

    //gpio get
    ret = KING_GpioGet(gpio, &gpio_value);
    if (FAIL == ret)
    {
        LogPrintf("KING_GpioGet() errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    ATPrintf("GPIO%d Get %d\r\n", gpio, gpio_value);
    LogPrintf("GPIO%d Get %d\r\n", gpio, gpio_value);
    return SUCCESS;

}


