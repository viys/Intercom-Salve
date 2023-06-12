#include "KingDef.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingFileSystem.h"
#include "KingAudio.h"
#include "app_main.h"

#define LogPrintf(fmt, args...)    do { KING_SysLog("[AudioTest] "fmt, ##args); } while(0)
#define TEST_DUPLEX                     1
#define TEST_HALFDUPLEX                 0
#define AUDIO_HALFDUPLEX_RECORD         1
#define AUDIO_HALFDUPLEX_PLAY           2
#define RECORD_TIME                     5
#define AUDIO_TEST_PLAY_FILE            "/mnt/sd/8.pcm"
#define AUDIO_TEST_RECORD_FILE          "/record1.pcm"
#define TEST_HALFDUPLEX_RECORD_FILE     AUDIO_TEST_RECORD_FILE
#define TEST_HALFDUPLEX_PLAY_FILE       TEST_HALFDUPLEX_RECORD_FILE
#define AUDIO_TEST_RECORD_FILE2         "/record2.pcm"
#define AUDIO_TEST_RECORD_FILE3         "/record3.pcm"

#define TEST_COUNT_MAX                  3
static uint8 s_test_count = 0;
static TIMER_HANDLE  s_rcdtimer_handle = NULL;

enum Msg_Evt
{
    MSG_EVT_BASE = 0,
    MSG_EVT_AUDIO_STOP,
    MSG_EVT_RECORD_STOP,
    MSG_EVT_RECORD,
    MSG_EVT_PLYRCD,
    MSG_EVT_PLYRCD_STOP,
    MSG_EVT_END,
};


static MSG_HANDLE msgHandle;
static FS_HANDLE PlayFileHandle;
static FS_HANDLE RecordFileHandle;

void Test_PlayFile_Open(void)
{
    int ret = FAIL;
    ret = KING_FsFileCreate(AUDIO_TEST_PLAY_FILE, FS_MODE_OPEN_EXISTING | FS_MODE_READ | FS_MODE_WRITE, 0, 0, &PlayFileHandle);
    LogPrintf("%s:Open %s ret %d", __FUNCTION__, AUDIO_TEST_PLAY_FILE, ret);
}

void Test_RecordFile_Open(void)
{
    int ret = FAIL;
    ret = KING_FsFileCreate(AUDIO_TEST_RECORD_FILE, FS_MODE_CREATE_ALWAYS | FS_MODE_READ | FS_MODE_WRITE, 0, 0, &RecordFileHandle);
    LogPrintf("%s:Open %s ret %d", __FUNCTION__, AUDIO_TEST_RECORD_FILE, ret);
}

static void Timer_Callback(uint32 tmrId)
{
    MSG_S msg;
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = MSG_EVT_PLYRCD;
    KING_MsgSend(&msg, msgHandle);
    LogPrintf("%s:SendMsg", __FUNCTION__);
}

void AudioPlayCb(AUDIO_EVENT_E event, uint8 *buff, uint32 *len)
{
    int ret = SUCCESS;
    MSG_S msg;
    uint8 pcmdata[640] = {0};
    uint32 bytesRead = 0;
    LogPrintf("%s:evnet:%d", __FUNCTION__, event);
    switch(event)
    {
        case AUDIO_PLAY_FILL_BUFF:
        {
            ret = KING_FsFileRead(PlayFileHandle, pcmdata, 320, &bytesRead);
            if (FAIL == ret)
            {
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = MSG_EVT_BASE;
                KING_MsgSend(&msg, msgHandle);
                return;
            }
            *len = bytesRead;
            if(bytesRead <= 0)
            {
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = MSG_EVT_AUDIO_STOP;
                KING_MsgSend(&msg, msgHandle);
                return;
            }  
            memcpy(buff, pcmdata, bytesRead);
            break;
        }
        case AUDIO_PLAY_STOP:
        {
            LogPrintf("AUDIO_PLAY_STOP!\r\n");
            memset(&msg, 0x00, sizeof(MSG_S));
            msg.messageID = MSG_EVT_BASE;
            KING_MsgSend(&msg, msgHandle);
            break;
        }

        default:
        {
            break;
        }
    }     
}


