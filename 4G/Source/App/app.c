#include "app.h"
#include "uart.h"
#include "io.h"

/* 定义必要全局变量 */
THREAD_ATTR_S threadAttr;
TIMER_ATTR_S  timerAttr;
uint32 writeLen = 0;

/* 线程ID */
THREAD_HANDLE AppTaskStartThreadH = NULL;
THREAD_HANDLE RS485ThreadH = NULL;
THREAD_HANDLE INPUT8ThreadH = NULL;
THREAD_HANDLE INPUT10ThreadH = NULL;
THREAD_HANDLE SW1ThreadH = NULL;

/* 定时器ID */
TIMER_HANDLE time1H = NULL;

/* 信号量 */
SEM_HANDLE sem1H = NULL;

/* 事件标志位 */
EVENT_HANDLE rs485EH = NULL;
EVENT_HANDLE INPUT8EH = NULL;
EVENT_HANDLE INPUT10EH = NULL;

/* 消息队列 */
MSG_HANDLE msg1H = NULL;

/*
 * 线程简单配置函数
 * 在使用此函数时需要创建一个名为threadAttr，类型为THREAD_ATTR_S的全局变量
 */
THREAD_ATTR_S* KING_Thread_Config(THREAD_ENTRY_FUN fun,void* param,THREAD_PRIORIT_E priority,uint32 stackSize)
{
    /* 清空线程配置指针 */
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));

    /* 线程配置 */
    threadAttr.fun = fun;
    threadAttr.param = param;
    threadAttr.priority = priority;
    threadAttr.stackSize = stackSize;
    
    /* 返回线程配置指针地址 */
    return &threadAttr;
}

/*
 * 线程高级配置函数
 */
THREAD_ATTR_S* KING_Thread_ConfigA(uint32 autoStart,THREAD_ENTRY_FUN fun,void* param,uint32 preemption,
                                  THREAD_PRIORIT_E priority,uint32 queueNum,uint32 stackSize)
{
    /* 清空线程配置指针 */
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));

    /* 线程配置 */
    threadAttr.stackSize = stackSize;
    threadAttr.queueNum = queueNum;
    threadAttr.preemption = preemption;
    threadAttr.priority = priority;
    threadAttr.autoStart = autoStart;
    threadAttr.fun = fun;
    threadAttr.param = param;

    /* 返回线程配置指针地址 */
    return &threadAttr;
}

/*
 * 定时器简单配置函数
 * 在使用此函数时需要创建一个名为timerAttr，类型为TIMER_ATTR_S的全局变量
 */
TIMER_ATTR_S* KING_Time_Config(TIMER_FUN fun,uint32 timeout,bool isPeriod)
{
    /* 清空线程配置指针 */
    memset(&timerAttr, 0x00, sizeof(timerAttr));

    /* 是否周期性启动定时器 */
    timerAttr.isPeriod= isPeriod;
    /* 定时器计数周期 */
    timerAttr.timeout = timeout;
    /* 定时器回调函数 */
    timerAttr.timer_fun = fun;

    /* 返回线程配置指针地址 */
    return &timerAttr;
}

/* 定时器1 */
void time1_th(uint32 tmrId)
{
    int temp = 0;
    //int ret = 0;
    while (1)
    {
        temp = !temp;
        KING_GpioSet(GPIO_7,temp);
        KING_GpioSet(GPIO_14,temp);
        LOG_P(1,"time1 running:%d\r\n",temp);        
        KING_Sleep(1000);
        
    }
    
    
}

/* rs485数据处理线程 */
void rs485_handle(void *Param)
{
    int ret = 0;

    /* 创建标志事件 */
    ret = KING_EventCreate("485_data",&rs485EH);
    LOG_P(ret,"KING_EventCreate() rs485EH Fail\r\n");
    /* 485串口初始化,115200 */
    rs485_init();
    
    while(1){

        /* 获取并清楚标志位 */
        ret = KING_EventGet(rs485EH,1,4000);
        LOG_P(ret,"RS485 resive timeout\r\n");
        if(ret == SUCCESS){
            //KING_Sleep(100);
            rs_485_data_resive();
            rs485_input_detection();
        }
    }
}

void input8_handle(void *Param)
{
    int ret =0;
    /* 创建标志事件 */
    ret = KING_EventCreate("Input8_event",&INPUT8EH);
    LOG_P(ret,"KING_EventCreate Input_event Fail\r\n");

    input_init();

    while (1)
    {
        ret = KING_EventGet(INPUT8EH,1,4000);
        LOG_P(ret,"INPUTEH8 resive timeout\r\n");
        if (ret == SUCCESS){
            LOG_P(1,"intput8_handle is run\r\n");
        }
        KING_Sleep(1111);
        
    }
    
}

void input10_handle(void *Param)
{
    int ret =0;
    /* 创建标志事件 */
    ret = KING_EventCreate("Input10_event",&INPUT10EH);
    LOG_P(ret,"KING_EventCreate Input_event Fail\r\n");

    input10_init();
    
    while (1)
    {
        ret = KING_EventGet(INPUT10EH,1,4000);
        LOG_P(ret,"INPUTEH10 resive timeout\r\n");
        if (ret == SUCCESS){
            LOG_P(1,"intput10_handle is run\r\n");
        }
    }
    
}

void sw1_thread(void *Param)
{
    // int ret = 0;
    sw_init();

    while (1)
    {
        KING_Sleep(1100);
    }
    

}

void AppTaskStart(void *param)
{
    int ret = 0;

    /* 创建485信息处理任务 */
    ret = KING_ThreadCreate("rs485_handle",KING_Thread_Config(rs485_handle,NULL,3,5000),&RS485ThreadH);
    LOG_P(ret,"KING_ThreadCreate() th4 Fail!\r\n");
    ret = KING_ThreadCreate("input8_handle",KING_Thread_Config(input8_handle,NULL,2,5000),&INPUT8ThreadH);
    ret = KING_ThreadCreate("input10_handle",KING_Thread_Config(input10_handle,NULL,2,2000),&INPUT10ThreadH);
    ret = KING_ThreadCreate("sw1_thread",KING_Thread_Config(sw1_thread,NULL,3,1000),&SW1ThreadH);
    /* 创建定时器 */
    ret = KING_TimerCreate(&time1H);
    LOG_P(ret,"KING_TimerCreate() time1 Fail!\r\n");
    ret = KING_TimerActive(time1H,KING_Time_Config(time1_th,1000,TRUE));
    LOG_P(ret,"KING_TimerActive time1 Fail!\r\n");
    
    KING_ThreadExit();
}


