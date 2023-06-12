#include "KingAudio.h"
#include "KingRTC.h"
#include "KingFileSystem.h"
#include "KingRtos.h"
#include "KingPlat.h"
#include "KingTts.h"
#include "at_common.h"
#include "at_module.h"
#include "at_errcode.h"
#include "at_cmd_audio.h"

#define AUD_LOG(fmt, ...) \
    do \
    { \
        KING_SysLog("[AT AUD][%s:%d]"#fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)


#define AT_CZAUDPLAY_FILE_PATH  "/mnt/sd/pcm1.img"
#define AT_CZAUDPLAY_FILECNT_MIN       1
#define AT_CZAUDPLAY_FILECNT_MAX       20
#define AT_CZAUDPLAY_FILECNT_DEFAULT   1
#define AT_CZAUDPLAY_FILE_NAME_MAX_LEN 17

#define AT_CZFPCM_INDEX_MIN   0
#define AT_CZFPCM_INDEX_MAX   1535
#define AT_CZFPCM_LEN_MIN     1
#define AT_CZFPCM_LEN_MAX     2048
#define AT_CZFPCM_CRC_MIN     0
#define AT_CZFPCM_CRC_MAX     255
#define PCM_DATA_BUF_SIZE         (2 * 1024)
#define CZFPCM_TIME_OUT           1 /* Unit: s */
#define AUDPLAY_PCM_IMAGE_HEADER_SIZE (2 * 1024)
#define AUDPLAY_PCM_DATA_BUF_SIZE   (6 * 1024)
#define AUDPLAY_MAX_PCM_DATA_READ     640

#define AUDPLAY_PCM_DATA_IDLE           0x00
#define AUDPLAY_PCM_DATA_CPYING         0x01
#define AUDPLAY_PCM_DATA_ALL_READY      0x02
#define AUDPLAY_PCM_DATA_FINISH_ERR     0x03



static UINT8 audInPlaying = 0;
static char *gFileName[AT_CZAUDPLAY_FILECNT_MAX];
static UINT8 gFileNum = 0;
static uint8 gPlayFlag = 0;
static UINT8 gAudplayPcmImageHeader[AUDPLAY_PCM_IMAGE_HEADER_SIZE] = {0};
static MSG_HANDLE MsgAudplay = NULL;
static THREAD_HANDLE threadAudioPlay = NULL;
static MUTEX_HANDLE playMutex = NULL;
static CiAtRingBuf AtRingBuf;
static UINT8 gPcmDataBuf[AUDPLAY_PCM_DATA_BUF_SIZE];
static UINT8 BufStatus = AUDPLAY_PCM_DATA_IDLE;
static BOOL   IsPcmDataInWrite = FALSE;
static UINT16 PCMResIndex;
static UINT8  PCMResLastPkg;
static UINT16 PCMResLen;
static UINT8  PCMResCrc;
static UINT8  *g_pPcmDataBuf = NULL;
static AT_CMD_ENGINE_T * Czfpcm_atHandle = NULL;
BOOL bPCMResUpdateEntryModeOn;
TIMER_HANDLE hTimer = NULL;
static FS_HANDLE gFd = NULL;

void AudPlayStart(void)
{
    audInPlaying = 1;
}

void AudPlayEnd(void)
{
    audInPlaying = 0;
}

UINT8 IsAudPlaying(void)
{
    return audInPlaying;
}

UINT8 IsFileExist( char *filename )
{
    INFO_TABLE *pPcmFileInfo;

    pPcmFileInfo  = (INFO_TABLE *)gAudplayPcmImageHeader;
    do
    {
        if (strcmp(pPcmFileInfo->Name, filename) == 0 )
        {
            return TRUE;
        }
        pPcmFileInfo++;
    } while ( (strlen(pPcmFileInfo->Name) != 0) && (pPcmFileInfo->Offset != 0) && (pPcmFileInfo->Size != 0) );

    return FALSE;
}

UINT32 AtRingBufSpace(CiAtRingBuf *pRingBuf)
{
    return (pRingBuf->size - pRingBuf->count);
}

UINT32 AtRingBufIn(CiAtRingBuf *pRingBuf, CHAR *pData, UINT32 len)
{
    UINT32 actual_size;
    UINT32 space = 0;
    UINT32 i;

    space = AtRingBufSpace(pRingBuf);
    actual_size = (space >= len) ? len : space;

    for(i = 0; i < actual_size; i++)
    {
        pRingBuf->bufPtr[pRingBuf->inPoint++] = pData[i];
        if(pRingBuf->inPoint >= pRingBuf->size)
        {
            pRingBuf->inPoint = 0;
        }
    }

    KING_MutexLock(pRingBuf->mutex, WAIT_OPT_INFINITE);
    pRingBuf->count += actual_size;
    KING_MutexUnLock(pRingBuf->mutex);
    return actual_size;
}

UINT32 AtRingBufOut(CiAtRingBuf *pRingBuf, CHAR *pBuf, UINT32 len)
{
    UINT32 actual_size = 0;
    UINT32 data_size = 0;
    UINT32 i;

    data_size = pRingBuf->count;
    actual_size = (data_size > len) ? len : data_size;
    for(i = 0; i < actual_size; i++)
    {
        pBuf[i] = pRingBuf->bufPtr[pRingBuf->outPoint++];
        if(pRingBuf->outPoint >= pRingBuf->size)
        {
            pRingBuf->outPoint = 0;
        }
    }

    KING_MutexLock(pRingBuf->mutex, WAIT_OPT_INFINITE);
    pRingBuf->count -= actual_size;
    KING_MutexUnLock(pRingBuf->mutex);

    return actual_size;
}


static void PlayCallback(AUDIO_EVENT_E event, uint8 *buff, uint32 *len)
{
    UINT16 actual_size = 0;
    static UINT16 count = 0;

    AUD_LOG("event=%d, buff=0x%x, len=%d",event, buff, *len);
    if(AUDIO_PLAY_FILL_BUFF == event)
    {
        if((buff != NULL) && (*len > 0))
        {
            count = 0;
            while(0 == actual_size)
            {
                memset(buff, 0x00, *len);
                actual_size = AtRingBufOut(&AtRingBuf, (char*)buff, *len);
                if(0 == actual_size)
                {
                    KING_Sleep(40);
                    count++;
                }
                if(count > 3)
                {
                    if((AUDPLAY_PCM_DATA_ALL_READY == BufStatus) || (AUDPLAY_PCM_DATA_FINISH_ERR == BufStatus))
                    {
                        AUD_LOG("data finished, stop audio");
                        KING_AudioStop();
                    }
                    else
                    {
                        gPlayFlag = 0;
                    }
                    break;
                }
            }
            AUD_LOG("actual_size=%d",actual_size);
            *len = actual_size;
        }
    }
    else if(AUDIO_PLAY_STOP == event)
    {
        AUD_LOG("AudPlayEnd");
        AudPlayEnd();
        // at urc
        if(AUDPLAY_PCM_DATA_ALL_READY == BufStatus)
        {
            at_CmdRespUrcText(Czfpcm_atHandle, (const uint8*)"+CZAUDPLAYIND: 0,5");
        }
        else
        {
            at_CmdRespUrcText(Czfpcm_atHandle, (const uint8*)"+CZAUDPLAYIND: 0,6");
        }

        BufStatus = AUDPLAY_PCM_DATA_IDLE;
        gPlayFlag = 0;
    }
}


static void AudPlayToPcm(AudPlayParserMsg *pPlayMsg)
{
    INFO_TABLE *pPcmFileInfo;
    UINT32 PcmDataPtr;
    UINT16 PcmDataSize;
    UINT8 PcmDataBuf[AUDPLAY_MAX_PCM_DATA_READ];
    UINT16 BufSpace;
    UINT8 Index;
    FS_HANDLE fd = NULL;
    int ret = FAIL;
    uint32 bytesRead = 0;
    static uint8 flag = 0;

    ret = KING_FsFileCreate(AT_CZAUDPLAY_FILE_PATH, FS_MODE_OPEN_EXISTING | FS_MODE_READ | FS_MODE_WRITE, 0, FS_ATTR_NONE, &fd);
    if (ret < 0)
    {
        AUD_LOG("open AT_CZAUDPLAY_FILE_PATH fail");

        BufStatus = AUDPLAY_PCM_DATA_IDLE;
        gPlayFlag = 0;
        AUD_LOG("AudPlay Error");
        AudPlayEnd();
        // at urc
        at_CmdRespUrcText(Czfpcm_atHandle, (const uint8*)"+CZAUDPLAYIND: 0,9");

        return;
    }

    for (Index = 0; Index < pPlayMsg->filenum; Index++ )
    {
        flag = 0;
        pPcmFileInfo = (INFO_TABLE *)gAudplayPcmImageHeader;
        BufStatus = AUDPLAY_PCM_DATA_CPYING;

        AUD_LOG("read file: %s", pPlayMsg->file[Index]);
        do {
            if (strcmp(pPcmFileInfo->Name, pPlayMsg->file[Index]) == 0)
            {
                PcmDataPtr = 0;
                do {
                    if ((pPcmFileInfo->Size - PcmDataPtr) > AUDPLAY_MAX_PCM_DATA_READ)
                    {
                        PcmDataSize = AUDPLAY_MAX_PCM_DATA_READ;
                    }
                    else
                    {
                        PcmDataSize = pPcmFileInfo->Size - PcmDataPtr;
                    }

                    memset(PcmDataBuf, 0x00, AUDPLAY_MAX_PCM_DATA_READ);

                    AUD_LOG("flag=%d, Offset=%d", flag, pPcmFileInfo->Offset);
                    if (0 == flag)
                    {
                        flag = 1;
                        KING_FsFilePointerSet(fd, pPcmFileInfo->Offset, FS_SEEK_BEGIN);
                    }
                    ret = KING_FsFileRead(fd, PcmDataBuf, PcmDataSize, &bytesRead);
                    if (ret < 0)
                    {
                        AUD_LOG("read file fail");
                        goto ERROR;
                    }

                    BufSpace = AtRingBufSpace(&AtRingBuf);
                    while (BufSpace < bytesRead)
                    {
                        KING_Sleep(10);
                        BufSpace = AtRingBufSpace(&AtRingBuf);
                    }

                    AtRingBufIn(&AtRingBuf, (char*)PcmDataBuf, bytesRead);
                    PcmDataPtr += bytesRead;
                    if (0 == gPlayFlag)
                    {
                        gPlayFlag = 1;
                        AUD_LOG("KING_AudioPcmStreamPlay");
                        KING_AudioPcmStreamPlay(SAMPLING_RATE_16KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, PlayCallback);
                    }
                }while (PcmDataPtr < pPcmFileInfo->Size);
                break;
            }
            pPcmFileInfo++;
        } while ((strlen(pPcmFileInfo->Name) != 0) && (pPcmFileInfo->Offset != 0) && (pPcmFileInfo->Size != 0));

        free(pPlayMsg->file[Index]);
        pPlayMsg->file[Index] = NULL;
    }

    KING_FsCloseHandle(fd);
    BufStatus = AUDPLAY_PCM_DATA_ALL_READY;
    AUD_LOG("read files end");

    return;

ERROR:
    for (Index = 0; Index < pPlayMsg->filenum; Index++)
    {
        if (NULL != pPlayMsg->file[Index])
        {
            free(pPlayMsg->file[Index]);
            pPlayMsg->file[Index] = NULL;
        }
    }
    KING_FsCloseHandle(fd);
    BufStatus = AUDPLAY_PCM_DATA_FINISH_ERR;
    AUD_LOG("read files end with error");
}



void AudPlayTask(void* argv)
{
    AudPlayParserMsg audPlayMsg;
    MSG_S msg;
    int ret = FAIL;

    while( 1 )
    {
        memset(&msg, 0x00, sizeof(msg));
        memset(&audPlayMsg, 0x00, sizeof(audPlayMsg));
        ret = KING_MsgRcv(&msg, MsgAudplay, WAIT_OPT_INFINITE);
        if(ret != SUCCESS )
        {
            AUD_LOG("KING_MsgRcv fail");
        }
        AUD_LOG("messageID=%d", msg.messageID);
        switch (msg.messageID)
        {
            case MSG_ID_PLAY_START:
            {
                audPlayMsg.file = gFileName;
                audPlayMsg.filenum = gFileNum;

                AUD_LOG("filenum=%d", audPlayMsg.filenum);
                KING_MutexLock(playMutex, WAIT_OPT_INFINITE);
                BufStatus = AUDPLAY_PCM_DATA_IDLE;
                AudPlayToPcm(&audPlayMsg);
                KING_MutexUnLock(playMutex);
            }
            break;

            default:
                break;
        }

        KING_AudioAbort();
    }
}

void AtRingBufInit(CiAtRingBuf *pRingBuf, CHAR* pBuf, UINT32 size)
{
  int ret = FAIL;

  pRingBuf->bufPtr = pBuf;
  pRingBuf->size = size;
  pRingBuf->inPoint = 0;
  pRingBuf->outPoint = 0;
  pRingBuf->count = 0;
  ret = KING_MutexCreate("ringbuf mutex", 0, &(pRingBuf->mutex));
  if (ret != SUCCESS )
  {
      AUD_LOG("KING_MutexCreate fail");
      return;
  }
}


void AudplayInit(void)
{
    int ret = FAIL;
    THREAD_ATTR_S attr;
    FS_HANDLE fd = NULL;
    uint32 bytesRead = 0;

    ret = KING_FSMount(STORAGE_TYPE_SD, 0, 0, 0, 0);
    if(FAIL == ret)
    {
        AUD_LOG("KING_FSMount fail = %d", KING_GetLastErrCode());
    }
    /*creat msgq*/
    ret = KING_MsgCreate(&MsgAudplay);
    if(FAIL == ret)
    {
        PRD_LOG("KING_MsgCreate fail");
        return;
    }

    KING_AudioPlayInit();
    /*creat task*/
    memset(&attr, 0x00, sizeof(attr));
    attr.fun = AudPlayTask;
    attr.priority = THREAD_PRIORIT1;
    attr.stackSize = 4096;

    ret = KING_ThreadCreate("AudPlayTask", &attr, &threadAudioPlay);
    if (ret != SUCCESS )
    {
        PRD_LOG("KING_ThreadCreate fail");
        return;
    }

    ret = KING_MutexCreate("pcmplay mutex", 0, &playMutex);
    if (ret != SUCCESS )
    {
        PRD_LOG("KING_MutexCreate fail");
        return;
    }

    ret = KING_FsFileCreate(AT_CZAUDPLAY_FILE_PATH, FS_MODE_OPEN_EXISTING | FS_MODE_READ | FS_MODE_WRITE, 0, FS_ATTR_NONE, &fd);
    if (ret < 0)
    {
        AUD_LOG("open AT_CZAUDPLAY_FILE_PATH fail");
    }

    KING_FsFileRead(fd, gAudplayPcmImageHeader, AUDPLAY_PCM_IMAGE_HEADER_SIZE, &bytesRead);
    AUD_LOG("read bytesRead=%d", bytesRead);
    KING_FsCloseHandle(fd);
    AtRingBufInit(&AtRingBuf, (char*)gPcmDataBuf, AUDPLAY_PCM_DATA_BUF_SIZE);
}



void CZAudPlaySndMsgToTask(char **file_name, UINT8 file_cnt)
{
    MSG_S msg;
    int ret = FAIL;

    memset(&msg, 0x00, sizeof(msg));
    msg.messageID = MSG_ID_PLAY_START;
    msg.pData = NULL;
    msg.DataLen = 0;
    gFileNum = file_cnt;
    ret = KING_MsgSend(&msg, MsgAudplay);
    if (ret != SUCCESS)
    {
        AudPlayEnd();
    }
}


void AT_CmdFunc_CLVL(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_SET)
    {
        // +CLVL=<level>
        bool paramok = true;
        uint8 temp = 0;
        uint8 level = at_ParamIntInRange(pParam->params[0], 0, 5, &paramok);
        if (!paramok || pParam->paramCount > 1)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }
        temp = level*20;
        AUD_LOG("temp=%d", temp);
        KING_AudioVolumeSet(AUDIO_SPEAK, temp);
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, (const uint8_t*)"+CLVL: (0-5)");
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        uint32 level = 0;
        uint32 temp = 0;
        char rsp[32];

        KING_AudioVolumeGet(AUDIO_SPEAK, &level);
        temp = level/20;
        AUD_LOG("level=%d", level);
        sprintf(rsp, "+CLVL: %d", temp);
        at_CmdRespInfoText(pParam->engine, (const uint8_t*)rsp);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

static void RingBufRaraInit(void)
{
    AtRingBuf.size = AUDPLAY_PCM_DATA_BUF_SIZE;
    AtRingBuf.inPoint = 0;
    AtRingBuf.outPoint = 0;
    AtRingBuf.outPoint = 0;
    AtRingBuf.count = 0;
    memset(AtRingBuf.bufPtr, 0x00, AUDPLAY_PCM_DATA_BUF_SIZE);
}

void AT_CmdFunc_CZAudPlay(AT_CMD_PARA *pParam)
{
    UINT8 file_i;
    UINT8 file_cnt = 0;
    char *str = NULL;
    UINT8 len;
    bool paramok = true;
    AUDIO_STATE_E state = AUDIO_STATE_MAX;

    Czfpcm_atHandle  = pParam->engine;
    if (pParam->iType == AT_CMD_SET)
    {
        // +CZAUDPLAY==<file_count>,"filename0",бн
        file_cnt = at_ParamIntInRange(pParam->params[0], AT_CZAUDPLAY_FILECNT_MIN, AT_CZAUDPLAY_FILECNT_MAX, &paramok);
        if (!paramok)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        if (pParam->paramCount != (file_cnt + 1))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        KING_AudioStatusGet(&state);
        if((!IsTtsStop()) || IsAudPlaying() || (AUDIO_STATE_IDLE != state))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }

        for (file_i = 0; file_i < file_cnt; file_i++)
        {
            str = (char*)at_ParamStr(pParam->params[file_i+1], &paramok);
            AUD_LOG("file_i=%d, paramok=%d", file_i, paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            len = strlen((char*)str);
            if ((len > AT_CZAUDPLAY_FILE_NAME_MAX_LEN) || (0 == len) || !IsFileExist(str))
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }

            gFileName[file_i] = malloc(AT_CZAUDPLAY_FILE_NAME_MAX_LEN);
            if(NULL == gFileName[file_i])
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                return;
            }
            memset(gFileName[file_i], 0x00, AT_CZAUDPLAY_FILE_NAME_MAX_LEN);
            strcpy(gFileName[file_i], (char*)str);

            AUD_LOG("gFileName[%d]=%s", file_i, gFileName[file_i]);
        }

        AudPlayStart();
        RingBufRaraInit();
        CZAudPlaySndMsgToTask(gFileName, file_cnt);
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, (const uint8_t*)"+CZAUDPLAY: (1-20),\"filename\",...");
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