void AudioRecordCb(AUDIO_EVENT_E event, uint8 *buff, uint32 *len)
{
    MSG_S msg;
    uint8 pcmdata[640] = {0};
    static uint32 val = 0;
    LogPrintf("%s:evnet:%d", __FUNCTION__, event);
    switch(event)
    {
        case AUDIO_RECORD_ONE_FRAME:
        {
            val++;
            if(val <= (RECORD_TIME * 50))
            {
                memset(pcmdata, 0x00, 640);
                memcpy(pcmdata, buff, *len);
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = MSG_EVT_RECORD;
                msg.pData = pcmdata;
                msg.DataLen = *len;
                KING_MsgSend(&msg, msgHandle);
            }
            else
            {
                val = 0;
                *len = 0;
                KING_AudioRecordStop();
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = MSG_EVT_RECORD_STOP;
                KING_MsgSend(&msg, msgHandle);
            }
            break;
        }

        default:
        {
            break;
        }
    }
}
void AudioHalfRecordCb(AUDIO_EVENT_E event, uint8 *buff, uint32 *len)
{
    MSG_S msg;
    uint8 pcmdata[640] = {0};
    static uint32 val = 0;
    LogPrintf("%s:evnet:%d", __FUNCTION__, event);
    switch(event)
    {
        case AUDIO_RECORD_ONE_FRAME:
        {
            val++;
            if(val <= (RECORD_TIME * 50))
            {
                memset(pcmdata, 0x00, 640);
                memcpy(pcmdata, buff, *len);
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = MSG_EVT_RECORD;
                msg.pData = pcmdata;
                msg.DataLen = *len;
                KING_MsgSend(&msg, msgHandle);
            }
            else
            {
                val = 0;
                *len = 0;
                //半双工接口不需要stop
                //KING_AudioRecordStop();
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = MSG_EVT_RECORD_STOP;
                KING_MsgSend(&msg, msgHandle);
            }
            break;
        }

        default:
        {
            break;
        }
    }
}

void AudioPlyRcdCb(AUDIO_EVENT_E event, uint8 *buff, uint32 *len)
{
    int ret = SUCCESS;
    MSG_S msg;
    uint8 pcmdata[640] = {0};
    uint32 bytesRead = 0;
    LogPrintf("%s:evnet:%d", __FUNCTION__, event);
    switch(event)
    {
        case AUDIO_PLAY_FILL_BUFF:
        {
            ret = KING_FsFileRead(RecordFileHandle, pcmdata, 320, &bytesRead);
            if (FAIL == ret)
            {
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = MSG_EVT_PLYRCD_STOP;
                KING_MsgSend(&msg, msgHandle);
                return;
            }
            *len = bytesRead;
            if(bytesRead <= 0)
            {
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = MSG_EVT_PLYRCD_STOP;
                KING_MsgSend(&msg, msgHandle);
                return;
            }  
            memcpy(buff, pcmdata, bytesRead);
            break;
        }
        case AUDIO_PLAY_STOP:
        {
            LogPrintf("AUDIO_PLAY_STOP!\r\n");
            memset(&msg, 0x00, sizeof(MSG_S));
            msg.messageID = MSG_EVT_BASE;
            KING_MsgSend(&msg, msgHandle);
            break;
        }

        default:
        {
            break;
        }
    }     
}

