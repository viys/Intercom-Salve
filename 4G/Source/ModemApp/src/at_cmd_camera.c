#include "At_common.h"
#include "At_errcode.h"
#include "At_module.h"
#include "AT_define.h"
#include "at_dispatch.h"
#include "at_data_engine.h"
#include "KingCamera.h"
#include "KingCbData.h"

static bool s_ifScan = false;
static uint8 s_decodeMode = 0;
static uint32 s_decodeType = 0;
static uint8 s_dlci = 0;
static MUTEX_HANDLE s_camOptMutex = NULL;

VOID AT_Cam_Result_OK(uint32 uReturnValue,
                      uint32 uResultCode, uint8 nDelayTime, uint8 *pBuffer, uint16 nDataSize, uint8 nDLCI)

{
    PAT_CMD_RESULT pResult = NULL;

    pResult = AT_CreateRC(uReturnValue,
                          uResultCode, CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, nDelayTime, pBuffer, nDataSize, nDLCI);

    AT_Notify2ATM(pResult, nDLCI);
    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}


VOID AT_Cam_Result_Err(uint32 uErrorCode, uint8 nErrorType, uint8 nDLCI)
{
    PAT_CMD_RESULT pResult = NULL;

    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR, uErrorCode, nErrorType, 0, 0, 0, nDLCI);
    AT_Notify2ATM(pResult, nDLCI);

    if (pResult != NULL)
    {
        AT_FREE(pResult);
        pResult = NULL;
    }

    return;
}

void camEvtCallback(void *data)
{
    CAMERA_DATA_S *pdata = (CAMERA_DATA_S *)data;
    KING_SysLog("camEvtCallback:eventID=%d", pdata->eventID);

    switch(pdata->eventID)
    {
        case CAMERA_EVENT_SCANCODE_START:
            s_ifScan = true;
            break;

        case CAMERA_EVENT_SCANCODE_RESULT:
            {
                AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(s_dlci);
                at_CmdRespInfoText(engine, (uint8 *)(pdata->data));
            }
            break;

        case CAMERA_EVENT_SCANCODE_STOP:
            s_ifScan = false;
            break;
    }
    
}

void AT_CmdFunc_CZSCANCODE(AT_CMD_PARA *pParam)
{
    PAT_CMD_RESULT pResult;
    char nOutputStr[128] = {
        0,
    };
        
    if (NULL == pParam)
    {
        goto ERROR;
    }
    else
    {
        AT_CMD_PARA *Param = pParam;
        if (NULL == Param->pPara)
            goto ERROR;

        switch (Param->iType)
        {
            case AT_CMD_TEST:
                sprintf(nOutputStr, "+CZSCANCODE: (0-2),(0-19)");
                AT_Cam_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (uint8 *)nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI);
                break;

            case AT_CMD_READ:
                {
                    uint8 i = 0;
                    char DecodeTypeStr[64];
                    
                    if(s_decodeType == 0)
                    {
                        sprintf(nOutputStr, "+CZSCANCODE: %d,0", s_decodeMode);
                    }
                    else
                    {
                        AT_MemSet(DecodeTypeStr, 0x0, 64);
                        for(i=0; i<19; i++)
                        {
                            if(((s_decodeType >> i) & 0x01))
                            {
                                sprintf(DecodeTypeStr + AT_StrLen(DecodeTypeStr), "%d,", i+1);
                            }
                        }
                        DecodeTypeStr[AT_StrLen(DecodeTypeStr) - 1] = '\0';
                        sprintf(nOutputStr, "+CZSCANCODE: %d,%s", s_decodeMode, DecodeTypeStr);                
                    }
                    AT_Cam_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (uint8 *)nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI);
                }
                break;

            case AT_CMD_SET:
                {
                    uint8 uParaCount;
                    uint8 mode;
                    uint8 type;
                    uint8 i = 0;
                    uint32 typeList = 0;
                    uint32 nRet = 0;
                    uint16 uSize;

                    nRet = AT_Util_GetParaCount(pParam->pPara, &uParaCount);
                    if (0 != nRet || uParaCount < 1 || uParaCount > 20)
                    {
                        AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                        break;
                    }

                    uSize = 1;
                    nRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &mode, &uSize);
                    if (nRet != 0 || mode < 0 || mode > 2)
                    {
                        AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                        break;
                    }

                    if (s_camOptMutex == NULL)
                    {
                        if (KING_MutexCreate("s_camOptMutex", 0, &s_camOptMutex) != 0 )
                        {
                            AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                            break;
                        }
                    }
                    
                    if(1 == mode || 2 == mode)
                    {
                        if(uParaCount > 1)
                        {
                            uSize = 1;
                            for(i = 1; i < uParaCount; i++)
                            {
                                nRet = AT_Util_GetParaWithRule(pParam->pPara, i, AT_UTIL_PARA_TYPE_UINT8, &type, &uSize);
                                if (nRet != 0 || type < 0 || type > 19)
                                {
                                    goto ERROR;
                                }

                                if(0 == type)
                                {
                                    typeList = 0;
                                    break;
                                }
                                else
                                {
                                    typeList |= (1 << (type-1));
                                }
                            }
                        }

                        KING_MutexLock(s_camOptMutex, WAIT_OPT_INFINITE);
                        if (!s_ifScan)
                        {
                            nRet = KING_CameraInit(NULL, 0);
                            if(nRet == 0)
                            {
                                CAMERA_MODEL_E camModel = 0;
                                if(mode == 1)
                                {
                                    camModel = CAMERA_ONE_SHOT;
                                }
                                else
                                {
                                    camModel = CAMERA_CONSECUTIVELY;
                                }
                                
                                KING_CameraStart(camModel, typeList, camEvtCallback);
                            }
                        }
                        KING_MutexUnLock(s_camOptMutex);
                    }
                    else if(0 == mode)
                    {
                        if(uParaCount > 1)
                        {
                            AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                            break;
                        }

                        KING_MutexLock(s_camOptMutex, WAIT_OPT_INFINITE);
                        if (s_ifScan)
                        {
                            nRet = KING_CameraStop();
                            if(0 == nRet)
                            {
                                KING_CameraDeinit();
                            }
                        }
                        KING_MutexUnLock(s_camOptMutex);
                    }

                    if (0 == nRet)
                    {
                        s_decodeMode = mode;
                        if(s_decodeMode != 0)
                        {
                            s_decodeType = typeList;
                        }
                        s_dlci = pParam->nDLCI;
                        AT_Cam_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 30, NULL, 0, pParam->nDLCI);
                    }
                    else
                    {
                        AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    }
               }
               break;

            default:
                goto ERROR;
                break;
        }
    }
    return;

