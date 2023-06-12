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
#define LogPrintf(fmt, args...)    do { KING_SysLog("AudioTest: "fmt, ##args); } while(0)
#define FS_TEST_FILE1   "test1.amr"
#define PLAY_RECORD_FORMAT AUD_FORMAT_AMRWB         //AUD_FORMAT_AMRNB
#define TEST_SAMPLING_RATE  SAMPLING_RATE_16KHZ     //SAMPLING_RATE_8KHZ

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
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


/*******************************************************************************
 ** Variables
 ******************************************************************************/
static FS_HANDLE gFileHandle;

/*******************************************************************************
 ** External Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Local Function Declerations
 ******************************************************************************/

/*******************************************************************************
 ** Functions
 ******************************************************************************/
#define MP3_FILE_TAG_HEAD_LEN    10
#define DATA_READ_LEN       4096
#define DATA_PLAY_READ_LEN       (1 * 1152)//2048
static THREAD_HANDLE testThreadH = NULL;
static MSG_HANDLE msgHandle = NULL; 


void AudioInit(void)
{
    int ret = 0;
    ret = KING_AudioPlayInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioPlayInit fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        return;
    }

    ret = KING_AudioRecordInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioRecordInit fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        return;
    }
}
int AudioStopPlay(void)
{
    int ret = SUCCESS;
    
    ret = KING_AudioStop();
    if(gFileHandle != NULL)
    {
        KING_FsCloseHandle(gFileHandle);
        gFileHandle = NULL;
    }
    return ret;
}

static void PlayAmrCB(AUDIO_EVENT_E event, uint8 *buff, uint32 *len)
{
    LogPrintf("Audio Event:%d:%d\r\n", event,*len);
    uint8 Amrdata[DATA_READ_LEN] = {0};
    uint32 bytesRead = 0;
    int ret = SUCCESS;
    
    switch(event)
    {
        case AUDIO_PLAY_FILL_BUFF:
        {
            if(*len < DATA_READ_LEN)
            {
                ret = KING_FsFileRead(gFileHandle, Amrdata, *len, &bytesRead);
            }
            else
            {
                ret = KING_FsFileRead(gFileHandle, Amrdata, DATA_READ_LEN, &bytesRead);
            }
            LogPrintf("KING_FsFileRead ret=%d, readByte=%d\r\n", ret, bytesRead);
            
            if (FAIL == ret || bytesRead <= 0)
            {
                KING_AudioAbort();
                KING_FsCloseHandle(gFileHandle);
                gFileHandle = NULL;
                *len = 0;
                return;
            }
            
            *len = bytesRead;
            memcpy(buff, Amrdata, bytesRead);
            break;
        }
        case AUDIO_PLAY_STOP:
        {
            LogPrintf("AUDIO_PLAY_STOP!\r\n");
            //appMsgSend(APP_MSG_AUDIO_PLAY_NEXT, NULL, 0);
            break;
        }
        default:
        {
            break;
        }
    }
}

static int CalcFrameSize(unsigned char *tag)
{
    int frame_size = 0;
    frame_size =
            (((int)(((*(tag + 0)) & 0x7f))) << 21) +
            (((int)(((*(tag + 1)) & 0x7f))) << 14) +
            (((int)(((*(tag + 2)) & 0x7f))) << 7) +
            (((int)(((*(tag + 3)) & 0x7f))) << 0);

    return frame_size;
}

int Mp3tagHeaderDecode(unsigned char *tag)
{
    int tag_size = 0;

    if(strncmp((const char *)tag, (const char *)"ID3", 3) == 0) 
    {
        tag_size = CalcFrameSize(&tag[6]);
    } 
    else 
    {
        return -1;
    }
    return tag_size;
}

int AudioRecordAmr(char * fileName)
{
    int ret = 0;
    AUDIO_STATE_E status;
    uint32 count = 0;

    ret = KING_MsgCreate(&msgHandle);
    LogPrintf("%s", __FUNCTION__);
    if(FAIL == ret)
    {
        LogPrintf("KING_MsgCreate Fail\r\n");
    }
    KING_AudioStatusGet(&status);
    if(AUDIO_STATE_BUSY == status)
    {
        AudioStopPlay();
    }
    while(AUDIO_STATE_BUSY == status)
    {
        KING_Sleep(10);
        count++;
        if(count > 10)
        {
            break;
        }
        KING_AudioStatusGet(&status);
    }
    ret = KING_FsFileCreate(fileName, FS_MODE_CREATE_ALWAYS | FS_MODE_READ | FS_MODE_WRITE, 0, 0, &gFileHandle);
    if (ret == -1)
    {
        LogPrintf("open file %s fail: 0x%X\r\n", fileName, KING_GetLastErrCode());
        return ret;
    }
    else
    {
        LogPrintf("open file ret=%d\r\n", ret);
    }
    
    ret = KING_AudioStreamRecordExt(PLAY_RECORD_FORMAT, TEST_SAMPLING_RATE, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE,
                                   fileName);

    LogPrintf("KING_AudioStreamRecordExt ret = %d\r\n",ret);
    LogPrintf("Start record");
    KING_Sleep(5000);
    LogPrintf("Stop record");
    KING_AudioRecordStop();//使用该接口结束录音
    return ret;
}

