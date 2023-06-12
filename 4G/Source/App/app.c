#include "app.h"
#include "uart.h"

/* 定义必要全局变量 */
THREAD_ATTR_S threadAttr;
TIMER_ATTR_S  timerAttr;
uint32 writeLen = 0;

/* 线程ID */
THREAD_HANDLE AppTaskStartThreadH = NULL;
THREAD_HANDLE TH1ThreadH = NULL;
THREAD_HANDLE TH2ThreadH = NULL;
THREAD_HANDLE TH3ThreadH = NULL;
THREAD_HANDLE TH4ThreadH = NULL;

/* 定时器ID */
TIMER_HANDLE time1H = NULL;

/* 信号量 */
SEM_HANDLE sem1H = NULL;

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


/* 任务1 */
void th1(void *param)
{
    int ret = 0;
    MSG_S msg1;

    /* 创建消息队列 */
    KING_MsgCreate(&msg1H);

    while(1){
        memset(&msg1,0x00,sizeof(MSG_S));
        msg1.messageID = 0x00000001;
//        DBG_U2_Send("MSG_S size is %d\r\n",sizeof(MSG_S));
        KING_MsgSend(&msg1,msg1H);
        DBG_U2_Send("th1 running...\r\n");
//        DBG_U2_Send("th1 ID is %x\r\n",TH1ThreadH);
        KING_Sleep(1000);
    }
}

/* 任务2 */
void th2(void *param)
{
    int ret = 0;
    uint8 temp = 0;

    while(1){
        DBG_U2_Send("th2 running...\r\n");
        DBG_U2_Send("th2 ID is %x\r\n",TH2ThreadH);
        KING_ThreadSuspend(TH1ThreadH);
        temp++;
        if(temp >= 5){
            temp = 0;
            KING_ThreadResume(TH1ThreadH);
            ret = KING_SemPut(sem1H);
            LOG_P(ret,"KING_SemPut() sem1 Fail!\r\n");
        }
        KING_Sleep(2000);
    }
}

/* 任务3 */
void th3(void *param)
{
    int ret = 0;
    while(1){
        ret = KING_SemGet(sem1H,WAIT_OPT_INFINITE);
        LOG_P(ret,"KING_SemGet() sem1 Fail!\r\n");
        DBG_U2_Send("th3 running\r\n");
    }
}

void th4(void *pragma)
{
    int ret = 0;
    MSG_S msg1;

    while (1)
    {
        memset(&msg1,0x00,sizeof(MSG_S));
        ret = KING_MsgRcv(&msg1,msg1H,WAIT_OPT_INFINITE);
        LOG_P(ret,"KING_MsgRcv() msg1 Fail!\r\n");
        DBG_U2_Send("reseve: msgID is %x\r\n",msg1.messageID);
    }
    

}

void time1_th(uint32 tmrId)
{
    int ret = 0;
    while (1)
    {
        DBG_U2_Send("time1 running\r\n");
        LOG_P(ret,"KING_SemPut() sem1 Fail!\r\n");
        KING_Sleep(1000);
    }
    
    
}

void AppTaskStart(void *param)
{
    int ret = 0;

    /* 创建信号量 */
    ret = KING_SemCreate("sem1",0,&sem1H);
    LOG_P(ret,"KING_SemCreate() sem1 Fail!\r\n");

    

    /* 创建任务1 */
    ret = KING_ThreadCreate("th1",KING_Thread_Config(th1,NULL,3,1024),&TH1ThreadH);
    LOG_P(ret,"KING_ThreadCreate() th1 Fail!\r\n");

    /* 创建任务2 */
    ret = KING_ThreadCreate("th2",KING_Thread_Config(th2,NULL,3,1024),&TH2ThreadH);
    LOG_P(ret,"KING_ThreadCreate() th2 Fail!\r\n");
    /* 创建任务3 */
    ret = KING_ThreadCreate("th3",KING_Thread_Config(th3,NULL,3,1024),&TH3ThreadH);
    LOG_P(ret,"KING_ThreadCreate() th3 Fail!\r\n");
    /* 创建任务4 */
    ret = KING_ThreadCreate("th4",KING_Thread_Config(th4,NULL,3,1024),&TH4ThreadH);
    LOG_P(ret,"KING_ThreadCreate() th4 Fail!\r\n");

    /* 创建定时器 */
    ret = KING_TimerCreate(&time1H);
    LOG_P(ret,"KING_TimerCreate() time1 Fail!\r\n");
    ret = KING_TimerActive(time1H,KING_Time_Config(time1_th,1000,TRUE));
    LOG_P(ret,"KING_TimerActive time1 Fail!\r\n");
    

    KING_ThreadExit();
}


