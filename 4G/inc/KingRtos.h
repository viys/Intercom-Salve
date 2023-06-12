#ifndef __KING_RTOS_H__
#define __KING_RTOS_H__

#include "KingDef.h"

// Event/Msg Wait option.
#define WAIT_OPT_NO_WAIT        0x0          // 不等待
#define WAIT_OPT_INFINITE       0xFFFFFFFF   // 永久等待

typedef void (*THREAD_ENTRY_FUN)(void* param);
typedef void (*TIMER_FUN)(uint32 tmrId);

//thread优先级定义，数值越小，优先级越高
typedef enum
{
    THREAD_PRIORIT1 = 1,  
    THREAD_PRIORIT2, 
    THREAD_PRIORIT3,  
    THREAD_PRIORIT4,
    THREAD_PRIORIT5,
    THREAD_PRIORIT6,
    THREAD_PRIORIT7,
    THREAD_PRIORIT8,
    THREAD_PRIORIT9
}THREAD_PRIORIT_E;


typedef struct
{
    uint32 stackSize;
    uint32 queueNum;
    uint32 preemption;
    THREAD_PRIORIT_E priority;
    uint32 autoStart;
    THREAD_ENTRY_FUN fun;
    void* param;
}THREAD_ATTR_S;

typedef struct
{
    uint32 messageID;
    void* pData;
    uint32 DataLen;
}MSG_S;

typedef struct
{
    TIMER_FUN timer_fun; ///< 定时器到期回调函数
    uint32 timeout; ///< 定时器倒计时间，单位ms
    bool isPeriod; ///< 是否周期性启动定时器。
}TIMER_ATTR_S;



/**
 * 创建线程
 *
 * @param[in]  pName 给定线程名字
 * @param[in]  pAttr 线程相关参数 如 堆栈、优先级、入口函数
               优先级定义见THREAD_PRIORIT_E，数值越小优先级越高
 * @param[out] pThreadID 给出线程的ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadCreate(char* pName, THREAD_ATTR_S* pAttr,THREAD_HANDLE* pThreadID);

/**
 * 退出当前线程
 * 线程退出建议使用这种方式，同时退出最好调用这个函数，避免资源泄露
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadExit(void);

/**
 * 终止指定线程
 * 不推荐使用这种方式终止线程，因很难确保被终止的线程当前没有占用锁资源等，容易产生问题
 * @param[in]  threadID 需要终止的线程ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadTerminate(THREAD_HANDLE threadID);

/**
 * 获取当前线程ID
 *
 * @param[out]  pThreadID 当前线程ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadIdCurrentGet(THREAD_HANDLE* pThreadID);

/**
 * 获取当前线程优先级
 *
 * @param[out]  pPriority 当前线程优先级
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadPriorityCurrentGet(uint32* pPriority);

/**
 * 设置线程优先级
 *
 * @param[in]  threadID 需要设定优先级的线程ID
 * @param[in]  Priority 优先级 1-255 1最高优先级
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadPrioritySet(THREAD_HANDLE threadID, uint32 Priority);

/**
 * 线程继续
 * 线程从挂起(suspend)状态重新进入ready
 * @param[in]  threadID 线程ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadResume(THREAD_HANDLE threadID);

/**
 * 线程挂起
 * 线程进入挂起(suspend)状态
 * @param[in]  threadID 线程ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadSuspend(THREAD_HANDLE threadID);

/**
 * 线程挂起一段执行时间
 * 线程进入挂起(suspend)状态，timeMs后恢复ready状态
 * @param[in]  timeMs 时间ms
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_Sleep(uint32 timeMs);

/**
 * 创建互斥锁
 *
 * @param[in]  name 互斥锁名字
 * @param[in]  inherit 优先级继承 0-不支持 1-支持
 * @param[out]  pMutex 互斥锁handle
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_MutexCreate(const char* name, uint32 inherit, MUTEX_HANDLE* pMutex);

/**
 * 获取互斥锁
 *
 * @param[in]  mutex 互斥锁handle
 * @param[in]  timeout 超时时间, 单位ms
 * @return 0 SUCCESS (成功获取到锁)  -1 FAIL
 */
int KING_MutexLock(MUTEX_HANDLE mutex, uint32 timeout);