int AudioPlayAmr(char * fileName)
{
    int ret = 0;
    AUDIO_STATE_E status;
    uint32 count = 0;
    uint32 rlen = 0;
    unsigned char tag[MP3_FILE_TAG_HEAD_LEN];
    int headLen = 0;
    LogPrintf("%s", __FUNCTION__);
    KING_AudioStatusGet(&status);
    if(AUDIO_STATE_BUSY == status)
    {
        AudioStopPlay();
    }
    while(AUDIO_STATE_BUSY == status)
    {
        KING_Sleep(10);
        count++;
        if(count > 10)
        {
            break;
        }
        KING_AudioStatusGet(&status);
    }
    ret = KING_FsFileCreate(fileName, FS_MODE_OPEN_EXISTING | FS_MODE_READ, 0, 0, &gFileHandle);
    if (ret == -1)
    {
        LogPrintf("open file %s fail: 0x%X\r\n", fileName, KING_GetLastErrCode());
        return ret;
    }
    else
    {
        LogPrintf("open file ret=%d\r\n", ret);
    }

    memset(tag, 0x00, MP3_FILE_TAG_HEAD_LEN);
    KING_FsFileRead(gFileHandle, tag, MP3_FILE_TAG_HEAD_LEN, &rlen);
    //如果播放的是MP3格式，先去掉文件头再调用接口播放，如果是AMR格式，直接调用接口播放
    if(rlen == MP3_FILE_TAG_HEAD_LEN)
    {
       headLen = Mp3tagHeaderDecode(tag);
       LogPrintf("headLen=%d",headLen);
       if(headLen > 0)
       {
            headLen = headLen+MP3_FILE_TAG_HEAD_LEN;
            KING_FsFilePointerSet(gFileHandle, headLen, FS_SEEK_BEGIN);
       }
       else
       {
           KING_FsFilePointerSet(gFileHandle, 0, FS_SEEK_BEGIN);
       }
    }
    ret = KING_AudioStreamPlayExt(PLAY_RECORD_FORMAT, PlayAmrCB);
    
    if(-1 == ret)
    {
        LogPrintf("Audio play fail!!%x\r\n",KING_GetLastErrCode());
    }
    return ret;
}

static void AudioPlayAmrThread(void *param)
{
    int ret = FAIL;
    AudioInit();
    KING_AudioChannelSet(AUDIO_MIC, 0);
    KING_AudioChannelSet(AUDIO_SPEAK, 1);
    KING_AudioVolumeSet(AUDIO_MIC, 90);
    KING_AudioVolumeSet(AUDIO_SPEAK, 90);
    KING_FsInit();

    ret = AudioRecordAmr(FS_TEST_FILE1);
    if (-1 == ret)
    {
        LogPrintf("Audio record fail!!\r\n");
        KING_ThreadExit();
    }
    LogPrintf("Start to Play");
    ret = AudioPlayAmr(FS_TEST_FILE1);
    if (-1 == ret)
    {
        LogPrintf("Audio play fail!!\r\n");
        KING_ThreadExit();
    }
    while(1)
    {
        KING_Sleep(3000);
    }
    //thread exit1
    KING_ThreadExit();
}


/**
 *  播放Amr
 *
 **/
void AudioPlayAmr_Test(void)
{
    int ret = SUCCESS;
    THREAD_ATTR_S threadAttr;
    LogPrintf("AudioPlayAmr_Test");
    //Thread create 
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = AudioPlayAmrThread;
    threadAttr.priority = THREAD_PRIORIT3;
    threadAttr.stackSize = 5*1024;
    ret = KING_ThreadCreate("Amrplay Test", &threadAttr, &testThreadH);
    if(ret != 0)
    {
        LogPrintf("KING_ThreadCreate() testThreadH Fail!");
    }
}

