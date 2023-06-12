/*******************************************************************************
 ** File Name:   rtos_test.c
 ** Copyright:   Copyright 2020-2020 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: rtos api test code
 *******************************************************************************
 
 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-04-23     suzhiliang         Create.
 ******************************************************************************/

/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "app_main.h"
#include "kingrtos.h"
#include "kingplat.h"
#include "kingcstd.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("rtos: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
enum Msg_Evt
{
    MSG_EVT_BASE = 0,
    MSG_EVT_TIMER,
    MSG_EVT_END,
};

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static MSG_HANDLE msgHandle = NULL; 
static THREAD_HANDLE threadHandle = NULL;
static EVENT_HANDLE EventHandle = NULL;
static TIMER_HANDLE timerHandle = NULL;
static TIMER_HANDLE testTimerH = NULL;
static MUTEX_HANDLE testMutexH = NULL;
static SEM_HANDLE testSemH = NULL;
static THREAD_HANDLE testThreadH = NULL;

static int testTimeout = 0;

/*******************************************************************************
 ** External Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Local Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Functions
 ******************************************************************************/
static void Timer_Callback(uint32 tmrId)
{
    MSG_S msg;
    static int timerTestCount = 0;
    int ret = SUCCESS;
    timerTestCount++;
    LogPrintf("Periodic timer event: %d\r\n", timerTestCount);

    
    
    //set event to rtos thread
    ret = KING_EventSet(EventHandle);
    if(FAIL == ret)
    {
        LogPrintf("EventSet fail\r\n");
    }
    
    //send msg
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = MSG_EVT_TIMER;
    ret = KING_MsgSend(&msg, msgHandle);
    if(FAIL == ret)
    {
        LogPrintf("EventSet fail\r\n");
    }
    if (timerTestCount > 5)
    {
        //Timer Deactive
        LogPrintf("TimerCallback: Deactive test Timer!\r\n");
        KING_TimerDeactive(timerHandle);
        return;
    }
    
   
}

