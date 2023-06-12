#include "app_main.h"
#include "KingLedSink.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @note PWM ID refers to specific pin of the module.
 * PWM frequency supported by different platform is different.
 * If not sure, please check with Cheerzing engineer.
 */
#define PWM_ID_0        0   // unusable on ML2510 and ML8110
#define PWM_ID_1        1   // UART1_CTS on ML2510, unusable on ML8110
#define PWM_ID_2        2   // unusable on ML2510, GPIO10 on ML8110
#define PWM_ID_3        3   // unusable on ML2510, GPIO11 on ML8110

#define TEST_PWM_FREQ           10000   // 10K
#define TEST_PWM_DUTY_CYCLE     40      // 40%

void pwm_test(void)
{
    int ret = -1;

    // PWM_ID_1 is available on ML2510
    ret = KING_PwmInit(PWM_ID_1);
    if (-1 == ret)
    {
        LogPrintf("KING_PwmInit() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    // ML2510: freq set from 1K to 200K, duty_cycle 0-100.
    ret = KING_PwmConfig(PWM_ID_1, TEST_PWM_FREQ, TEST_PWM_DUTY_CYCLE);
    if (-1 == ret)
    {
        LogPrintf("KING_PwmConfig() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    LogPrintf("Start PWM[%d], FREQ[%d], DUTY_CYCLE[%d]\r\n", PWM_ID_1, TEST_PWM_FREQ, TEST_PWM_DUTY_CYCLE);
    ret = KING_PwmStart(PWM_ID_1);
    if (-1 == ret)
    {
        LogPrintf("KING_PwmStart() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    KING_Sleep(60000);

    ret = KING_PwmStop(PWM_ID_1);
    if (-1 == ret)
    {
        LogPrintf("KING_PwmStop() errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    LogPrintf("Stop PWM. ==== PWM TEST END ====.");

    KING_PwmDeinit(PWM_ID_1);
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

