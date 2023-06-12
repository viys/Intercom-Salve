#ifndef __APP_H
#define __APP_H

#include "KingDef.h"
#include "KingPlat.h"
#include "KingDef.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingUart.h"

extern uint32 writeLen;
extern THREAD_ATTR_S threadAttr;

extern THREAD_HANDLE AppTaskStartThreadH;

/* 线程指针配置函数 */
THREAD_ATTR_S* KING_Thread_Config(THREAD_ENTRY_FUN fun,void* param,THREAD_PRIORIT_E priority,uint32 stackSize);
/* 线程高级配置函数 */
THREAD_ATTR_S* KING_Thread_ConfigA(uint32 autoStart,THREAD_ENTRY_FUN fun,void* param,uint32 preemption,THREAD_PRIORIT_E priority,uint32 queueNum,uint32 stackSize);
/* 线程创建函数 */
void AppTaskStart(void *param);

#endif /* __APP_H */
