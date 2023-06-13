#include "app.h"
#include "KingPowerManage.h"
#include "uart.h"

void APP_Main(uint32 bootMode)
{
    int ret = 0;

    POWER_REASON_E reason = bootMode;
    KING_SysLog("enter APP_Main... reason = %d", reason);
    
    KING_WakeLock(LOCK_SUSPEND, (uint8 *)"KingApp");

    KING_SysLog("enter APP_Main... reason =END");

    /* 创建启动任务函数 */
    ret = KING_ThreadCreate("AppTaskStart",KING_Thread_Config(AppTaskStart,NULL,4,4096),&AppTaskStartThreadH);
    LOG_P(ret,"KING_ThreadCreate() AppTaskStart Fail!\r\n");
}
