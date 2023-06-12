/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingGpio.h"
#include "app_main.h"


/*******************************************************************************
 ** MACROS
 ******************************************************************************/
#define LogPrintf(fmt, args...)    do { KING_SysLog("gpio: "fmt, ##args); } while(0)
#define GPIO_7         7
#define GPIO_8         8
#define GPIO_10        10
#define GPIO_13        13

#define ADC_CHANNEL_0  2
#define ADC_CHANNEL_1  3


/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
static MSG_HANDLE msgHandle = NULL;
enum Msg_Evt
{
    MSG_EVT_BASE = 0,
    MSG_EVT_GPIO_INT,
    MSG_EVT_END,
};

/*******************************************************************************
 ** Variables
 ******************************************************************************/

/*******************************************************************************
 ** External Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Local Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Functions
 ******************************************************************************/

/**
 *     GPIO int callback
 *  Note: 中断回调函数里不可做过于耗时的操作。
 *  即使是LogPrintf()这种串口打印函数也不要使用。
 *  正确的方法是在Callback里面直接发送一个Event或Message
 *   的方式，在Callback外去处理中断事件。
 **/
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

void Gpio_Test(void)
{
    int ret = 0;
    uint32 value = 0;
    GPIO_CFG_S cfg;
    MSG_S msg;
    uint32 gpio;
    uint32 gpio_int;
    /*
     * GPIO Set/Get test.
     */
    gpio = GPIO_7;
    LogPrintf("GPIO_test begin %d\r\n", gpio);
    KING_Sleep(50);

    ret = KING_GpioInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_GpioInit() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    memset(&cfg, 0x00, sizeof(GPIO_CFG_S));
    cfg.default_val = 0;
    cfg.int_type = GPIO_INT_DISABLE;
    cfg.pull_mode = GPIO_PULL_UP;
    cfg.dir = GPIO_DIR_OUTPUT;

    ret = KING_GpioModeSet(gpio, &cfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_GpioModeSet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    ret = KING_GpioGet(gpio, &value);
    if (FAIL == ret)
    {
        LogPrintf("KING_GpioGet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    LogPrintf("Gpio[%d] Get value=%d\r\n", gpio, value);

    

    LogPrintf("Gpio[%d] Set value=1\r\n", gpio);
    ret = KING_GpioSet(gpio,1);
    if (FAIL == ret)
    {
        LogPrintf("KING_GpioSet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    ret = KING_GpioGet(gpio, &value);
    if (FAIL == ret)
    {
        LogPrintf("KING_GpioGet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("Gpio[%d] Get value=%d\r\n", gpio, value);
    KING_Sleep(1000);
    /*
     * ADC TEST.
     */
    ADC_CFG_S adcCfg;
    adcCfg.vdd_ref = ADC_VDD_REF_2;
    //adcCfg.resolution = 40;
    ret = KING_AdcConfig(ADC_CHANNEL_1, &adcCfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_AdcConfig() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    
    ret = KING_AdcGet(ADC_CHANNEL_1, &value);
    if (FAIL == ret)
    {
        LogPrintf("KING_AdcGet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("ADC[%d] Get value=%d\r\n", ADC_CHANNEL_1, value);

    
    /*
     * GPIO 中断.
     */

    gpio_int = GPIO_10;
    LogPrintf("GPIO[%d] Init with INT callback.\r\n", gpio_int);
    memset(&cfg, 0x00, sizeof(GPIO_CFG_S));
    cfg.default_val = 0;
    cfg.int_type = GPIO_INT_EDGE_BOTH;
    cfg.pull_mode = GPIO_PULL_UP;
    cfg.dir = GPIO_DIR_INPUT;
    cfg.int_callback = gpioIntCallback;
    ret = KING_GpioModeSet(gpio_int, &cfg);
    if (FAIL == ret)
    {
        LogPrintf("KING_GpioModeSet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    //msg
    ret = KING_MsgCreate(&msgHandle);
    if(FAIL ==ret)
    {
        LogPrintf("KING_MsgCreate() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    while(1)
    {
        KING_MsgRcv(&msg, msgHandle, WAIT_OPT_INFINITE);
        switch(msg.messageID)
        {
            case MSG_EVT_GPIO_INT:
            {
                LogPrintf("GPIO[%d] gpioIntCallback is called!\r\n", gpio_int);
                break;
            }
            default:
            {
                LogPrintf("msg read no know\r\n");
                break;
            }
        }
    }
}


