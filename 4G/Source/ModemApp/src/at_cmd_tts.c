#if  1//def KING_SDK_TTS_SUPPORT

#include "KingAudio.h"
#include "KingTts.h"
#include "KingRtos.h"
#include "KingPlat.h"
#include "at_common.h"
#include "at_module.h"
#include "at_errcode.h"

#define TTS_MAX_TEXT_LEN        (959)
static uint8 initFlag = 0;

extern int IsTtsStop(void);
extern UINT8 IsAudPlaying(void);
extern void AudPlayStart(void);
extern void AudPlayEnd(void);
static AT_CMD_ENGINE_T * ttsAtHandle = NULL;

static void TtsPlayCallback(TTS_PLAY_E event, uint8 *buff, uint32 len)
{
    KING_SysLog("event=%d", event);
    if(TTS_PLAY_STOP == event)
    {
        AudPlayEnd();
        at_CmdRespUrcText(ttsAtHandle, (const uint8*)"+QTTS: 0");
    }
    else if(TTS_PLAY_START == event)
    {
        AudPlayStart();
    }
}

void AT_CmdFunc_QTTS(AT_CMD_PARA *pParam)
{
    if(0 == initFlag)
    {
        initFlag = 1;
        KING_TtsInit();
    }
    ttsAtHandle = pParam->engine;
    if (AT_CMD_SET == pParam->iType)
    {
        // STOP:    AT+QTTS=0
        // START:   AT+QTTS=<mode>,<text>
        bool paramok = true;
        uint8 mode = at_ParamUintInRange(pParam->params[0], 0, 2, &paramok);
        if (!paramok)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }
        
        if (mode == 0) // stop
        {
            if (pParam->paramCount > 1)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }

            at_CmdRespOK(pParam->engine);
            if(!IsTtsStop())
            {
                KING_TtsStop();
            }
        }
        else// start
        {
            TTS_CODE_TYPE_E codeType = CODE_UCS2;
            uint32 len = 0;
            const uint8 *str = NULL;
            uint32 pVolume = 0;
            AUDIO_STATE_E state = AUDIO_STATE_MAX;
            KING_AudioStatusGet(&state);
            if((!IsTtsStop()) || IsAudPlaying() || (AUDIO_STATE_IDLE != state))
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                return;
            }
            AudPlayStart();
            str = at_ParamStr(pParam->params[1], &paramok);
            if (!paramok || pParam->paramCount > 2)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                AudPlayEnd();
                return;
            }

            KING_AudioVolumeGet(AUDIO_SPEAK, &pVolume);
            if(0 == pVolume)
            {
                at_CmdRespOK(pParam->engine);
                at_CmdRespUrcText(ttsAtHandle, (const uint8*)"+QTTS: 0");
                AudPlayEnd();
                return;
            }
            
            len = strlen((char*)str);
            
            PRD_LOG("len=%d", len);
            if (len == 0 || len > TTS_MAX_TEXT_LEN)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                AudPlayEnd();
                return;
            }

            if (mode == 1)
            {
                codeType = CODE_UCS2;
            }
            else if(mode == 2)
            {
                codeType = CODE_GBK;
            }
            if (FAIL == KING_TtsPlay(codeType, (uint8*)str, len, TtsPlayCallback))
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                AudPlayEnd();
            }
            else
            {
                at_CmdRespOK(pParam->engine);
            }
        }
    }
    else if (AT_CMD_READ == pParam->iType)
    {
        char rsp[48];
        int state = 0;
        if(IsTtsStop())
        {
            state = 0;
        }
        else
        {
            state = 1;
        }
        sprintf(rsp, "+QTTS: %d", state);
        at_CmdRespInfoText(pParam->engine, (const uint8_t*)rsp);
        at_CmdRespOK(pParam->engine);
    }
    else if (AT_CMD_TEST == pParam->iType)
    {
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPTION_NOT_SURPORT);
    }
}

