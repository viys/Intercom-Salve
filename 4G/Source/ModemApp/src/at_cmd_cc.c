#include "At_common.h"
#include "At_errcode.h"
#include "At_module.h"

#include "kingDef.h"
#include "kingPlat.h"
#include "kingrtos.h"
#include "kingusim.h"
#include "KingDualsim.h"
#include "kingcbdata.h"
#include "kingerrcode.h"
#include "kingtts.h"
#include "kingaudio.h"
#include "kingfilesystem.h"
typedef enum
{
    CC_STATE_IDLE,
    CC_STATE_INCOMING,
    CC_STATE_ORIGINATING,
    CC_STATE_ALERTING,
    CC_STATE_CONNECTED,
    CC_STATE_WAITING,
    CC_STATE_ON_HOLD,
}CC_STATE;

#define AT_GPRS_DIALSTR1 "T*98"
#define AT_GPRS_DIALSTR2 "T*99"
#define AT_GPRS_DIALSTR3 "t*98"
#define AT_GPRS_DIALSTR4 "t*99"
#define AT_GPRS_DIALSTR5 "*98"
#define AT_GPRS_DIALSTR6 "*99"

#define CALL_MAX_NUM  7

#define FILEPATH_LEN_MAX 100
#define TTS_LEN_MAX    500
#define DEFAULT_PCM_FILE "/preset/MI.pcm"
static CC_STATE CallState[CALL_MAX_NUM] = {0};
static TIMER_HANDLE timerHandle = 0;
static int RingCount = 0;
static int simid = SIM_0;

static FS_HANDLE s_filehandle = NULL ;
static int s_ring_func = 0;
char g_playpath[FILEPATH_LEN_MAX + 1] = { 0 };
char g_ttsbuff[TTS_LEN_MAX + 1] = { 0 };

static void Czcrmode_Endring_Handle(void);
static void Czcrmode_incoming_Handle(char * number);

static void AudioCb(AUDIO_EVENT_E event, uint8 *buff, uint32 *len)
{
    int ret = SUCCESS;
    uint8 pcmdata[640] = {0};
    uint32 bytesRead = 0;

    switch(event)
    {
        case AUDIO_PLAY_FILL_BUFF:
        {
            ret = KING_FsFileRead(s_filehandle, pcmdata, 320, &bytesRead);
            if (FAIL == ret)
            {
                KING_SysLog("KING_FsFileRead fail errorcode: 0x%X\r\n", KING_GetLastErrCode());
                KING_FsCloseHandle(s_filehandle);
                s_filehandle = NULL;
                KING_AudioStop();
                return;
            }
            memcpy(buff, pcmdata, bytesRead);
            *len = bytesRead;
            if(bytesRead <= 0)
            {
                KING_FsFilePointerSet(s_filehandle, 0, FS_SEEK_BEGIN);
                KING_SysLog("LOOP\r\n");
            }
            break;
        }
        case AUDIO_PLAY_STOP:
        {
            KING_SysLog("AUDIO_PLAY_STOP!\r\n");
            break;
        }

        default:
        {
            break;
        }
    }
}

static CC_STATE GetCallState()
{
    int num = 0;

    for (int i=0;i<CALL_MAX_NUM;i++)
    {
        if (CallState[i] == CC_STATE_ORIGINATING ||
            CallState[i] == CC_STATE_ALERTING)
        {
            return CallState[i];
        }
        else if(CallState[i] == CC_STATE_INCOMING ||
            CallState[i] == CC_STATE_WAITING)
        {
            return CC_STATE_INCOMING;
        }
        else if (CallState[i] == CC_STATE_CONNECTED||
            CallState[i] == CC_STATE_ON_HOLD)
        {
            num++;
        }
    }

    if (num > 0)
    {
        return CC_STATE_CONNECTED;
    }

    return CC_STATE_IDLE;
}

static void ShowRing()
{
    char strRsp[10] = {0};

    if (CC_STATE_INCOMING == GetCallState())
    {
        strcpy(strRsp,"RING");
        at_CmdReportUrcText(0,(const uint8_t*)strRsp);
        RingCount++;

        if (RingCount == gATCurrentcc_s0 && gATCurrentcc_s0 > 0)
        {
            KING_SysLog("Deactive Ring timer and auto answer!\r\n");
            KING_TimerDeactive(timerHandle);
            RingCount = 0;
            Czcrmode_Endring_Handle();
            KING_CallAnswer(simid);
        }
    }
    else
    {
        KING_SysLog("Deactive Ring timer!\r\n");
        KING_TimerDeactive(timerHandle);
        RingCount = 0;
    }
}

