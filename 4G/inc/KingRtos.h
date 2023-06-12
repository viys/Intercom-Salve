#ifndef __KING_RTOS_H__
#define __KING_RTOS_H__

#include "KingDef.h"

// Event/Msg Wait option.
#define WAIT_OPT_NO_WAIT        0x0          // ���ȴ�
#define WAIT_OPT_INFINITE       0xFFFFFFFF   // ���õȴ�

typedef void (*THREAD_ENTRY_FUN)(void* param);
typedef void (*TIMER_FUN)(uint32 tmrId);

//thread���ȼ����壬��ֵԽС�����ȼ�Խ��
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
    TIMER_FUN timer_fun; ///< ��ʱ�����ڻص�����
    uint32 timeout; ///< ��ʱ������ʱ�䣬��λms
    bool isPeriod; ///< �Ƿ�������������ʱ����
}TIMER_ATTR_S;



/**
 * �����߳�
 *
 * @param[in]  pName �����߳�����
 * @param[in]  pAttr �߳���ز��� �� ��ջ�����ȼ�����ں���
               ���ȼ������THREAD_PRIORIT_E����ֵԽС���ȼ�Խ��
 * @param[out] pThreadID �����̵߳�ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadCreate(char* pName, THREAD_ATTR_S* pAttr,THREAD_HANDLE* pThreadID);

/**
 * �˳���ǰ�߳�
 * �߳��˳�����ʹ�����ַ�ʽ��ͬʱ�˳���õ������������������Դй¶
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadExit(void);

/**
 * ��ָֹ���߳�
 * ���Ƽ�ʹ�����ַ�ʽ��ֹ�̣߳������ȷ������ֹ���̵߳�ǰû��ռ������Դ�ȣ����ײ�������
 * @param[in]  threadID ��Ҫ��ֹ���߳�ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadTerminate(THREAD_HANDLE threadID);

/**
 * ��ȡ��ǰ�߳�ID
 *
 * @param[out]  pThreadID ��ǰ�߳�ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadIdCurrentGet(THREAD_HANDLE* pThreadID);

/**
 * ��ȡ��ǰ�߳����ȼ�
 *
 * @param[out]  pPriority ��ǰ�߳����ȼ�
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadPriorityCurrentGet(uint32* pPriority);

/**
 * �����߳����ȼ�
 *
 * @param[in]  threadID ��Ҫ�趨���ȼ����߳�ID
 * @param[in]  Priority ���ȼ� 1-255 1������ȼ�
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadPrioritySet(THREAD_HANDLE threadID, uint32 Priority);

/**
 * �̼߳���
 * �̴߳ӹ���(suspend)״̬���½���ready
 * @param[in]  threadID �߳�ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadResume(THREAD_HANDLE threadID);

/**
 * �̹߳���
 * �߳̽������(suspend)״̬
 * @param[in]  threadID �߳�ID
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ThreadSuspend(THREAD_HANDLE threadID);

/**
 * �̹߳���һ��ִ��ʱ��
 * �߳̽������(suspend)״̬��timeMs��ָ�ready״̬
 * @param[in]  timeMs ʱ��ms
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_Sleep(uint32 timeMs);

/**
 * ����������
 *
 * @param[in]  name ����������
 * @param[in]  inherit ���ȼ��̳� 0-��֧�� 1-֧��
 * @param[out]  pMutex ������handle
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_MutexCreate(const char* name, uint32 inherit, MUTEX_HANDLE* pMutex);

/**
 * ��ȡ������
 *
 * @param[in]  mutex ������handle
 * @param[in]  timeout ��ʱʱ��, ��λms
 * @return 0 SUCCESS (�ɹ���ȡ����)  -1 FAIL
 */
int KING_MutexLock(MUTEX_HANDLE mutex, uint32 timeout);

/**
 * �ͷŻ�����
 *
 * @param[in]  mutex ������handle
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_MutexUnLock(MUTEX_HANDLE mutex);

 /**
 * ɾ��������
 *
 * @param[in]  mutex ������handle
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_MutexDelete(MUTEX_HANDLE mutex);

 /**
  * ���������ź���
  *
  * @param[in]  name �ź�������
  * @param[in]  init_count �ź�����ʼ����
  * @param[out] pSem �ź���handle
  * @return 0 SUCCESS  -1 FAIL
  */
