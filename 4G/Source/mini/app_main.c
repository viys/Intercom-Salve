#include "app_main.h"
#include "KingPowerManage.h"


extern void AudioDuplexthread(void);
extern void socket_thread(void);
extern void socket_test22(void);
extern void AudioPlayMP3_init(void);
extern void Fota_test(void);

void APP_Main(uint32 bootMode)
{
    POWER_REASON_E reason = bootMode;
    KING_SysLog("enter APP_Main... reason = %d", reason);
    
    KING_WakeLock(LOCK_SUSPEND, (uint8 *)"KingApp");

    Fota_test();
    AudioPlayMP3_init();
    socket_thread();
    AudioDuplexthread();

    KING_SysLog("enter APP_Main... reason =END");
}
// reconnect