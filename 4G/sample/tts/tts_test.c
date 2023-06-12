/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingcstd.h"
#include "kingrtos.h"
#include "kingplat.h"
#include "kingtts.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("TTS: "fmt, ##args); } while(0)

/*******************************************************************************
 **  Function 
 ******************************************************************************/
static void tts_paramGet(void)
{
    int ret = -1;
    TTS_PARAM_S ttsParam;

    memset(&ttsParam, 0x00, sizeof(ttsParam));
    ret = KING_TtsParamsGet(&ttsParam);
    if (ret == 0)
    {
        LogPrintf("Volume: %d, readdigit:%d speed:%d, pitch :%d, role = %d, readalp = %d, language = %d", 
                    ttsParam.volume, ttsParam.readDigit, ttsParam.speed, ttsParam.pitch,
                    ttsParam.role, ttsParam.readalp, ttsParam.language);
    }
}

static void tts_paramSet(void)
{
    int ret = -1;
    TTS_PARAM_S ttsParam;

    memset(&ttsParam, 0x00, sizeof(ttsParam));
    ttsParam.volume = 32767;
    ttsParam.readDigit = 0;
    ttsParam.speed = 897;
    ttsParam.pitch = 567;
    ret = KING_TtsParamsSet(&ttsParam);
    LogPrintf("KING_TtsParamsSet ret=%d", ret);

    tts_paramGet();
}

static void tts_playCB(TTS_PLAY_E event, uint8 *buff, uint32 len)
{
    LogPrintf("event = %d", event);
    if(TTS_PLAY_STOP == event)
    {
        LogPrintf("TTS_PLAY_STOP");
    }
    else if(TTS_PLAY_START == event)
    {
        LogPrintf("TTS_PLAY_START");
    }
}

static void tts_play(void)
{
    int ret = -1;
    
    ret = KING_TtsPlay(CODE_GBK, (uint8*)"交易撤销成功", strlen("交易撤销成功"), tts_playCB);
    LogPrintf("KING_TtsPlay ret=%d", ret);
    KING_Sleep(3000);
    //欢迎使用语音合成系统
    ret = KING_TtsPlay(CODE_UCS2, (uint8*)"6B228FCE4F7F75288BED97F3540862107CFB7EDF", strlen("6B228FCE4F7F75288BED97F3540862107CFB7EDF"), tts_playCB);
    LogPrintf("KING_TtsPlay ret=%d", ret);
}

void tts_test(void)
{
    int ret = -1;
    
    ret = KING_TtsInit();
    if (ret != 0)
    {
        LogPrintf("KING_TtsInit is fail!");
        return;
    }
    tts_paramGet();
    tts_paramSet();
    tts_play();

    KING_Sleep(10000);

    ret = KING_TtsStop();
    LogPrintf("KING_TtsStop ret=%d", ret);
    ret = KING_TtsDeinit();
    LogPrintf("KING_TtsDeinit ret=%d", ret);
}

