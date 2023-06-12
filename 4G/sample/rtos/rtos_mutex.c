#include "app_main.h"
#include "kingrtos.h"
#include "kingplat.h"
#include "kingcstd.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("rtos_mutex: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static THREAD_HANDLE testMutexThread = NULL;
static THREAD_HANDLE testMutexThread2 = NULL;
static MUTEX_HANDLE testMutexHandle = NULL;

static int flag = 0;

static void setFlag(void)
{
    LogPrintf("setFlag befor enter Lock");
    KING_MutexLock(testMutexHandle, WAIT_OPT_INFINITE);
    LogPrintf("setFlag after enter Lock");

    flag++;
    LogPrintf("setFlag befor enter KING_Sleep");
    KING_Sleep(1000);
    LogPrintf("setFlag after enter KING_Sleep");

    LogPrintf("setFlag befor enter UnLock");
    KING_MutexUnLock(testMutexHandle);
    LogPrintf("setFlag after enter UnLock");    
}

static void RtosMutex_ThreadFunc(void *param)
{
    while(1)
    {
        LogPrintf("RtosMutex_ThreadFunc()!");
        setFlag();
        KING_Sleep(2000);

        if (flag > 5)
        {
            break;
        }
    }
    if (testMutexHandle != NULL)
    {
        KING_MutexDelete(testMutexHandle);
        testMutexHandle = NULL;
    }
    if (testMutexThread != NULL)
    {
        testMutexThread = NULL; 
    }
    KING_ThreadExit();
}

static void RtosMutex_Thread2Func(void *param)
{
    while(1)
    {
        LogPrintf("RtosMutex_Thread2Func()!");
        KING_Sleep(200);
        setFlag();
        KING_Sleep(500);

        if (flag > 5)
        {
            break;
        }
    }
    if (testMutexHandle != NULL)
    {
        KING_MutexDelete(testMutexHandle);
        testMutexHandle = NULL;
    }
    if (testMutexThread2 != NULL)
    {
        testMutexThread2 = NULL; 
    }
    KING_ThreadExit();

}

void RtosMutex_Test(void)
{
    int ret = SUCCESS;
    THREAD_ATTR_S threadAttr;
    THREAD_ATTR_S threadAttr1;

    //Thread create 
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = RtosMutex_ThreadFunc;
    threadAttr.priority = THREAD_PRIORIT1;
    threadAttr.stackSize = 1024;
    ret = KING_ThreadCreate("mutex_Test", &threadAttr, &testMutexThread);
    if(ret != 0)
    {
        LogPrintf("KING_ThreadCreate() Fail!");
    }  

    //Thread create 
    memset(&threadAttr1, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr1.fun = RtosMutex_Thread2Func;
    threadAttr1.priority = THREAD_PRIORIT1;
    threadAttr1.stackSize = 1024;
    ret = KING_ThreadCreate("mutex_Test1", &threadAttr1, &testMutexThread2);
    if(ret != 0)
    {
        LogPrintf("KING_ThreadCreate() testThreadH Fail!");
    }
    

    ret = KING_MutexCreate("mutex_h", 0, &testMutexHandle);
    if (ret != 0 )
    {
        LogPrintf("KING_MutexCreate fail");
    }  

    if (testMutexHandle == NULL)
    {
        LogPrintf("testMutexHandle is NULL!");
    }
}