//receive uart pcm data
void ProcessPCMResData(char *buf, int len )
{
    static UINT16 LenCnt = 0;
    static UINT32 DataBufLen = 0;
    UINT8  CrcSum;
    UINT16 i;
    uint32 byteswritten = 0;
    int ret = FAIL;

    memcpy(&g_pPcmDataBuf[LenCnt], buf, len);
    LenCnt     += len;
    DataBufLen += len;
    if ( LenCnt >= PCMResLen )
    {
        LenCnt = 0;
        KING_TimerDeactive(hTimer);
        KING_TimerDelete(hTimer);
        hTimer = NULL;
        at_CmdRespOK(Czfpcm_atHandle);

        CrcSum = 0;
        for ( i = 0; i < PCMResLen; i++ )
        {
            CrcSum ^= g_pPcmDataBuf[i];
        }
        if ( CrcSum != PCMResCrc )
        {
            at_CmdRespInfoText(Czfpcm_atHandle, (const uint8_t*)"+CZFPCMIND: 1");
            goto BUF_FREE;
        }

        if ( PCMResLen % 4 )
        {
            UINT8 i;
            for (i = 0; i < 4 - PCMResLen % 4; i++)
            {
                g_pPcmDataBuf[PCMResLen + i] = 0xFF;
            }
            PCMResLen += i;
        }

        KING_FsFilePointerSet(gFd, PCMResIndex * PCM_DATA_BUF_SIZE, FS_SEEK_BEGIN);
        ret = KING_FsFileWrite(gFd, g_pPcmDataBuf, PCMResLen, &byteswritten);
        if ( SUCCESS == ret )
        {
            at_CmdRespInfoText(Czfpcm_atHandle, (const uint8_t*)"+CZFPCMIND: 0");
        }
        else
        {
            at_CmdRespInfoText(Czfpcm_atHandle, (const uint8_t*)"+CZFPCMIND: 2");
        }
BUF_FREE:
        IsPcmDataInWrite = FALSE;
        bPCMResUpdateEntryModeOn = FALSE;
        if(gFd != NULL)
        {
            KING_FsCloseHandle(gFd);
            gFd = NULL;
        }
    }
}