/*************************************************************
函数名称:AudioDuplexTest
函数描述:AudioPoc模式全双工测试代码
说     明:从SD卡中播放PCM的同时录制PCM声音，最后单独播放录制的PCM
*************************************************************/
void AudioDuplexTest(void)
{
    int ret = SUCCESS;
    MSG_S msg;
    uint32 size;
    TIMER_ATTR_S timerattr;

    LogPrintf("%s Enter", __FUNCTION__);
    KING_FsInit();
    //挂载SD卡，方便从SD卡中直接播放PCM文件
    KING_FSMount(STORAGE_TYPE_SD,1,1,1,1);
    Test_PlayFile_Open();
    Test_RecordFile_Open();
    ret = KING_AudioPlayInit();
    LogPrintf("KING_AudioPlayInit ret=%d",ret);
    ret = KING_AudioRecordInit();
    LogPrintf("KING_AudioRecordInit ret=%d",ret);
    ret = KING_TimerCreate(&s_rcdtimer_handle);
    if(FAIL == ret)
    {
        LogPrintf("KING_TimerCreate() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    memset(&timerattr, 0x00, sizeof(timerattr));
    timerattr.isPeriod= FALSE;
    timerattr.timeout = 10000;
    timerattr.timer_fun = Timer_Callback;
    
    ret = KING_MsgCreate(&msgHandle);
    if(FAIL == ret)
    {
        LogPrintf("KING_MsgCreate Fail\r\n");
    }

    KING_AudioChannelSet(AUDIO_MIC, 0);
    KING_AudioChannelSet(AUDIO_SPEAK, 1);
    KING_AudioVolumeSet(AUDIO_MIC, 90);
    KING_AudioVolumeSet(AUDIO_SPEAK, 90);
    //cb读文件，播放PCM
    KING_AudioPcmStreamPlayV2(SAMPLING_RATE_8KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, AUDEV_PLAY_TYPE_POC, AudioPlayCb);
    //cb保存文件，录制成PCM
    KING_AudioRecordStartV2(SAMPLING_RATE_8KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, AUDEV_RECORD_TYPE_POC, AudioRecordCb);

    //全双工模式下必须调用KING_AudioDevStartPocMode启动
    ret = KING_AudioDevStartPocMode(TEST_DUPLEX);
    LogPrintf("KING_AudioDevStartPocMode ret=%d", ret);
    ret = KING_TimerActive(s_rcdtimer_handle, &timerattr);
    if(FAIL == ret)
    {
        LogPrintf("KING_TimerActive() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        KING_TimerDelete(s_rcdtimer_handle);
        return;
    }
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg, msgHandle, WAIT_OPT_INFINITE);

        switch (msg.messageID)
        {
            case MSG_EVT_AUDIO_STOP:
            {
                LogPrintf("stop!\r\n");
                KING_FsCloseHandle(PlayFileHandle);
                
                break;
            }
            case MSG_EVT_RECORD:
            {
                uint32 bytesWrite = 0;
                KING_FsFileWrite(RecordFileHandle, msg.pData, msg.DataLen, &bytesWrite);
                
                break;
            }
            case MSG_EVT_RECORD_STOP:
            {
                ret = KING_AudioDevStopPocMode();
                LogPrintf("KING_AudioDevStopPocMode %d \r\n", ret);
                KING_FsFileSizeGet(RecordFileHandle, &size);
                LogPrintf("get file size %d \r\n", size);
                KING_FsCloseHandle(RecordFileHandle);
                RecordFileHandle = NULL;
                KING_AudioRecordStop();
                KING_AudioStop();
                break;
            }
            case MSG_EVT_PLYRCD://播放录制的PCM文件
            {
                KING_AudioPlayDeinit();
                KING_AudioPlayInit();
                KING_FsFileCreate(AUDIO_TEST_RECORD_FILE, FS_MODE_OPEN_EXISTING | FS_MODE_READ, 0, 0, &RecordFileHandle);
                KING_AudioPcmStreamPlay(SAMPLING_RATE_8KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, AudioPlyRcdCb);
                break;
            }
            case MSG_EVT_PLYRCD_STOP:
            {
                ret = KING_AudioStop();
                KING_FsCloseHandle(RecordFileHandle);
                RecordFileHandle = NULL;
                break;
            }
            default:
                break;
        }
    }
}

/*************************************************************
函数名称:AudioHalfDuplexTest
函数描述:AudioPoc模式半双工测试代码
说     明:切换成半双工录制PCM，再切换成半双工播放，循环3次
*************************************************************/
void AudioHalfDuplexTest(void)
{
    int ret = SUCCESS;
    MSG_S msg = { 0 };
    uint32 size = 0;

    LogPrintf("%s Enter", __FUNCTION__);
    KING_FsInit();
    Test_RecordFile_Open();
    KING_AudioPlayInit();
    KING_AudioRecordInit();

    ret = KING_MsgCreate(&msgHandle);
    if(FAIL == ret)
    {
        LogPrintf("KING_MsgCreate Fail\r\n");
    }
    ret = KING_TimerCreate(&s_rcdtimer_handle);
    if(FAIL == ret)
    {
        LogPrintf("KING_TimerCreate() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    KING_AudioChannelSet(AUDIO_MIC, 0);
    KING_AudioChannelSet(AUDIO_SPEAK, 1);
    KING_AudioVolumeSet(AUDIO_MIC, 90);
    KING_AudioVolumeSet(AUDIO_SPEAK, 90);

    KING_AudioPcmStreamPlayV2(SAMPLING_RATE_8KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, AUDEV_PLAY_TYPE_POC, AudioPlayCb);

    KING_AudioRecordStartV2(SAMPLING_RATE_8KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, AUDEV_RECORD_TYPE_POC, AudioHalfRecordCb);

    ret = KING_AudioDevStartPocMode(TEST_HALFDUPLEX);
    LogPrintf("KING_AudioDevStartPocMode ret=%d", ret);

    //以半双工方式启动Poc模式，先半双工录制
    ret = KING_AudioDevPocModeSwitch(AUDIO_HALFDUPLEX_RECORD);

    if(FAIL == ret)
    {
        LogPrintf("KING_TimerActive() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        KING_TimerDelete(s_rcdtimer_handle);
        return;
    }
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg, msgHandle, WAIT_OPT_INFINITE);

        switch (msg.messageID)
        {
            case MSG_EVT_AUDIO_STOP:
            {
                s_test_count++;
                LogPrintf("NO.%d MSG_EVT_AUDIO_STOP", s_test_count);
                KING_FsCloseHandle(PlayFileHandle);
                PlayFileHandle = NULL;
                if(s_test_count == 1)
                {
                    KING_FsFileCreate(AUDIO_TEST_RECORD_FILE2, FS_MODE_CREATE_ALWAYS | FS_MODE_READ | FS_MODE_WRITE, 0, 0, &RecordFileHandle);
                    ret = KING_AudioDevPocModeSwitch(AUDIO_HALFDUPLEX_RECORD);
                    LogPrintf("KING_AudioDevPocModeSwitch record ret=%d", ret);
                }
                else if (s_test_count == 2)
                {
                    KING_FsFileCreate(AUDIO_TEST_RECORD_FILE3, FS_MODE_CREATE_ALWAYS | FS_MODE_READ | FS_MODE_WRITE, 0, 0, &RecordFileHandle);
                    ret = KING_AudioDevPocModeSwitch(AUDIO_HALFDUPLEX_RECORD);
                    LogPrintf("KING_AudioDevPocModeSwitch record ret=%d", ret);
                }
                else
                {
                    ret = KING_AudioDevStopPocMode();
                    LogPrintf("KING_AudioDevStopPocMode ret=%d", ret);
                }
                
                break;
            }
            case MSG_EVT_RECORD:
            {
                uint32 bytesWrite = 0;
                KING_FsFileWrite(RecordFileHandle, msg.pData, msg.DataLen, &bytesWrite);
                break;
            }

            case MSG_EVT_RECORD_STOP:
            {
                KING_FsFileSizeGet(RecordFileHandle, &size);
                LogPrintf("Stop record, record file size=%d", size);
                KING_FsCloseHandle(RecordFileHandle);
                RecordFileHandle = NULL;
                if(s_test_count == 0)
                {
                    ret = KING_FsFileCreate(AUDIO_TEST_RECORD_FILE, FS_MODE_OPEN_EXISTING | FS_MODE_READ, 0, 0, &PlayFileHandle);
                    ret = KING_AudioDevPocModeSwitch(AUDIO_HALFDUPLEX_PLAY);
                    LogPrintf("KING_AudioDevPocModeSwitch play ret=%d", ret);
                }
                else if(s_test_count == 1)
                {
                    ret = KING_FsFileCreate(AUDIO_TEST_RECORD_FILE2, FS_MODE_OPEN_EXISTING | FS_MODE_READ, 0, 0, &PlayFileHandle);
                    ret = KING_AudioDevPocModeSwitch(AUDIO_HALFDUPLEX_PLAY);
                    LogPrintf("KING_AudioDevPocModeSwitch play ret=%d", ret);
                }
                else if(s_test_count == 2)
                {
                    ret = KING_FsFileCreate(AUDIO_TEST_RECORD_FILE3, FS_MODE_OPEN_EXISTING | FS_MODE_READ, 0, 0, &PlayFileHandle);
                    ret = KING_AudioDevPocModeSwitch(AUDIO_HALFDUPLEX_PLAY);
                    LogPrintf("KING_AudioDevPocModeSwitch play ret=%d", ret);
                }

                break;
            }

            default:
                break;
        }
    }
}