int KING_SemCreate(const char* name, uint32 init_count, SEM_HANDLE* pSem);

  /**
  * ��ȡ�����ź���
  * �ɹ���ȡ�������һ
  * @param[in]  sem �ź���handle
  * @param[in]  timeout �ȴ�ʱ�䣬��λms
  * @return 0 SUCCESS(�ɹ���ȡ���ź���)  -1 FAIL
  */
int KING_SemGet(SEM_HANDLE sem, uint32 timeout);

  /**
  * �ͷż����ź���
  * �ɹ��ͷź������һ
  * @param[in]  sem �ź���handle
  * @return 0 SUCCESS  -1 FAIL
  */
int KING_SemPut(SEM_HANDLE sem);

  /**
  * ɾ�������ź���
  *
  * @param[in]  sem �ź���handle
  * @return 0 SUCCESS  -1 FAIL
  */
int KING_SemDelete(SEM_HANDLE sem);

  /**
  * �����¼�
  *
  * @param[in]    name �¼�����
  * @param[out] pEvent �¼�handle
  * @return 0 SUCCESS    -1 FAIL
  */
int KING_EventCreate(const char* name, EVENT_HANDLE* pEvent);

/**
* ��־�¼�����
*
* @param[in] event �¼�handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_EventSet(EVENT_HANDLE event);

/**
* ɾ���¼�
*
* @param[in] event �¼�handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_EventDelete(EVENT_HANDLE event);

/**
* ��ȡ�¼�
* timeoutʱ�䵥λΪms������������ֵ���ֱ�Ϊ��
* WAIT_OPT_NO_WAIT �� WAIT_OPT_INFINITE
*
* @param[in] event �¼�handle
* @param[in] clearFlag ��ȡ���¼����Ƿ�������¼� 0-����� 1-���
* @param[in] timeout �ȴ�ʱ��, ��λms
* @return 0 SUCCESS(�ɹ���ȡ���¼�)   -1 FAIL
*/
int KING_EventGet(EVENT_HANDLE event, uint32 clearFlag, uint32 timeout);

/**
* ������Ϣ����
*
* @param[out] pHandle ��Ϣ����handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MsgCreate(MSG_HANDLE* pHandle);

/**
* ������Ϣ
*
* @param[in] pMsg ��Ϣʵ��ָ��
* @param[in] handle ���շ���Ϣ����handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MsgSend(MSG_S* pMsg, MSG_HANDLE handle);

/**
* ������Ϣ.
* timeoutʱ�䵥λΪms������������ֵ���ֱ�Ϊ��
* WAIT_OPT_NO_WAIT �� WAIT_OPT_INFINITE
*
* @param[in] pMsg ��Ϣʵ��ָ��
* @param[in] handle ��Ϣ����handle
* @param[in] timeout ��ʱʱ�䣬��λms
* @return 0 SUCCESS(�ɹ���ȡ���¼�)   -1 FAIL
*/
int KING_MsgRcv(MSG_S* pMsg, MSG_HANDLE handle, uint32 timeout);

/**
* ɾ����Ϣ����
*
* @param[in] handle ��Ϣ����handle
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MsgDelete(MSG_HANDLE handle);

/**
* �ڴ�����
* @param[in]  pMem ���뵽�ĵ�ַָ���ָ��
* @param[in]  size Ҫ������ڴ��С
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MemAlloc(void** pMem, uint32 size);

/**
* �ڴ��ͷ�
* @param[in]  pMem Ҫ�ͷŵĵ�ַָ��
* @return 0 SUCCESS   -1 FAIL
*/
int KING_MemFree(void* pMem);


/**
* ��ʱ������
* @param[out]  pTimer ������timer id
* @return 0 SUCCESS   -1 FAIL
*/
int KING_TimerCreate(TIMER_HANDLE* pTimer);

/**
* ��ʱ������
* @param[in]  Timer Ҫ�����timer id
* @param[in]  pCfg Ҫ����timer����ز�������
* @return 0 SUCCESS   -1 FAIL
*/
int KING_TimerActive(TIMER_HANDLE Timer, TIMER_ATTR_S* pCfg);

/**
* ��ʱ��ȥ����
* @param[in]  Timer ȥ�����timer id
* @return 0 SUCCESS   -1 FAIL
*/
int KING_TimerDeactive(TIMER_HANDLE Timer);

/**
* ��ʱ��ɾ��
* @param[in]  Timer Ҫɾ����timer id
* @return 0 SUCCESS   -1 FAIL
*/
int KING_TimerDelete(TIMER_HANDLE Timer);

#endif