static void CzfPcmTimeOut(uint32 tmrId)
{

    IsPcmDataInWrite = FALSE;
    bPCMResUpdateEntryModeOn = FALSE;
    at_CmdRespCmeError(Czfpcm_atHandle, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    if(gFd != NULL)
    {
        KING_FsCloseHandle(gFd);
        gFd = NULL;
    }
}


void AT_CmdFunc_CZFPCM(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    TIMER_ATTR_S timerAttr;
    int ret = FAIL;

    Czfpcm_atHandle  = pParam->engine;
    if (pParam->iType == AT_CMD_SET)
    {
        // +CZFPCM=<index>,<last_pkg>,<len>,<crc>
        if ( TRUE == IsPcmDataInWrite)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }
        PCMResIndex = at_ParamIntInRange(pParam->params[0], AT_CZFPCM_INDEX_MIN, AT_CZFPCM_INDEX_MAX, &paramok);
        if (!paramok)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        PCMResLastPkg = at_ParamIntInRange(pParam->params[1], 0, 1, &paramok);
        if (!paramok)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        PCMResLen = at_ParamIntInRange(pParam->params[2], AT_CZFPCM_LEN_MIN, AT_CZFPCM_LEN_MAX, &paramok);
        if (!paramok)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        if ( (!PCMResLastPkg) && (PCMResLen != AT_CZFPCM_LEN_MAX) )
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        PCMResCrc = at_ParamIntInRange(pParam->params[3], AT_CZFPCM_CRC_MIN, AT_CZFPCM_CRC_MAX, &paramok);
        if (!paramok)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        if ( NULL == g_pPcmDataBuf )
        {
            g_pPcmDataBuf = (UINT8 *)malloc(PCM_DATA_BUF_SIZE);
            if ( NULL == g_pPcmDataBuf )
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
        }

        IsPcmDataInWrite = TRUE;
        bPCMResUpdateEntryModeOn = TRUE;
        at_CmdRespInfoText(pParam->engine, (const uint8_t*)"> ");


        KING_TimerCreate(&hTimer);
        memset(&timerAttr, 0x00, sizeof(timerAttr));
        timerAttr.isPeriod = 0;
        timerAttr.timeout = CZFPCM_TIME_OUT*1000;
        timerAttr.timer_fun = CzfPcmTimeOut;
        KING_TimerActive(hTimer, &timerAttr);
        ret = KING_FsFileCreate(AT_CZAUDPLAY_FILE_PATH, FS_MODE_OPEN_EXISTING | FS_MODE_READ | FS_MODE_WRITE, 0, FS_ATTR_NONE, &gFd);
        if(FAIL == ret)
        {
            AUD_LOG("KING_FsFileCreate fail");
        }
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, (const uint8_t*)"+CZFPCM: (0-1535),(0,1),(1-2048),(0-255)");
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}