void Ringtimer(void)
{
    int ret = -1;
    TIMER_ATTR_S timerattr;

    ShowRing();

    if (timerHandle == NULL)
    {
        ret = KING_TimerCreate(&timerHandle);
        if(-1 == ret)
        {
            KING_SysLog("RingTimer Create() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
            return;
        }
    }

    memset(&timerattr, 0x00, sizeof(timerattr));
    timerattr.isPeriod= TRUE;
    timerattr.timeout = 4000;
    timerattr.timer_fun = ShowRing;
    ret = KING_TimerActive(timerHandle, &timerattr);
    if(-1 == ret)
    {
        KING_SysLog("RingTimer Active() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    KING_SysLog("Create Ring timer!\r\n");
}

static void ShowNoCarrier()
{
    char strRsp[20] = {0};

    strcpy(strRsp,"NO CARRIER");
    at_CmdReportUrcText(0,(const uint8_t*)strRsp);
}

static void CCEventCb(uint32 eventID, void* pdata, uint32 len)
{
    KH_CB_DATA_U* data = pdata;
    SIM_ID work_sim = SIM_0;

    KING_DualSimCurrent(&work_sim);

    KING_SysLog("at cc eventID=%d, pdata=%p\r\n", eventID, len);
    switch (eventID)
    {
        case CC_EVENT_INCOMING:
            KING_SysLog("CC_EVENT_INCOMING call id %d number is %s",
                                data->CCEvent.callID,data->CCEvent.pNumber);
            CallState[data->CCEvent.callID] = CC_STATE_INCOMING;
            simid = data->CCEvent.simID;

            Czcrmode_incoming_Handle(data->CCEvent.pNumber);

            if (work_sim == simid)
            {
                Ringtimer();
            }
            else
            {
                KING_CallHangUpAll(simid); // 当前AT仅支持单卡，而协议栈是双卡双待单通，暂时做规避处理。
            }
            break;

        case CC_EVENT_ORIGINATING:
            KING_SysLog("CC_EVENT_ORIGINATING call id %d",data->CCEvent.callID);
            CallState[data->CCEvent.callID] = CC_STATE_ORIGINATING;
            break;

        case CC_EVENT_ALERTING:
            KING_SysLog("CC_EVENT_ALERTING call id %d",data->CCEvent.callID);
            CallState[data->CCEvent.callID] = CC_STATE_ALERTING;
            break;

        case CC_EVENT_CONNECTED:
            KING_SysLog("CC_EVENT_CONNECTED call id %d",data->CCEvent.callID);
            CallState[data->CCEvent.callID] = CC_STATE_CONNECTED;
            break;

        case CC_EVENT_TERMINATED:
            KING_SysLog("CC_EVENT_TERMINATED call id %d",data->CCEvent.callID);
            CallState[data->CCEvent.callID] = CC_STATE_IDLE;
            ShowNoCarrier();
            Czcrmode_Endring_Handle();
            break;

        case CC_EVENT_WAITING:
            KING_SysLog("CC_EVENT_WAITING call id %d",data->CCEvent.callID);
            CallState[data->CCEvent.callID] = CC_STATE_WAITING;
            ShowRing();
            break;

        case CC_EVENT_ON_HOLD:
            KING_SysLog("CC_EVENT_ON_HOLD call id %d",data->CCEvent.callID);
            CallState[data->CCEvent.callID] = CC_STATE_ON_HOLD;
            break;

        case CC_EVENT_ERROR:
            KING_SysLog("CC_EVENT_ERROR call id %d",data->CCEvent.callID);
            Czcrmode_Endring_Handle();
            break;

        default:
            KING_SysLog("UNKOWN_EVENT call id %d",data->CCEvent.callID);
            break;
    }
}

void atCCInit(void)
{
    KING_RegNotifyFun(DEV_CLASS_CALL_CONTROL,0,CCEventCb);
}

static bool isGprsDial(const uint8 *dial)
{
    if (memcmp(dial, AT_GPRS_DIALSTR1, strlen(AT_GPRS_DIALSTR1)) == 0 ||
        memcmp(dial, AT_GPRS_DIALSTR2, strlen(AT_GPRS_DIALSTR2)) == 0 ||
        memcmp(dial, AT_GPRS_DIALSTR3, strlen(AT_GPRS_DIALSTR3)) == 0 ||
        memcmp(dial, AT_GPRS_DIALSTR4, strlen(AT_GPRS_DIALSTR4)) == 0 ||
        memcmp(dial, AT_GPRS_DIALSTR5, strlen(AT_GPRS_DIALSTR5)) == 0 ||
        memcmp(dial, AT_GPRS_DIALSTR6, strlen(AT_GPRS_DIALSTR6)) == 0)
        return true;
    return false;
}

void atCmdHandleD(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_EXE)
    {
        int ret;
        int numlen = 0;
        bool paramok = true;
        SIM_ID nSimId = SIM_0;
        const uint8 *dial_num = at_ParamDStr(pParam->params[0],&paramok);
        //AT_Util_GetParaWithRule

        KING_DualSimCurrent(&nSimId);

        if (!paramok || pParam->paramCount > 1)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        KING_SysLog("ATD num is %s",dial_num);

        if (isGprsDial(dial_num))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        ORIG_CALL_INFO CallingInfo;
        memset(&CallingInfo,0,sizeof(ORIG_CALL_INFO));
        CallingInfo.phoneNumberPtr = (uint8*)dial_num;
        CallingInfo.clir = CC_NO_CLIR;
        CallingInfo.cug = CC_NO_CUG;

        numlen = strlen((char*)dial_num);
        switch (dial_num[numlen - 1])
        {
            case 'i':
                CallingInfo.clir = CC_DEACTIVATE_CLIR;
                CallingInfo.phoneNumberPtr[numlen-1] = 0;
                break;

            case 'I':
                CallingInfo.clir = CC_ACTIVATE_CLIR;
                CallingInfo.phoneNumberPtr[numlen-1] = 0;
                break;

            case 'g':
                CallingInfo.cug = CC_DEACTIVATE_CUG;
                CallingInfo.phoneNumberPtr[numlen-1] = 0;
                break;

            case 'G':
                CallingInfo.cug = CC_ACTIVATE_CUG;
                CallingInfo.phoneNumberPtr[numlen-1] = 0;
                break;

            default:
                break;
        }

        ret = KING_CallVoiceDial(nSimId,&CallingInfo);
        if (ret != 0)
        {
            int err = KING_GetLastErrCode();
            if (err == CC_ERR_NO_SERVICE)
            {
                at_CmdRespErrorCode(pParam->engine, CMD_RC_NOCARRIER);
            }
            else if (err == CC_ERR_INVALID_PARAM)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            }
            else if (err == CC_ERR_DAILING)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            }
            else
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        else
        {
            at_CmdRespOK(pParam->engine);
        }
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

void atCmdHandleA(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_EXE)
    {
        int ret;
        SIM_ID nSimId = SIM_0;

        KING_DualSimCurrent(&nSimId);

        if (GetCallState() == CC_STATE_IDLE)
        {
            at_CmdRespErrorCode(pParam->engine, CMD_RC_NOCARRIER);
            return;
        }

        Czcrmode_Endring_Handle();

        ret = KING_CallAnswer(nSimId);
        if (ret != 0)
        {
            KING_SysLog("ATA ret is %d:%d",ret,KING_GetLastErrCode());
        }

        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }

}

void atCmdHandleH(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_EXE)
    {
        int ret;
        SIM_ID nSimId = SIM_0;

        KING_DualSimCurrent(&nSimId);

        KING_SysLog("ATH get param count %d",pParam->paramCount);

        if (pParam->paramCount > 0)
        {
            bool paramok = true;
            uint8_t value =  at_ParamUint(pParam->params[0],&paramok);
            if (!paramok || value != 0)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
        }

        ret = KING_CallHangUpAll(nSimId);
        if (ret != 0)
        {
            KING_SysLog("ATH ret is %d:%d",ret,KING_GetLastErrCode());
        }

        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }

}

void atCmdHandleS0(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_SET)
    {
        bool paramok = true;
        uint8_t value =  at_ParamUintInRange(pParam->params[0],0,255,&paramok);
        if (!paramok || pParam->paramCount>1)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        gATCurrentcc_s0 = value;

        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        char strRsp[10] = {0};
        sprintf(strRsp,"%d",gATCurrentcc_s0);
        at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, (const uint8_t*)"S0: (0-255)");
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}