void AT_CmdFunc_QTTSETUP(AT_CMD_PARA *pParam)
{
    uint8 mode = 0;
    uint8 id = 0;
    int16 value = 0;
    bool ret = true;
    bool paramok = true;
    TTS_PARAM_S ttsParam;
    uint32 errcode = ERR_AT_CME_PARAM_INVALID;
    if(0 == initFlag)
    {
        initFlag = 1;
        KING_TtsInit();
    }
    if(AT_CMD_TEST == pParam->iType)
    {
        char rsp[256];
        memset(rsp, 0, sizeof(rsp));
        sprintf(rsp, "+QTTSETUP: %s\n+QTTSETUP: %s\n+QTTSETUP: %s\n+QTTSETUP: %s\n+QTTSETUP: %s\n+QTTSETUP: %s\n+QTTSETUP: %s\n+QTTSETUP: %s", 
            "(1,2),1[,(-32768-32767)]", "(1,2),2[,(0-1)]", "(1,2),3[,(-32768-32767)]", "(1,2),4[,(-32768-32767)]",  "(1,2),5[,(0-2)]", \
            "(1,2),6[,(0-1)]", "(1,2),7[,(0-2)]", "(1,2),8[,(0-2)]");
    
        at_CmdRespInfoText(pParam->engine, (const uint8_t*)rsp);
        at_CmdRespOK(pParam->engine);
    }
    else if(AT_CMD_SET == pParam->iType)
    {
        if(2 == pParam->paramCount)
        {
            mode = at_ParamUintInRange(pParam->params[0], 1, 2, &paramok);
            if (!paramok || (mode != 2))
            {
                ret = false;
            }
            else
            {
                id = at_ParamUintInRange(pParam->params[1], 1, 8, &paramok);
                if(!paramok)
                {
                    ret = false;
                }

                if((2 == id) || (6 == id) || (7 == id) || (8 == id))
                {
                    ret = false;
                    errcode = ERR_AT_CME_OPERATION_NOT_SUPPORTED;
                }
            }
            
        }
        else if(3 == pParam->paramCount)
        {
            memset(&ttsParam, 0x00, sizeof(ttsParam));
            KING_TtsParamsGet(&ttsParam);
            mode = at_ParamUintInRange(pParam->params[0], 1, 2, &paramok);
            if(!paramok || (mode != 1) )
            {
                ret = false;
            }
            else
            {
                id = at_ParamUintInRange(pParam->params[1], 1, 8, &paramok);
                if(!paramok)
                {
                    ret = false;
                }
                else
                {
                    if(1 == id)
                    {
                        value = at_ParamIntInRange(pParam->params[2], -32768, 32767, &paramok);
                        if(!paramok)
                        {
                            ret = false;
                        }
                        else
                        {
                            ttsParam.volume = value;
                        }
                    }
                    else if(2 == id)
                    {
                        #if 0
                        value = at_ParamUintInRange(pParam->params[2], 0, 1, &paramok);
                        //KING_SysLog("value=%d\n", value);
                    
                        if(!paramok)
                        {
                            ret = false;
                        }
                        else
                        {
                            ttsParam.role = value;
                        }
                        #endif
                        
                        ret = false;
                        errcode = ERR_AT_CME_OPERATION_NOT_SUPPORTED;
                    }
                    else if(3 == id)
                    {
                        value = at_ParamIntInRange(pParam->params[2], -32768, 32767, &paramok);
                        //KING_SysLog("value=%d\n", value);
                        if(!paramok)
                        {
                            ret = false;
                        }
                        else
                        {
                            ttsParam.pitch = value;
                        }
                    }
                    else if(4 == id)
                    {
                        value = at_ParamIntInRange(pParam->params[2], -32768, 32767, &paramok);
                        //KING_SysLog("value=%d\n", value);
                        if(!paramok)
                        {
                            ret = false;
                        }
                        else
                        {
                            ttsParam.speed= value;
                        }
                    }
                    else if(5 == id)
                    {
                        value = at_ParamUintInRange(pParam->params[2], 0, 2, &paramok);
                        //KING_SysLog("value=%d\n", value);
                        if(!paramok)
                        {
                            ret = false;
                        }
                        else
                        {
                            ttsParam.readDigit = value;
                        }
                    }
                    else if(6 == id)
                    {
                        #if 0
                        value = at_ParamUintInRange(pParam->params[2], 0, 1, &paramok);
                        //KING_SysLog("value=%d\n", value);
                        if(!paramok)
                        {
                            ret = false;
                        }
                        else
                        {
                            ret = false;
                            //ttsParam.inCallPlay = value;
                        }
                        #endif
                        
                        ret = false;
                        errcode = ERR_AT_CME_OPERATION_NOT_SUPPORTED;
                    }
                    else if(7 == id)
                    {
                        #if 0
                        value = at_ParamUintInRange(pParam->params[2], 0, 2, &paramok);
                        //KING_SysLog("value=%d\n", value);
                        if(!paramok)
                        {
                            ret = false;
                        }
                        else
                        {
                            ttsParam.readalp = value;
                        }
                        #endif
                        
                        ret = false;
                        errcode = ERR_AT_CME_OPERATION_NOT_SUPPORTED;
                    }
                    else if(8 == id)
                    {
                        #if 0
                        value = at_ParamUintInRange(pParam->params[2], 0, 2, &paramok);
                        //KING_SysLog("value=%d\n", value);
                        if(!paramok)
                        {
                            ret = false;
                        }
                        else
                        {
                            ttsParam.language = value;
                        }
                        #endif
                        
                        ret = false;
                        errcode = ERR_AT_CME_OPERATION_NOT_SUPPORTED;
                    }
                }
            }
            
        }
        else
        {
            ret = false;
        }
    

        if(ret)
        {
             if(1 == mode)
             {
                 KING_TtsParamsSet(&ttsParam);
                at_CmdRespOK(pParam->engine);
             }
             else if(2 == mode)
             {
                 char rsp[48];
                 memset(rsp, 0, sizeof(rsp));
                 KING_TtsParamsGet(&ttsParam);
                 if(1 == id)
                 {                    
                    sprintf(rsp, "+QTTSETUP: %d,%d,%d", mode, id, ttsParam.volume);
                 }
                 else if(2 == id)
                 {                    
                    sprintf(rsp, "+QTTSETUP: %d,%d,%d", mode, id, ttsParam.role);
                 }
                 else if(3 == id)
                 {
                     sprintf(rsp, "+QTTSETUP: %d,%d,%d", mode, id, ttsParam.pitch);
                 }
                 else if(4 == id)
                 {
                     sprintf(rsp, "+QTTSETUP: %d,%d,%d", mode, id, ttsParam.speed);
                 }
                 else if(5 == id)
                 {
                     sprintf(rsp, "+QTTSETUP: %d,%d,%d", mode, id, ttsParam.readDigit);
                 }
                 else if(6 == id)
                 {
                     //sprintf(rsp, "%s: %d,%d,%d", pParam->pPara,mode, id, ttsParam.incallPlay);
                 }
                 else if(7 == id)
                 {
                     sprintf(rsp, "+QTTSETUP: %d,%d,%d", mode, id, ttsParam.readalp);
                 }
                 else if(8 == id)
                 {
                     sprintf(rsp, "+QTTSETUP: %d,%d,%d", mode, id, ttsParam.language);
                 }
                 at_CmdRespInfoText(pParam->engine, (const uint8_t*)rsp);
                 at_CmdRespOK(pParam->engine);
             }
        }
        else
        {
            at_CmdRespCmeError(pParam->engine, errcode);
        }
    }
    else if(AT_CMD_READ == pParam->iType)
    {
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
    }
}
#endif