void czUtl_ConvertBinToHex(UINT8 *bin_ptr, UINT16 length, UINT8 *hex_ptr, UINT8 charcase)
{
  UINT8 semi_octet;
  INT32 i;

  if((NULL == bin_ptr) || (NULL == hex_ptr))
  {
    return;
  }
  for(i = 0; i < length; i++)
  {
    // get the high 4 bits
    semi_octet = (UINT8)((bin_ptr[i] & 0xF0) >> 4);

    if(semi_octet <= 9)  //semi_octet >= 0
    {
      *hex_ptr = (UINT8)(semi_octet + '0');
    }
    else
    {
        if (0 == charcase)
        {
            *hex_ptr = (UINT8)(semi_octet + 'A' - 10);
        }
        else
        {
            *hex_ptr = (UINT8)(semi_octet + 'a' - 10);
        }
    }

    hex_ptr++;

    // get the low 4 bits
    semi_octet = (UINT8)(bin_ptr[i] & 0x0f);

    if(semi_octet <= 9)  // semi_octet >= 0
    {
      *hex_ptr = (UINT8)(semi_octet + '0');
    }
    else
    {
        if (0 == charcase)
        {
            *hex_ptr = (UINT8)(semi_octet + 'A' - 10);
        }
        else
        {
            *hex_ptr = (UINT8)(semi_octet + 'a' - 10);
        }
    }

    hex_ptr++;
  }
}