/**
 * 释放互斥锁
 *
 * @param[in]  mutex 互斥锁handle
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_MutexUnLock(MUTEX_HANDLE mutex);

 /**
 * 删除互斥锁
 *
 * @param[in]  mutex 互斥锁handle
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_MutexDelete(MUTEX_HANDLE mutex);

 /**
  * 创建计数信号量
  *
  * @param[in]  name 信号量名字
  * @param[in]  init_count 信号量初始计数
  * @param[out] pSem 信号量handle
  * @return 0 SUCCESS  -1 FAIL
  */
int KING_SemCreate(const char* name, uint32 init_count, SEM_HANDLE* pSem);

  /**
  * 获取计数信号量
  * 成功获取后计数减一
  * @param[in]  sem 信号量handle
  * @param[in]  timeout 等待时间，单位ms
  * @return 0 SUCCESS(成功获取到信号量)  -1 FAIL
  */
int KING_SemGet(SEM_HANDLE sem, uint32 timeout);

  /**
  * 释放计数信号量
  * 成功释放后计数加一
  * @param[in]  sem 信号量handle
  * @return 0 SUCCESS  -1 FAIL
  */
int KING_SemPut(SEM_HANDLE sem);

  /**
  * 删除计数信号量
  *
  * @param[in]  sem 信号量handle
  * @return 0 SUCCESS  -1 FAIL
  */
int KING_SemDelete(SEM_HANDLE sem);

  /**
  * 创建事件
  *
  * @param[in]    name 事件名字
  * @param[out] pEvent 事件handle
  * @return 0 SUCCESS    -1 FAIL
  */
int KING_EventCreate(const char* name, EVENT_HANDLE* pEvent);

/**
* 标志事件产生
*
* @param[in] event 事件handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_EventSet(EVENT_HANDLE event);

/**
* 删除事件
*
* @param[in] event 事件handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_EventDelete(EVENT_HANDLE event);

/**
* 获取事件
* timeout时间单位为ms。有两个特殊值，分别为：
* WAIT_OPT_NO_WAIT 和 WAIT_OPT_INFINITE
*
* @param[in] event 事件handle
* @param[in] clearFlag 获取到事件后是否清除该事件 0-不清除 1-清除
* @param[in] timeout 等待时间, 单位ms
* @return 0 SUCCESS(成功获取到事件)   -1 FAIL
*/
int KING_EventGet(EVENT_HANDLE event, uint32 clearFlag, uint32 timeout);

/**
* 创建消息队列
*
* @param[out] pHandle 消息队列handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MsgCreate(MSG_HANDLE* pHandle);

/**
* 发送消息
*
* @param[in] pMsg 消息实体指针
* @param[in] handle 接收方消息队列handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MsgSend(MSG_S* pMsg, MSG_HANDLE handle);

/**
* 接收消息.
* timeout时间单位为ms。有两个特殊值，分别为：
* WAIT_OPT_NO_WAIT 和 WAIT_OPT_INFINITE
*
* @param[in] pMsg 消息实体指针
* @param[in] handle 消息队列handle
* @param[in] timeout 超时时间，单位ms
* @return 0 SUCCESS(成功获取到事件)   -1 FAIL
*/
int KING_MsgRcv(MSG_S* pMsg, MSG_HANDLE handle, uint32 timeout);

/**
* 删除消息队列
*
* @param[in] handle 消息队列handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MsgDelete(MSG_HANDLE handle);

/**
* 内存申请
* @param[in]  pMem 申请到的地址指针的指针
* @param[in]  size 要申请的内存大小
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MemAlloc(void** pMem, uint32 size);

/**
* 内存释放
* @param[in]  pMem 要释放的地址指针
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MemFree(void* pMem);


/**
* 定时器创建
* @param[out]  pTimer 创建的timer id
* @return 0 SUCCESS   -1 FAIL
*/
int KING_TimerCreate(TIMER_HANDLE* pTimer);

/**
* 定时器激活
* @param[in]  Timer 要激活的timer id
* @param[in]  pCfg 要激活timer的相关参数配置
* @return 0 SUCCESS   -1 FAIL
*/
int KING_TimerActive(TIMER_HANDLE Timer, TIMER_ATTR_S* pCfg);

/**
* 定时器去激活
* @param[in]  Timer 去激活的timer id
* @return 0 SUCCESS   -1 FAIL
*/
int KING_TimerDeactive(TIMER_HANDLE Timer);

/**
* 定时器删除
* @param[in]  Timer 要删除的timer id
* @return 0 SUCCESS   -1 FAIL
*/
int KING_TimerDelete(TIMER_HANDLE Timer);

#endif