ERROR:
    pResult = AT_CreateRC(CMD_FUNC_FAIL, CMD_RC_ERROR,
                          CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);
    if (pResult != NULL)
    {
        AT_Notify2ATM(pResult, pParam->nDLCI);
        AT_FREE(pResult);
        pResult = NULL;
    }
}


void AT_CmdFunc_CZCAMPARAM(AT_CMD_PARA *pParam)
{
    char nOutputStr[128] = {
        0,
    };
        
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            sprintf(nOutputStr, "+CZCAMPARAM: (0,1),<param>[,<paramValue>]");
            AT_Cam_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (uint8 *)nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI);
            break;

        case AT_CMD_SET:
            {
                uint8 uParaCount;
                uint8 opt;
                uint8 param;
                uint32 nRet = 0;
                uint16 uSize;

                nRet = AT_Util_GetParaCount(pParam->pPara, &uParaCount);
                if (0 != nRet || uParaCount < 2 || uParaCount > 3)
                {
                    AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    break;
                }

                uSize = 1;
                nRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &opt, &uSize);
                if (nRet != 0 || opt < 0 || opt > 1)
                {
                    AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    break;
                }

                if(opt == 0 && uParaCount == 3)
                {
                    AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    break;
                }
                
                uSize = 1;
                nRet = AT_Util_GetParaWithRule(pParam->pPara, 1, AT_UTIL_PARA_TYPE_UINT8, &param, &uSize);
                if (nRet != 0 || param < 0 || param > 4)
                {
                    AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    break;
                }

                if(opt == 1)
                {
                    uint32 value;
                    uSize = 4;
                    nRet = AT_Util_GetParaWithRule(pParam->pPara, 2, AT_UTIL_PARA_TYPE_UINT32, &value, &uSize);
                    if (nRet != 0 || value > 5)
                    {
                        AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                        break;
                    }
                    
                    nRet = KING_Camera_ParameterSet(param, value);
                    if(nRet == 0)
                    {
                        AT_Cam_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, NULL, 0, pParam->nDLCI);
                    }
                    else
                    {
                        AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    }
                }
                else
                {
                    uint32 getValue = 0;
                    nRet = KING_Camera_ParameterGet(param, &getValue);
                    if(nRet == 0)
                    {
                        sprintf(nOutputStr, "+CZCAMPARAM: %d, %d", param, getValue);
                        AT_Cam_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, (uint8 *)nOutputStr, AT_StrLen(nOutputStr), pParam->nDLCI);
                    }
                    else
                    {
                        AT_Cam_Result_Err(nRet, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
                    }
                }               
            }
            break;

        default:
            AT_Cam_Result_Err(CMD_ERROR_CODE_OK, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI);
            break;
    }
}