char *GetPcmVersion(char *filename)
{
    INFO_TABLE *pPcmFileInfo;
    UINT8 *pPcmVerHex = NULL;
    char *pPcmVerStr = NULL;
    FS_HANDLE fd = NULL;
    int ret = FAIL;
    uint32 bytesRead = 0;

    pPcmFileInfo = (INFO_TABLE *)gAudplayPcmImageHeader;
    ret = KING_FsFileCreate(AT_CZAUDPLAY_FILE_PATH, FS_MODE_OPEN_EXISTING | FS_MODE_READ | FS_MODE_WRITE, 0, FS_ATTR_NONE, &fd);
    if(FAIL == ret)
    {
        AUD_LOG("KING_FsFileCreate fail");
        return NULL;
    }

    do
    {
        if (strcmp(pPcmFileInfo->Name, filename) == 0 )
        {
            pPcmVerHex = (UINT8 *)malloc(pPcmFileInfo->Size);
            pPcmVerStr = malloc(pPcmFileInfo->Size * 2 + 1);
            if ( NULL == pPcmVerStr || NULL == pPcmVerHex)
            {
                break;
            }
            memset(pPcmVerStr, 0x00, pPcmFileInfo->Size * 2 + 1);
            KING_FsFilePointerSet(fd, pPcmFileInfo->Offset, FS_SEEK_BEGIN);
            ret = KING_FsFileRead(fd, pPcmVerHex, pPcmFileInfo->Size, &bytesRead);
            czUtl_ConvertBinToHex(pPcmVerHex, pPcmFileInfo->Size, (uint8*)pPcmVerStr, 0);
            break;
        }
        pPcmFileInfo++;
    } while ((strlen(pPcmFileInfo->Name) != 0) && (pPcmFileInfo->Offset != 0) && (pPcmFileInfo->Size != 0));

    if ( pPcmVerHex != NULL )
    {
        free(pPcmVerHex);
    }
    KING_FsCloseHandle(fd);
    return pPcmVerStr;
}