static void Czcrmode_incoming_Handle(char * number)
{
    switch (s_ring_func)
    {
        case 1 :  // 打开并播放用户指定路径的PCM文件，如不存在该文件，则打开默认PCM文件 "MI.pcm"
            {
                int ret = FAIL;
                ret = KING_FsFileCreate(g_playpath, FS_MODE_OPEN_EXISTING | FS_MODE_READ, 0, 0, &s_filehandle);
                if (ret == FAIL)
                {
                    KING_SysLog("No such file: %s, use deafult file: %s !!", g_playpath, DEFAULT_PCM_FILE);
                    ret = KING_FsFileCreate(DEFAULT_PCM_FILE, FS_MODE_OPEN_EXISTING | FS_MODE_READ, 0, 0, &s_filehandle);
                }
                if (ret == SUCCESS)
                {
                    KING_AudioPcmStreamPlay(SAMPLING_RATE_8KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, AudioCb);
                }
                else
                {
                    KING_SysLog("No such file: %s , no play pcm", DEFAULT_PCM_FILE);
                }
            }
            break;

        case 2:
            KING_TtsInit();
            KING_TtsPlay(CODE_GBK, (uint8*)g_ttsbuff, strlen(g_ttsbuff), NULL);  //TTS播放指定字符串
            break;

        case 3:
            {
                KING_TtsInit();
                if (strlen(number) == 0)
                {
                    KING_TtsPlay(CODE_UCS2, (uint8*)"672A77E567657535", strlen("672A77E567657535"), NULL);
                }
                else
                {
                    KING_TtsPlay(CODE_GBK, (uint8*)number, strlen(number), NULL);  //TTS播放对端电话号码
                }
            }
            break;

        default:
            break;
    }
}