static void Timer_Test(void)
{
    int ret = FAIL;
    TIMER_ATTR_S timerattr;

    //timer create 
    ret = KING_TimerCreate(&timerHandle);
    if(FAIL == ret)
    {
        LogPrintf("KING_TimerCreate() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    //timer active
    memset(&timerattr, 0x00, sizeof(timerattr));
    timerattr.isPeriod= TRUE;
    timerattr.timeout = 1000;
    timerattr.timer_fun = Timer_Callback;
    ret = KING_TimerActive(timerHandle, &timerattr);
    if(FAIL == ret)
    {
        LogPrintf("KING_TimerActive() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        KING_TimerDelete(timerHandle);
        return;
    }
    LogPrintf("Create periodic timer!\r\n");
}

static void Timer_Cb(uint32 tmrId)
{
    int ret = -1;
    
    LogPrintf("Timer_Cb start\r\n");
    testTimeout = 1;
    ret = KING_ThreadResume(testThreadH);
    LogPrintf("KING_ThreadResume: ret=%d\r\n", ret);
    
    KING_SemPut(testSemH);

    LogPrintf("Timer_Cb end\r\n");
}

static void Timer_test1(void)
{
    int ret = FAIL;
    TIMER_ATTR_S timerattr;
    
    LogPrintf("Timer_test1 start\r\n");
    //timer create 
    ret = KING_TimerCreate(&testTimerH);
    if(FAIL == ret)
    {
        LogPrintf("KING_TimerCreate() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    
    
    if (testTimerH != NULL)
    {
        //timer active
        memset(&timerattr, 0x00, sizeof(timerattr));
        timerattr.isPeriod = FALSE;
        timerattr.timeout = 4000;
        timerattr.timer_fun = Timer_Cb;
        ret = KING_TimerActive(testTimerH, &timerattr);
        if(FAIL == ret)
        {
            LogPrintf("KING_TimerActive() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
            KING_TimerDelete(testTimerH);
            return;
        }

    }
    LogPrintf("KING_ThreadSuspend: start\r\n");
    KING_Sleep(1000);
    ret = KING_ThreadSuspend(testThreadH);
    LogPrintf("KING_ThreadSuspend: ret=%d\r\n", ret);
    KING_MutexLock(testMutexH, WAIT_OPT_INFINITE);
    KING_SemGet(testSemH, WAIT_OPT_INFINITE);
    if (testTimeout)
    {
        LogPrintf("testTimeout\r\n");
        testTimeout = 0;
    }
    KING_MutexUnLock(testMutexH);
    KING_SemDelete(testSemH);
    KING_MutexDelete(testMutexH);
    KING_TimerDelete(testTimerH);
}

static void Rtos_ThreadFunc(void *param)
{
    int ret = FAIL;
    uint32 priority = 0;
    THREAD_HANDLE threadID = NULL;
    int count = 0;
    
    //get current thread id get
    KING_ThreadIdCurrentGet(&threadID);
    LogPrintf("Thread Id: 0x%x\r\n",threadID);
    
    //get thread priority
    ret = KING_ThreadPriorityCurrentGet(&priority);
    if(FAIL == ret)
    {
        LogPrintf("KING_ThreadPrioritySet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("Thread Priority : %d\r\n",priority);

    //set thread priority
    ret = KING_ThreadPrioritySet(threadID, THREAD_PRIORIT2);
    if(FAIL == ret)
    {
        LogPrintf("KING_ThreadPrioritySet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    //get thread priority
    KING_ThreadPriorityCurrentGet(&priority);
    LogPrintf("Thread Priority : %d\r\n",priority);

    //event create
    ret = KING_EventCreate("Rtos Event", &EventHandle);
    if(FAIL == ret)
    {
        LogPrintf("KING_EventCreate() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        KING_ThreadExit();
    }
    
    while(1)
    {
        //wait event set
        KING_EventGet(EventHandle, 1, WAIT_OPT_INFINITE);
        count++;
        LogPrintf("Event Get %d\r\n",count);

        if(count > 5)
        {
            //event deletex
            KING_EventDelete(EventHandle);
            //timer delete
            KING_TimerDelete(timerHandle);
            LogPrintf("Thread end\r\n");
            break;
        }
    }

    //thread exit
    KING_ThreadExit();
}

static void Rtos_testThread(void *param)
{
    int count = 0;
    THREAD_HANDLE threadID = NULL;

    LogPrintf("Rtos_testThread start\r\n");

    //get current thread id get
    KING_ThreadIdCurrentGet(&threadID);
    LogPrintf("Thread Id: 0x%x\r\n",threadID);
    while(1)
    {
        LogPrintf("Rtos_testThread\r\n");
        KING_Sleep(200);
        count++;
        if (count > 50)
        {
            break;
        }
    }

    KING_ThreadTerminate(threadID);
}

void Rtos_Test(void)
{
    int ret = SUCCESS;
    THREAD_ATTR_S threadAttr;
    THREAD_ATTR_S threadAttr1;
    MSG_S msg;
    char *pData = NULL;
    
    LogPrintf("\r\n-----Rtos test start------\r\n");
    ret = KING_MemAlloc((void **)&pData, 10);
    if(FAIL == ret || pData == NULL)
    {
        LogPrintf("KING_MemAlloc Fail\r\n");
    }

    memset(pData, 0x00, 10);
    memcpy(pData, "123456789", 9);
    LogPrintf("pData = %s\r\n", pData);
    KING_MemFree(pData);
    pData = NULL;
    
    //msg
    ret = KING_MsgCreate(&msgHandle);
    if(FAIL == ret)
    {
        LogPrintf("KING_MsgCreate Fail\r\n");
    }
    
    //Thread create 
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = Rtos_ThreadFunc;
    threadAttr.priority = THREAD_PRIORIT1;
    threadAttr.stackSize = 4096;
    ret = KING_ThreadCreate("Rtos Test", &threadAttr, &threadHandle);
    if(ret != 0)
    {
        LogPrintf("KING_ThreadCreate() Fail!");
    }
    
    ret = KING_MutexCreate("test mutex", 0, &testMutexH);
    if (ret != 0 )
    {
        LogPrintf("KING_MutexCreate fail");
    }
    
    ret = KING_SemCreate("test sem", 0, &testSemH);
    if (ret != 0 )
    {
        LogPrintf("KING_SemCreate fail");
    }
    
    
    //Thread create 
    memset(&threadAttr1, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr1.fun = Rtos_testThread;
    threadAttr1.priority = THREAD_PRIORIT3;
    threadAttr1.stackSize = 1024;
    ret = KING_ThreadCreate("thread Test", &threadAttr1, &testThreadH);
    if(ret != 0)
    {
        LogPrintf("KING_ThreadCreate() testThreadH Fail!");
    }

    //Timer
    Timer_Test();
    Timer_test1();

    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg, msgHandle, WAIT_OPT_INFINITE);
        switch (msg.messageID)
        {
            case MSG_EVT_TIMER:
            {
                LogPrintf("timer to msg\r\n");
                break;
            }
            default:
            {
                LogPrintf("msg no know\r\n");
                break;
            }
        }
    }
}