void AT_CmdFunc_CZGetPcmVer(AT_CMD_PARA *pParam)
{
    char *filename = NULL;
    uint32 len;
    bool paramok = true;
    char *pPcmVerStr = NULL;
    char *pResBuf = NULL;
    char temp[1024+1] = {0};
    char *pdata = NULL;
    uint32 rspLen = 0;

    if (pParam->iType == AT_CMD_SET)
    {
        // +CZGETPCMVER="filename"
        filename = (char*)at_ParamStr(pParam->params[0], &paramok);
        if (!paramok)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }
        len = strlen(filename);
        if((len > AT_CZAUDPLAY_FILE_NAME_MAX_LEN) || (0 == len) || !IsFileExist(filename))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }
        pPcmVerStr = GetPcmVersion(filename);
        if (pPcmVerStr != NULL )
        {
            len = strlen(pPcmVerStr) + strlen("+CZGETPCMVER: ");
            pResBuf = (char *)malloc(len+1);
            if (pResBuf != NULL )
            {
                memset(pResBuf, 0x00, len+1);
                sprintf(pResBuf,"+CZGETPCMVER: %s",pPcmVerStr);
                len = strlen(pResBuf);
                pdata = pResBuf;
                while(len > 0)
                {
                    memset(temp, 0x00, sizeof(temp));
                    if(len > 1024)
                    {
                        memcpy(temp, pdata, 1024);
                        pdata = pdata+1024;
                        len = len-1024;
                        rspLen = 1024;
                    }
                    else
                    {
                        memcpy(temp, pdata, len);
                        pdata = pdata+len;
                        rspLen = len;
                        len = 0;
                    }
                    at_CmdRespOutputNText(pParam->engine, (const uint8_t*)temp, rspLen);
                }
                pdata = NULL;
                at_CmdRespOK(pParam->engine);
                free(pResBuf);
                pResBuf = NULL;
                free(pPcmVerStr);
                pPcmVerStr = NULL;
                return;
            }
            free(pPcmVerStr);
            pPcmVerStr = NULL;
        }
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, (const uint8_t*)"+CZGETPCMVER: \"filename\"");
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}

