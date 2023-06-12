#ifndef __KING_RTOS_H__
#define __KING_RTOS_H__

#include "KingDef.h"

// Event/Msg Wait option.
#define WAIT_OPT_NO_WAIT        0x0          // ���?�
#define WAIT_OPT_INFINITE       0xFFFFFFFF   // ����?�

typedef void (*THREAD_ENTRY_FUN)(void* param);
typedef void (*TIMER_FUN)(uint32 tmrId);

//thread���?����?��???�����?�?��
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
    TIMER_FUN timer_fun; ///< ��?�����??�����
    uint32 timeout; ///< ��?������?�?��?ms
    bool isPeriod; ///< �?�������������?����
}TIMER_ATTR_S;



/**
 * �����?�
 *
 * @param[in]  pName �����?�����
 * @param[in]  pAttr �?���?��� �� ��?�����?�����?���
               ���?������THREAD_PRIORIT_E����???���?�?��
 * @param[out] pThreadID �����??�ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadCreate(char* pName, THREAD_ATTR_S* pAttr,THREAD_HANDLE* pThreadID);

/**
 * �?���?�?�
 * �?��?�����?�����?�?��??�?����������������������??�
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadExit(void);

/**
 * ��??���?�
 * ���?�?�����?�?��?�??������?������?���??�?���?������?�?����?�������
 * @param[in]  threadID ��?��?���?�ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadTerminate(THREAD_HANDLE threadID);

/**
 * ��?��?�?�ID
 *
 * @param[out]  pThreadID ��?�?�ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadIdCurrentGet(THREAD_HANDLE* pThreadID);

/**
 * ��?��?�?����?�
 *
 * @param[out]  pPriority ��?�?����?�
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadPriorityCurrentGet(uint32* pPriority);

/**
 * �����?����?�
 *
 * @param[in]  threadID ��?�?���?����?�ID
 * @param[in]  Priority ���?� 1-255 1������?�
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadPrioritySet(THREAD_HANDLE threadID, uint32 Priority);

/**
 * �??���
 * �???���(suspend)??�������ready
 * @param[in]  threadID �?�ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadResume(THREAD_HANDLE threadID);

/**
 * �??���
 * �??������(suspend)??
 * @param[in]  threadID �?�ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadSuspend(THREAD_HANDLE threadID);

/**
 * �??���?��?��?��
 * �??������(suspend)??��timeMs��?�ready??
 * @param[in]  timeMs ?��ms
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_Sleep(uint32 timeMs);

/**
 * ����������
 *
 * @param[in]  name ����������
 * @param[in]  inherit ���?��?� 0-��?�� 1-?��
 * @param[out]  pMutex ������handle
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_MutexCreate(const char* name, uint32 inherit, MUTEX_HANDLE* pMutex);

/**
 * ��?������
 *
 * @param[in]  mutex ������handle
 * @param[in]  timeout ��??��, ��?ms
 * @return 0 SUCCESS (�?���?����)  -1 FAIL
 */
int KING_MutexLock(MUTEX_HANDLE mutex, uint32 timeout);

/**
 * �??�����
 *
 * @param[in]  mutex ������handle
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_MutexUnLock(MUTEX_HANDLE mutex);

 /**
 * ?��������
 *
 * @param[in]  mutex ������handle
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_MutexDelete(MUTEX_HANDLE mutex);

 /**
  * ���������?���
  *
  * @param[in]  name �?�������
  * @param[in]  init_count �?�����?����
  * @param[out] pSem �?���handle
  * @return 0 SUCCESS  -1 FAIL
  */
int KING_SemCreate(const char* name, uint32 init_count, SEM_HANDLE* pSem);

  /**
  * ��?�����?���
  * �?���?�������?
  * @param[in]  sem �?���handle
  * @param[in]  timeout �?�?�?��?ms
  * @return 0 SUCCESS(�?���?���?���)  -1 FAIL
  */
int KING_SemGet(SEM_HANDLE sem, uint32 timeout);

  /**
  * �??����?���
  * �?��??������?
  * @param[in]  sem �?���handle
  * @return 0 SUCCESS  -1 FAIL
  */
int KING_SemPut(SEM_HANDLE sem);

  /**
  * ?�������?���
  *
  * @param[in]  sem �?���handle
  * @return 0 SUCCESS  -1 FAIL
  */
int KING_SemDelete(SEM_HANDLE sem);

  /**
  * �������
  *
  * @param[in]    name �������
  * @param[out] pEvent ���handle
  * @return 0 SUCCESS    -1 FAIL
  */
int KING_EventCreate(const char* name, EVENT_HANDLE* pEvent);

/**
* ��?�������
*
* @param[in] event ���handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_EventSet(EVENT_HANDLE event);

/**
* ?�����
*
* @param[in] event ���handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_EventDelete(EVENT_HANDLE event);

/**
* ��?���
* timeout?�???ms������������?���?�?��
* WAIT_OPT_NO_WAIT �� WAIT_OPT_INFINITE
*
* @param[in] event ���handle
* @param[in] clearFlag ��?��������?��������� 0-����� 1-���
* @param[in] timeout �?�?��, ��?ms
* @return 0 SUCCESS(�?���?�����)   -1 FAIL
*/
int KING_EventGet(EVENT_HANDLE event, uint32 clearFlag, uint32 timeout);

/**
* ������?����
*
* @param[out] pHandle ��?����handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MsgCreate(MSG_HANDLE* pHandle);

/**
* ������?
*
* @param[in] pMsg ��??��?��
* @param[in] handle ���?���?����handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MsgSend(MSG_S* pMsg, MSG_HANDLE handle);

/**
* ������?.
* timeout?�???ms������������?���?�?��
* WAIT_OPT_NO_WAIT �� WAIT_OPT_INFINITE
*
* @param[in] pMsg ��??��?��
* @param[in] handle ��?����handle
* @param[in] timeout ��??�?��?ms
* @return 0 SUCCESS(�?���?�����)   -1 FAIL
*/
int KING_MsgRcv(MSG_S* pMsg, MSG_HANDLE handle, uint32 timeout);

/**
* ?����?����
*
* @param[in] handle ��?����handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MsgDelete(MSG_HANDLE handle);

/**
* �?�����
* @param[in]  pMem ���?�?�??���?��
* @param[in]  size ?������?��?
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MemAlloc(void** pMem, uint32 size);

/**
* �?��?�
* @param[in]  pMem ?�???�??��
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MemFree(void* pMem);


/**
* ��?������
* @param[out]  pTimer ������timer id
* @return 0 SUCCESS   -1 FAIL
*/
int KING_TimerCreate(TIMER_HANDLE* pTimer);

/**
* ��?������
* @param[in]  Timer ?�����timer id
* @param[in]  pCfg ?����timer����?�������
* @return 0 SUCCESS   -1 FAIL
*/
int KING_TimerActive(TIMER_HANDLE Timer, TIMER_ATTR_S* pCfg);

/**
* ��?��?����
* @param[in]  Timer ?�����timer id
* @return 0 SUCCESS   -1 FAIL
*/
int KING_TimerDeactive(TIMER_HANDLE Timer);

/**
* ��?��?��
* @param[in]  Timer ??����timer id
* @return 0 SUCCESS   -1 FAIL
*/
int KING_TimerDelete(TIMER_HANDLE Timer);

#endif
