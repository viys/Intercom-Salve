/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingFileSystem.h"
#include "KingAudio.h"
#include "app_main.h"

/*******************************************************************************
 ** MACROS
 ******************************************************************************/
#define LogPrintf(fmt, args...)    do { KING_SysLog("audio: "fmt, ##args); } while(0)    
#define FS_TEST_FILE1   "test1.pcm"
#define RECORD_TIME     5 //5s 

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
    
enum Msg_Evt
{
    MSG_EVT_BASE = 0,
    MSG_EVT_AUDIO_STOP,
    MSG_EVT_RECORD_STOP,
    MSG_EVT_RECORD,
    MSG_EVT_END,
};

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static MSG_HANDLE msgHandle;
static FS_HANDLE FileHandle;
static uint8 isAudioFin = 0;

/*******************************************************************************
 ** External Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Local Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Functions
 ******************************************************************************/
/**
 *  播放和录音回调函数
 *
 **/
void AudioCb(AUDIO_EVENT_E event, uint8 *buff, uint32 *len)
{
    int ret = SUCCESS;
    MSG_S msg;
    uint8 pcmdata[640] = {0};
    uint32 bytesRead = 0;
    static uint32 val = 0;
    switch(event)
    {
        case AUDIO_PLAY_FILL_BUFF:
        {
            ret = KING_FsFileRead(FileHandle, pcmdata, 320, &bytesRead);
            if (FAIL == ret)
            {
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = MSG_EVT_AUDIO_STOP;
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
            isAudioFin = 1;
            memset(&msg, 0x00, sizeof(MSG_S));
            msg.messageID = MSG_EVT_BASE;
            KING_MsgSend(&msg, msgHandle);
            break;
        }
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

/**
 *  录音
 *
 **/
void Record_Test(void)
{
    int ret = SUCCESS;
    uint32 size;
    

    //文件不存在创建完后打开文件；文件存在则打开文件
    ret = KING_FsFileCreate(FS_TEST_FILE1, FS_MODE_CREATE_ALWAYS | FS_MODE_READ | FS_MODE_WRITE, 0, 0, &FileHandle);
    if (FAIL == ret)
    {
        LogPrintf("KING_FsFileCreate fail errorcode: 0x%X\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("open existing fileHandle；%d\r\n", FileHandle);
    //文件大小
    ret = KING_FsFileSizeGet(FileHandle, &size);
    if (FAIL == ret)
    {
        LogPrintf("get file size fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        KING_FsCloseHandle(FileHandle);
        return;
    }
    LogPrintf("get file size %d \r\n", size);

    ret = KING_AudioRecordInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioRecordInit fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        KING_FsCloseHandle(FileHandle);
        return;
    }

    ret = KING_AudioRecordStart(SAMPLING_RATE_8KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, AudioCb);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioRecordStart fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        KING_FsCloseHandle(FileHandle);
        return;
    }
    LogPrintf("Audio Record Start\r\n");
    
    KING_Sleep(1000);
    ret = KING_AudioRecordPause();
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioRecordPause fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("Audio pause end\r\n");
    
    KING_Sleep(1000);
    ret = KING_AudioRecordResume();
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioRecordResume fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
    }
    
    LogPrintf("Audio resume end\r\n");
}

/**
 *  播放
 *
 **/
void Play_Test(void)
{
    int ret = SUCCESS;
    uint32 size;
    ret = KING_FsFileCreate(FS_TEST_FILE1, FS_MODE_OPEN_EXISTING | FS_MODE_READ | FS_MODE_WRITE, 0, 0, &FileHandle);
    if (FAIL == ret)
    {
        LogPrintf("KING_FsFileCreate fail errorcode: 0x%X\r\n", KING_GetLastErrCode());
        return;
    }
    LogPrintf("open existing fileHandle；%d\r\n", FileHandle);
    //文件大小
    ret = KING_FsFileSizeGet(FileHandle, &size);
    if (FAIL == ret)
    {
        LogPrintf("get file size fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        KING_FsCloseHandle(FileHandle);
        return;
    }
    LogPrintf("get file size %d \r\n", size);

    ret = KING_AudioPlayInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioPlayInit fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        KING_FsCloseHandle(FileHandle);
        return;
    }

    ret = KING_AudioPcmStreamPlay(SAMPLING_RATE_8KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, AudioCb);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioPcmStreamPlay fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        KING_FsCloseHandle(FileHandle);
        return;
    }
    LogPrintf("Audio play Start\r\n");
}

static void Audio_channelSetAndGet(void)
{
    int ret = SUCCESS;
    uint32 channel = 0;

    ret = KING_AudioChannelGet(AUDIO_MIC, &channel);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_MIC Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_MIC channel=%u\r\n", channel);
    }

    channel = 1;
    ret = KING_AudioChannelSet(AUDIO_MIC, channel);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_MIC Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    ret = KING_AudioChannelGet(AUDIO_MIC, &channel);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_MIC Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_MIC 11 channel=%u\r\n", channel);
    }
    
    channel = 0;
    ret = KING_AudioChannelSet(AUDIO_MIC, channel);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_MIC Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    ret = KING_AudioChannelGet(AUDIO_MIC, &channel);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_MIC Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    } 
    else
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_MIC 22 channel=%u\r\n", channel);
    }

    ret = KING_AudioChannelGet(AUDIO_SPEAK, &channel);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_SPEAK Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_SPEAK channel=%u\r\n", channel);
    }

    channel = 0;
    ret = KING_AudioChannelSet(AUDIO_SPEAK, channel);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_SPEAK Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    
    ret = KING_AudioChannelGet(AUDIO_SPEAK, &channel);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_SPEAK Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    } 
    else
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_SPEAK 11 channel=%u\r\n", channel);
    }
    
    channel = 1;
    ret = KING_AudioChannelSet(AUDIO_SPEAK, channel);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_SPEAK Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    
    ret = KING_AudioChannelGet(AUDIO_SPEAK, &channel);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_SPEAK Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_AudioChannelGet() AUDIO_SPEAK 22 channel=%u\r\n", channel);
    }
    
}