static void Czcrmode_Endring_Handle(void)
{
    switch (s_ring_func)
    {
        case 1 :
            KING_AudioStop();
            KING_FsCloseHandle(s_filehandle);
            s_filehandle = NULL;
            break;
            
        case 2:
        case 3:
            KING_TtsStop(); // 停止播放当前TTS
            break;

        default:
            break;
    }
}

void AT_CmdFunc_Czcrmode(AT_CMD_PARA *pParam)
{
    if (pParam->iType == AT_CMD_SET)
    {
        bool paramok = true;
        int ret = -1;
        uint8 ring_mode = 0xFF;
        uint32 size = sizeof(ring_mode);

        const uint16 mode = at_ParamUintInRange(pParam->params[0], 0, 3,&paramok);
        if (!paramok)
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (((mode == 1) || (mode ==2)) && (pParam->paramCount != 2))
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        if (pParam->paramCount == 2)
        {
            const char *value = (char *)at_ParamStr(pParam->params[1], &paramok);
            if (!paramok)
            {
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
            else
            {
                if (mode == 1)
                {
                    memset(g_playpath, 0x00, FILEPATH_LEN_MAX + 1);
                    if (strlen(value) > FILEPATH_LEN_MAX )
                    {
                        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                    }
                    else
                    {
                        strncpy(g_playpath, (char*)value, strlen(value));
                    }
                }
                else if (mode == 2)
                {
                    memset(g_ttsbuff, 0x00, TTS_LEN_MAX + 1);
                    if (strlen(value) > TTS_LEN_MAX)
                    {
                        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                    }
                    else
                    {
                        strncpy(g_ttsbuff, (char*)value, strlen(value));
                    }
                }
            }
        }

        s_ring_func = mode;
        if (mode == 0)
        {
            ring_mode = 0;
            ret = KING_SysCfg(1, SYS_CFG_ID_CC_RING_MODE, &ring_mode, &size);
            if (ret != 0)
            {
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
            }
            else
            {
                at_CmdRespOK(pParam->engine);
            }
        }
        else
        {
            ring_mode = 1;
            ret = KING_SysCfg(1, SYS_CFG_ID_CC_RING_MODE, &ring_mode, &size);
            if (ret != 0)
            {
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
            }
            else
            {
                at_CmdRespOK(pParam->engine);
            }
        }
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        char strRsp[20] = {0};

        sprintf(strRsp,"+CZCRMODE: %d", s_ring_func);
        at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, (const uint8_t*)"+CZCRMODE: (0-3)");
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
    }
}
