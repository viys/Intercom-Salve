/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "app_main.h"
#include "kingplat.h"
#include "kingrtos.h"
#include "KingPowerManage.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("power: "fmt, ##args); } while(0)

/*******************************************************************************
 **  Function 
 ******************************************************************************/
static void power_WakeLockAndUnlock(void)
{
    int ret = -1;
    
    LogPrintf("power_WakeLockAndUnlock begin\r\n");
    
    ret = KING_WakeLock(LOCK_NONE, (uint8 *)"powerlock");
    if (FAIL == ret)
    {
        LogPrintf("KING_WakeLock LOCK_NONE fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    
    ret = KING_WakeLock(LOCK_SUSPEND, NULL);
    if (FAIL == ret)
    {
        LogPrintf("KING_WakeLock LOCK_SUSPEND fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_WakeLock(LOCK_SLEEP, (uint8 *)"powerlock");
    if (FAIL == ret)
    {
        LogPrintf("KING_WakeLock LOCK_SLEEP fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("KING_WakeLock LOCK_SLEEP end\r\n");

    KING_Sleep(20000);
    ret = KING_WakeUnlock(LOCK_SLEEP, (uint8 *)"powerlock");
    if (FAIL == ret)
    {
        LogPrintf("KING_WakeUnlock LOCK_SLEEP fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("KING_WakeUnlock LOCK_SLEEP end\r\n");
    
    KING_Sleep(10000);
    ret = KING_WakeLock(LOCK_SUSPEND, (uint8 *)"powerlock");
    if (FAIL == ret)
    {
        LogPrintf("KING_WakeLock LOCK_SUSPEND fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("KING_WakeLock LOCK_SUSPEND end\r\n");
    
    KING_Sleep(10000);
    ret = KING_WakeUnlock(LOCK_SUSPEND, (uint8 *)"powerlock");
    if (FAIL == ret)
    {
        LogPrintf("KING_WakeUnlock LOCK_SUSPEND fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("KING_WakeUnlock LOCK_SUSPEND end\r\n");
    
    KING_Sleep(10000);
    ret = KING_WakeLock(LOCK_SUSPEND, (uint8 *)"powerlock");
    if (FAIL == ret)
    {
        LogPrintf("KING_WakeLock LOCK_SUSPEND fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    
    LogPrintf("power_WakeLockAndUnlock end\r\n");
}

void power_test(int mode)
{
    LogPrintf("power test begin\r\n");
    
    power_WakeLockAndUnlock();
    if (mode == 0)
    {
        LogPrintf("Reboot system now...\r\n");
        KING_Sleep(1000);
        KING_PowerReboot();
    }
    else if (mode == 1)
    {
        LogPrintf("Power off system now...\r\n");
        KING_Sleep(1000);
        if (0 != KING_PowerOff())
        {
            LogPrintf("Power off system Failed!\r\n");
        }
    }
}

