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
  #define LogPrintf(fmt, args...) while(0)
// #define LogPrintf(fmt, args...)    do { KING_SysLog("audio: "fmt, ##args); } while(0)    
// #define FS_TEST_FILE1   "test1.mp3"
// #define FS_TEST_FILE1   "/app/system/audio/test.mp3"
// "/app/system/audio/wait_conv.mp3"
// "/app/system/audio/end_conv.mp3"
/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
    

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static FS_HANDLE mp3_gFileHandle;

extern MSG_HANDLE msgHandle_audio;       //AUDIO
extern MSG_HANDLE    socketMsgHandle;       //SOCKET
MSG_HANDLE   MP3_msgHandle;      //MP3
extern MSG_HANDLE   file_msgHandle;       //files
/*******************************************************************************
 ** Functions
 ******************************************************************************/
#define MP3_FILE_TAG_HEAD_LEN    10
#define MP3_DATA_READ_LEN       4096
#define MP3_DATA_PLAY_READ_LEN       (1 * 1152)//2048
extern uint8 audio_en_v2;    //是否全双工
static THREAD_HANDLE mp3_testThreadH = NULL;
enum Msg_Evt_MP3{
    Msg_Evt_MP3_NULL,
    Msg_Evt_MP3_AUDIO,
    Msg_Evt_MP3_STOP,
    Msg_Evt_MP3_STOP_AUDIO_START,
};
enum HMI_Aktion_Enum{   //屏幕操作
    HMI_Aktion_Enum_0 = 1,  //主拨号_呼叫       
    HMI_Aktion_Enum_1,  //主拨号_取消
    HMI_Aktion_Enum_2,  //主拨号_接通
    HMI_Aktion_Enum_3,  //主拨号_挂断
    HMI_Aktion_Enum_4,  //
    HMI_Aktion_Enum_5,   //主拨号返回_忙
    HMI_Aktion_Enum_6,   //主拨号返回_呼叫中
    HMI_Aktion_Enum_7,   //主拨号返回_被同意
    HMI_Aktion_Enum_8,   //主拨号返回_被拒绝
    HMI_Aktion_Enum_9,   //主拨号返回_已接通
    HMI_Aktion_Enum_10,   //主拨号返回_被挂断
    HMI_Aktion_Enum_11,  //
    HMI_Aktion_Enum_12,   //被拨号请求_呼叫
    HMI_Aktion_Enum_13,   //被拨号请求_取消
    HMI_Aktion_Enum_14,   //被拨号请求_已接通
    HMI_Aktion_Enum_15,   //被拨号请求_挂断
    HMI_Aktion_Enum_16,   //
    HMI_Aktion_Enum_17,   //被拨号返回_忙
    HMI_Aktion_Enum_18,   //被拨号返回_呼叫中
    HMI_Aktion_Enum_19,   //被拨号返回_统一
    HMI_Aktion_Enum_20,   //被拨号返回_拒绝
    HMI_Aktion_Enum_21,   //被拨号返回_拒绝
    HMI_Aktion_Enum_22,   //被拨号返回_接通
    HMI_Aktion_Enum_23,   //被拨号返回_挂断
    HMI_Aktion_Enum_24,   //
    HMI_Aktion_Enum_25,   //主拨号:无网络
    HMI_Aktion_Enum_26,   //巡检返回
    HMI_Aktion_Enum_27,  //巡检返回不在线
    HMI_Aktion_Enum_28,  //
    HMI_Aktion_Enum_29,  //屏幕显示通话记录
    HMI_Aktion_Enum_30,  //屏幕显示几条通话记录
    HMI_Aktion_Enum_31,  //
    HMI_Aktion_Enum_32,  //
    HMI_Aktion_Enum_33,  //
};
enum Msg_Evt
{
    MSG_EVT_BASE = 0,
    MSG_EVT_AUDIO_STOP,
    MSG_EVT_RECORD_STOP,
    MSG_EVT_RECORD,
    MSG_EVT_PLYRCD,
    MSG_EVT_PLYRCD_STOP,
    MSG_EVT_END,
    MSG_EVT_UART2_READ,
    MSG_EVT_UART3_READ,
    MSG_EVT_HMI_NEWS,
};
void AudioInit(void)
{
    int ret = 0;
    ret = KING_AudioPlayInit();
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioPlayInit fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        return;
    }
}
int AudioStopPlay(void)
{
    int ret = SUCCESS;
    
    ret = KING_AudioAbort();
    if(mp3_gFileHandle != NULL)
    {
        KING_FsCloseHandle(mp3_gFileHandle);
        mp3_gFileHandle = NULL;
    }
    return ret;
}
void PlayMP3CB(AUDIO_EVENT_E event, uint8 *buff, uint32 *len)
{
    LogPrintf("Audio Event:%d:%d\r\n", event,*len);
    uint8 mp3data[MP3_DATA_READ_LEN] = {0};
    uint32 bytesRead = 0;
    int ret = SUCCESS;
    
    switch(event)
    {
        case AUDIO_PLAY_FILL_BUFF:
        {
            if(*len < MP3_DATA_READ_LEN)
            {
                ret = KING_FsFileRead(mp3_gFileHandle, mp3data, *len, &bytesRead);
            }
            else
            {
                ret = KING_FsFileRead(mp3_gFileHandle, mp3data, MP3_DATA_READ_LEN, &bytesRead);
            }
            // LogPrintf("KING_FsFileRead ret=%d, readByte=%d\r\n", ret, bytesRead);
            
            if (FAIL == ret || bytesRead <= 0)
            {
                KING_AudioAbort();
                KING_FsCloseHandle(mp3_gFileHandle);
                mp3_gFileHandle = NULL;
                *len = 0;
                return;
            }
            
            *len = bytesRead;
            memcpy(buff, mp3data, bytesRead);
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
int CalcFrameSize(unsigned char *tag)
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
int AudioPlayMP3(char * fileName)
{
    int ret = 0;
    AUDIO_STATE_E status;
    uint32 count = 0;
    uint32 rlen = 0;
    unsigned char tag[MP3_FILE_TAG_HEAD_LEN];
    int headLen = 0;
    
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
    ret = KING_FsFileCreate(fileName, FS_MODE_OPEN_EXISTING | FS_MODE_READ, 0, 0, &mp3_gFileHandle);
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
    KING_FsFileRead(mp3_gFileHandle, tag, MP3_FILE_TAG_HEAD_LEN, &rlen);
    if(rlen == MP3_FILE_TAG_HEAD_LEN)
    {
       headLen = Mp3tagHeaderDecode(tag);
       if(headLen > 0)
       {
            headLen = headLen+MP3_FILE_TAG_HEAD_LEN;
            KING_FsFilePointerSet(mp3_gFileHandle, headLen, FS_SEEK_BEGIN);
       }
       else
       {
           KING_FsFilePointerSet(mp3_gFileHandle, 0, FS_SEEK_BEGIN);
       }
    }
    ret = KING_AudioStreamPlayExt(AUD_FORMAT_MP3, PlayMP3CB);
    if(-1 == ret)
    {
        LogPrintf("Audio play fail!!%x\r\n",KING_GetLastErrCode());
    }
    return ret;
}
void AudioPlayMP3Thread(void *param)
{
    // int ret = FAIL;
    MSG_S msg_audio_mp3;
    MSG_S msg_audio;
    AudioInit();
    while(1)
    {
        memset(&msg_audio_mp3, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg_audio_mp3, MP3_msgHandle, WAIT_OPT_INFINITE);
        LogPrintf("Audio play Msg_ID:%d,data:%s",msg_audio_mp3.messageID,msg_audio_mp3.pData);
        switch (msg_audio_mp3.messageID)
        {
            case Msg_Evt_MP3_NULL:{
                break;
            }
            case Msg_Evt_MP3_AUDIO:{
                if(audio_en_v2){
                    KING_AudioDevStopPocMode();
                    KING_AudioStop();
                    audio_en_v2=0;  
                }else{
                    KING_AudioStop();
                    KING_FsCloseHandle(mp3_gFileHandle);
                    AudioPlayMP3(msg_audio_mp3.pData);
                }

                break;
            }
            case Msg_Evt_MP3_STOP:{
                if(audio_en_v2){
                    KING_AudioDevStopPocMode();
                    KING_AudioStop();
                    audio_en_v2=0;  
                }else{
                    KING_AudioStop();
                    KING_FsCloseHandle(mp3_gFileHandle);
                    AudioPlayMP3(msg_audio_mp3.pData);
                }
                break;
            }
            case Msg_Evt_MP3_STOP_AUDIO_START:{
                if(!audio_en_v2){
                    KING_AudioStop();
                    KING_FsCloseHandle(mp3_gFileHandle);                
                }

                memset(&msg_audio, 0x00, sizeof(MSG_S));
                msg_audio.messageID = MSG_EVT_HMI_NEWS;
                // msg_audio.pData = pcmdata;
                // msg_audio.DataLen = HMI_Aktion_Enum_14;
                KING_MsgSend(&msg_audio, msgHandle_audio);
                break;
            }
            default:
                break;
        }
    }
    //thread exit
    LogPrintf("Audio play fail...end\r\n");
    KING_ThreadExit();
}
void AudioPlayMP3_init()
{
    int ret = SUCCESS;
    // uint32 j;
    LogPrintf("AudioPlayMP3_Test");

    THREAD_ATTR_S threadAttr;
    KING_MsgCreate(&MP3_msgHandle);
        //Thread create
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = AudioPlayMP3Thread;
    threadAttr.priority = THREAD_PRIORIT3;
    threadAttr.stackSize = 5*1024;
    threadAttr.queueNum = 64;
    ret = KING_ThreadCreate("mp3play Test", &threadAttr, &mp3_testThreadH);
    if(ret != 0)
    {
        LogPrintf("KING_ThreadCreate() mp3_testThreadH Fail!");
    }
}

// memset(&msg_audio_mp3, 0x00, sizeof(MSG_S));
// msg_audio_mp3.messageID = Msg_Evt_MP3_AUDIO;
// msg_audio_mp3.pData=(char*)"/app/system/audio/test.mp3";
// KING_MsgSend(&msg_audio_mp3, MP3_msgHandle);


		// {
		// 	"file": "/app/system/audio/wait_conv.mp3",
		// 	"local_file": "mini.mp3"
		// },
		// {
		// 	"file": "/app/system/audio/end_conv.mp3",
		// 	"local_file": "mini2.mp3"
		// },
        // {
		// 	"file": "/app/system/audio/no_reply.mp3",   
		// 	"local_file": "no_reply.mp3"
		// },
        // {
		// 	"file": "/app/system/audio/tim60_end.mp3",   
		// 	"local_file": "tim60_end.mp3"
		// },
        // {
		// 	"file": "/app/system/audio/busy.mp3",   
		// 	"local_file": "busy.mp3"
		// }