static void Audio_volumeGetAndSet(void)
{
    int ret = SUCCESS;
    uint32 volumeVal = 0;
    
    ret = KING_AudioVolumeGet(AUDIO_MIC, &volumeVal);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioVolumeGet() AUDIO_MIC errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_AudioVolumeGet() AUDIO_MIC volumeVal=%u\r\n", volumeVal);
    }
    
    volumeVal = 90;
    ret = KING_AudioVolumeSet(AUDIO_MIC, volumeVal);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioVolumeSet() AUDIO_MIC errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    volumeVal = 0;
    ret = KING_AudioVolumeGet(AUDIO_MIC, &volumeVal);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioVolumeGet() errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_AudioVolumeGet() AUDIO_MIC 11 volumeVal=%u\r\n", volumeVal);
    }
    
    volumeVal = 0;
    ret = KING_AudioVolumeGet(AUDIO_SPEAK, &volumeVal);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioVolumeGet() AUDIO_SPEAK errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_AudioVolumeGet() AUDIO_SPEAK volumeVal=%u\r\n", volumeVal);
    }
    
    volumeVal = 90;
    ret = KING_AudioVolumeSet(AUDIO_SPEAK, volumeVal);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioVolumeSet() AUDIO_SPEAK errcode=0x%x\r\n", KING_GetLastErrCode());
    }

    volumeVal = 0;
    ret = KING_AudioVolumeGet(AUDIO_SPEAK, &volumeVal);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioVolumeGet() errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    else
    {
        LogPrintf("KING_AudioVolumeGet() AUDIO_SPEAK 11 volumeVal=%u\r\n", volumeVal);
    }
}

static void dtmf_test(void)
{
    LogPrintf("dtmf_test start\r\n");
    int ret = SUCCESS;
    
    ret = KING_DtmfInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_DtmfInit() errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_DtmfPlay("1", 200, 0);
    if (FAIL == ret)
    {
        LogPrintf("KING_DtmfPlay() errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    KING_Sleep(2000);
    ret = KING_DtmfStop();
    if (FAIL == ret)
    {
        LogPrintf("KING_DtmfStop() errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_DtmfDeInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_DtmfDeInit() errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    LogPrintf("dtmf_test end\r\n");
}

/**
 *  播放和录音测试
 *
 **/
void Audio_Test(void)
{
    int ret = SUCCESS;
    MSG_S msg;
    uint32 size;

    Audio_channelSetAndGet();
    Audio_volumeGetAndSet();

    Record_Test();

    //msg
    ret = KING_MsgCreate(&msgHandle);
    if(FAIL == ret)
    {
        LogPrintf("KING_MsgCreate Fail\r\n");
    }
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg, msgHandle, WAIT_OPT_INFINITE);
        if (isAudioFin)
        {
            KING_AudioPlayDeinit();
            break;
        }
        switch (msg.messageID)
        {
            case MSG_EVT_AUDIO_STOP:
            {
                
                LogPrintf("stop!\r\n");
                KING_AudioStop();
                KING_FsCloseHandle(FileHandle);
                break;
            }
            case MSG_EVT_RECORD:
            {
                uint32 bytesWrite = 0;
                KING_FsFileWrite(FileHandle, msg.pData, msg.DataLen, &bytesWrite);
                
                break;
            }
            case MSG_EVT_RECORD_STOP:
            {
                //文件大小
                KING_FsFileSizeGet(FileHandle, &size);
                LogPrintf("get file size %d \r\n", size);
                KING_FsCloseHandle(FileHandle);
                FileHandle = 0;
                KING_AudioRecordStop();
                KING_AudioRecordDeinit();

                //播放
                Play_Test();
                break;
            }
            default:
                break;
        }
    }
    
    KING_Sleep(3000);
    dtmf_test();

    LogPrintf("Audio Test completely!!!\r\n");
}



