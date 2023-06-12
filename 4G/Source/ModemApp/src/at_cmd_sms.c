#include "at_common.h"
#include "at_module.h"
#include "at_errcode.h"
#include "at_cfg.h"
#include "at_utility.h"
#include "kingsms.h"
#include "kingatapi.h"
#include "at_define.h"
#include "KingCbData.h"


static int _smsStr2StorageInfo(char *mem, SMS_STORAGE_TYPE *storage, uint32_t *used, uint32_t *total, uint8 nSim)
{
    int nOperationRet = SUCCESS;
    AT_StrUpr(mem);
    if (!strcmp(mem, "ME"))
    {
        *storage = SMS_STORAGE_ME;
    }
    else if (!strcmp(mem, "SM"))
    {
        *storage = SMS_STORAGE_SM;
    }
    else if (!strcmp(mem, "MT"))
    {
        *storage = SMS_STORAGE_MT;
    }
    else if (!strcmp(mem, "SR"))
    {
        *storage = SMS_STORAGE_SR;
    }
    else
    {
        nOperationRet = ERR_AT_CMS_INVALID_PARA;
    }
    KING_SysLog("_smsStr2StorageInfo *storage %d ",*storage);
    if (SUCCESS == nOperationRet)
    {
        if(*storage == SMS_STORAGE_SM)
        {
            nOperationRet = KING_SmsStorageStatusGet(nSim,(SIM_SMSSTORAGE)*storage,used,total);
        }
        else
        {
            nOperationRet = ERR_AT_CMS_MEM_FAIL;
        }
    }
    return nOperationRet;
}

static int _smsStorage2StrInfo(char *mem, SMS_STORAGE_TYPE storage, uint32_t *used, uint32_t *total, uint8 nSim)
{
    int nOperationRet = SUCCESS;
    
    nOperationRet = KING_SmsStorageStatusGet(nSim,(SIM_SMSSTORAGE)storage,used,total);

    if (SUCCESS != nOperationRet)
    {
        nOperationRet = ERR_AT_CMS_MEM_FAIL;
    }
    else
    {
        if (storage == SMS_STORAGE_ME)
        {
            strcpy(mem, "ME");
        }
        else if (storage == SMS_STORAGE_SM)
        {
            strcpy(mem,"SM");
        }
        else if (storage == SMS_STORAGE_MT)
        {
            strcpy(mem, "MT");
        }
        else if (storage == SMS_STORAGE_SR)
        {
            strcpy(mem,"SR");
        }
        else
        {
            nOperationRet = ERR_AT_CMS_INVALID_PARA;
        }
    }
    
    return nOperationRet;
}

#define AT_SMS_STATUS_READ 0
#define AT_SMS_STATUS_UNREAD 1
#define AT_SMS_STATUS_SENT 2
#define AT_SMS_STATUS_UNSENT 3
#define AT_SMS_STATUS_ALL 4

typedef struct
{
    uint32_t value;  ///< integer value
    const char *str; ///< string value
} osiValueStrMap_t;


static const osiValueStrMap_t gSmsNumState[] = {
    {AT_SMS_STATUS_UNREAD, "REC UNREAD"},
    {AT_SMS_STATUS_READ, "REC READ"},
    {AT_SMS_STATUS_SENT, "STO UNSENT"},
    {AT_SMS_STATUS_UNSENT, "STO SENT"},
    {AT_SMS_STATUS_ALL, "ALL"},
    {0, NULL},
};

const osiValueStrMap_t *_smsOsiVsmapFindByVal(const osiValueStrMap_t *vsmap, uint32_t value)
{
    for (;;)
    {
        const osiValueStrMap_t *vs = vsmap++;
        if (vs->str == NULL)
            return NULL;
        if (vs->value == value)
            return vs;
    }
    return NULL; // never reach
}

static const char *_smsAtStatusStr(uint8_t status)
{
    const osiValueStrMap_t *m = _smsOsiVsmapFindByVal(gSmsNumState, status);
    return (m != NULL) ? m->str : "";
}

void AT_CmdFunc_CPMS(AT_CMD_PARA *pParam)
{
    int nOperationRet = SUCCESS;
    const uint8* MemBuff = NULL;
    uint8 nSim = AT_SIM_ID(pParam->nDLCI);
    uint8 nStorage1Last = 0;
    uint8 nStorage2Last = 0;
    uint8 nStorage3Last = 0;
    uint8 PromptBuff[64] = {0};
    uint8 ReadMem1[3] = {0};
    uint8 ReadMem2[3] = {0};
    uint8 ReadMem3[3] = {0};
    bool paramok = TRUE;
    
    KING_SysLog("AT_CmdFunc_CPMS");
    
    if (pParam->iType == AT_CMD_SET)
    {
        nStorage1Last = gATCurrentAtSMSInfo[nSim].nStorage1;
        nStorage2Last = gATCurrentAtSMSInfo[nSim].nStorage2;
        nStorage3Last = gATCurrentAtSMSInfo[nSim].nStorage3;
        
        if ((pParam->paramCount == 0) || (pParam->paramCount > 3))
        {
            nOperationRet = ERR_AT_CMS_INVALID_PARA;
        }

        //===========================================
        // para1, para 2 and para3 process these code
        //===========================================
        if (nOperationRet == SUCCESS && pParam->paramCount > 0)
        {
            MemBuff = at_ParamStr(pParam->params[0],&paramok);
            if (paramok)
            {
                KING_SysLog("AT_CmdFunc_CPMS MemBuff1 %s ",MemBuff);
                nOperationRet = _smsStr2StorageInfo((char*)MemBuff, &gATCurrentAtSMSInfo[nSim].nStorage1,
                                        &gATCurrentAtSMSInfo[nSim].nUsed1,&gATCurrentAtSMSInfo[nSim].nTotal1,nSim);
            }
        }

        //===========================================
        // para 2 and para3 process these code
        //===========================================
        if (nOperationRet == SUCCESS && pParam->paramCount > 1)
        {
            MemBuff = at_ParamStr(pParam->params[1],&paramok);
            if (paramok)
            {
                KING_SysLog("AT_CmdFunc_CPMS MemBuff2 %s ",MemBuff);
                nOperationRet = _smsStr2StorageInfo((char*)MemBuff, &gATCurrentAtSMSInfo[nSim].nStorage2,
                                        &gATCurrentAtSMSInfo[nSim].nUsed2,&gATCurrentAtSMSInfo[nSim].nTotal2,nSim);
            }
        }

        //===========================================
        // para3 process these code
        //===========================================
        if (nOperationRet == SUCCESS && pParam->paramCount > 2)
        {
            MemBuff = at_ParamStr(pParam->params[2],&paramok);
            if (paramok)
            {
                KING_SysLog("AT_CmdFunc_CPMS MemBuff3 %s ",MemBuff);
                nOperationRet = _smsStr2StorageInfo((char*)MemBuff, &gATCurrentAtSMSInfo[nSim].nStorage3,
                                        &gATCurrentAtSMSInfo[nSim].nUsed3,&gATCurrentAtSMSInfo[nSim].nTotal3,nSim);
            }
        }

        if (SUCCESS == nOperationRet)
        {

            AT_Sprintf((char*)PromptBuff, "+CPMS: %u,%u,%u,%u,%u,%u",
                       gATCurrentAtSMSInfo[nSim].nUsed1,
                       gATCurrentAtSMSInfo[nSim].nTotal1,
                       gATCurrentAtSMSInfo[nSim].nUsed2,
                       gATCurrentAtSMSInfo[nSim].nTotal2,
                       gATCurrentAtSMSInfo[nSim].nUsed3,
                       gATCurrentAtSMSInfo[nSim].nTotal3);
            at_CmdRespInfoText(pParam->engine, PromptBuff);
            at_CmdRespOK(pParam->engine);
            return;
        }
        else
        {

            gATCurrentAtSMSInfo[nSim].nStorage1 = nStorage1Last;
            gATCurrentAtSMSInfo[nSim].nStorage2 = nStorage2Last;
            gATCurrentAtSMSInfo[nSim].nStorage3 = nStorage3Last;
            AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, nOperationRet));
        }
    }
    else if(pParam->iType == AT_CMD_READ)
    {
        SIM_STATES simStates;
        KING_SimCardInformationGet(SIM_0,&simStates);
        if(simStates != SIM_READY)
        {
            AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_SIM_WRONG));
        }
        // ///////////////////////////////
        /* read mem1 buffer                        */
        // ///////////////////////////////
        nOperationRet = _smsStorage2StrInfo((char*)ReadMem1,gATCurrentAtSMSInfo[nSim].nStorage1,
                                &gATCurrentAtSMSInfo[nSim].nUsed1,&gATCurrentAtSMSInfo[nSim].nTotal1,nSim);
        
        // ///////////////////////////////
        /* read mem2 buffer                        */
        // ///////////////////////////////
        nOperationRet = _smsStorage2StrInfo((char*)ReadMem2,gATCurrentAtSMSInfo[nSim].nStorage2,
                                &gATCurrentAtSMSInfo[nSim].nUsed2,&gATCurrentAtSMSInfo[nSim].nTotal2,nSim);


        // ///////////////////////////////
        /* read mem3 buffer                        */
        // ///////////////////////////////
        nOperationRet = _smsStorage2StrInfo((char*)ReadMem3,gATCurrentAtSMSInfo[nSim].nStorage3,
                                &gATCurrentAtSMSInfo[nSim].nUsed3,&gATCurrentAtSMSInfo[nSim].nTotal3,nSim);


        if (SUCCESS == nOperationRet)
        {
            AT_Sprintf((char*)PromptBuff, "+CPMS: \"%s\",%u,%u,\"%s\",%u,%u,\"%s\",%u,%u",
                       ReadMem1,
                       gATCurrentAtSMSInfo[nSim].nUsed1,
                       gATCurrentAtSMSInfo[nSim].nTotal1,
                       ReadMem2,
                       gATCurrentAtSMSInfo[nSim].nUsed2,
                       gATCurrentAtSMSInfo[nSim].nTotal2,
                       ReadMem3,
                       gATCurrentAtSMSInfo[nSim].nUsed3,
                       gATCurrentAtSMSInfo[nSim].nTotal3);
            at_CmdRespInfoText(pParam->engine, PromptBuff);
            at_CmdRespOK(pParam->engine);
            return;
        }
        else
        {
            AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, nOperationRet));
        }
    }
    else if(pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, (uint8*)"+CPMS: (\"ME\",\"SM\",\"MT\",\"SR\"),(\"ME\",\"SM\",\"MT\",\"SR\"),(\"ME\",\"SM\",\"MT\",\"SR\")");
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
}

void AT_CmdFunc_CMGD(AT_CMD_PARA *pParam)
{
    int nOperationRet = SUCCESS;
    uint8 PromptInfo[32] = {0};
    bool paramok = TRUE;
    uint32 used;
    uint32 total;

    uint8_t nSim = AT_SIM_ID(pParam->nDLCI);

    uint8_t nStorage = gATCurrentAtSMSInfo[nSim].nStorage1;

    nOperationRet = KING_SmsStorageStatusGet(nSim,nStorage,&used,&total);

    if(nOperationRet != SUCCESS)
    {
        AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_ME_FAIL));
    }
    if(pParam->iType == AT_CMD_SET)
    {
        uint8 nDeleteFlag = 0;
        uint16 nDelMsgIndex = 0;
        if (pParam->paramCount > 2)
        {
            AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_INVALID_PARA));
        }
        
        //===========================================
        // para1, para 2 process these code
        //===========================================
        if (pParam->paramCount > 0)
        {
            nDelMsgIndex = at_ParamUint(pParam->params[0],&paramok);
            if (!paramok)
            {
                AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_INVALID_PARA));
            }
        
            if (nDelMsgIndex < 1 || nDelMsgIndex > total)
            {
                AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_INVALID_MEM_INDEX));
            }
        }
        
        //===========================================
        // para 2 process these code
        //===========================================
        if (pParam->paramCount > 1)
        {
            nDeleteFlag = at_ParamUint(pParam->params[1],&paramok);
            if (!paramok)
            {
                AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_INVALID_PARA));
            }
            if(nDeleteFlag != 0 && nDeleteFlag != 4)
            {
                AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_INVALID_PARA));
            }
        }
        
        if (nDeleteFlag == 4) //delete all sms
            nDelMsgIndex = 0;

        if (SUCCESS != KING_SmsMsgDelete(nSim, SMS_STORAGE_SIM, nDelMsgIndex))
        {
            AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_MEM_FAIL));
        }
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if(pParam->iType == AT_CMD_TEST)
    {
        sprintf((char*)PromptInfo, "+CMGD: (1-%u),(0-4)", total);
        at_CmdRespInfoText(pParam->engine, PromptInfo);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
}


void AT_CmdFunc_CMGR(AT_CMD_PARA *pParam)
{
    int nOperationRet = SUCCESS;
    uint8_t nSim = AT_SIM_ID(pParam->nDLCI);
    bool paramok = TRUE;
    uint32 used;
    uint32 total;

    if (pParam->iType == AT_CMD_SET)
    {
        SMS_MESSAGE_T smsMsg = {0};
        char rsp[128];
        
        if (pParam->paramCount > 1)
        {
            AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_INVALID_PARA));
        }
        unsigned nStorage = gATCurrentAtSMSInfo[nSim].nStorage1;
        nOperationRet = KING_SmsStorageStatusGet(nSim,nStorage,&used,&total);
        if(nOperationRet != SUCCESS)
        {
            KING_SysLog("AT_CmdFunc_CMGD get storage file");
            AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_ME_FAIL));
        }
        // +CMGR=<index>
        uint32 nindex = at_ParamUintInRange(pParam->params[0], 1, total, &paramok);
        if (!paramok)
        {
            AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_INVALID_PARA));
        }
        
        KING_SysLog("AT_CmdFunc_CMGD nStorage:%d,nindex:%d",nStorage,nindex);
        nOperationRet = KING_SmsMsgGet(nSim, nStorage,nindex,&smsMsg);
        if( SUCCESS != nOperationRet)
        {
            KING_SysLog("AT_CmdFunc_CMGD get msg file %d",KING_GetLastErrCode());
            AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_ME_FAIL));
        }

        if(smsMsg.type == SMS_DELIVER)
        {
            sprintf(rsp, "+CMGR: \"%s\",\"%s\",%s",
                    _smsAtStatusStr(smsMsg.smsDeliver.status), smsMsg.smsDeliver.addr, smsMsg.smsDeliver.scts);
            at_CmdRespInfoText(pParam->engine, (uint8*)rsp);
            at_CmdRespInfoText(pParam->engine, smsMsg.smsDeliver.data);
        }
        else if(smsMsg.type == SMS_SUBMIT)
        {
            sprintf(rsp, "+CMGR: \"%s\",\"%s\",",_smsAtStatusStr(smsMsg.smsSubmit.status), smsMsg.smsSubmit.addr);
            at_CmdRespInfoText(pParam->engine, (uint8*)rsp);
            at_CmdRespInfoText(pParam->engine, smsMsg.smsSubmit.data);
        }
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_OPER_NOT_ALLOWED);
    }
}

void AT_CmdFunc_CMGS(AT_CMD_PARA *pParam)
{
    uint8_t nSim = AT_SIM_ID(pParam->nDLCI);
    bool paramok = true;

    if (pParam->iType == AT_CMD_SET)
    {
        SMS_MESSAGE_T sms = {0};
        const uint8 *da = at_ParamStr(pParam->params[0], &paramok);
        /*
        if(pParam->paramCount > 1)
        {
            uint8_t toda = at_ParamUint(pParam->params[1], &paramok);
        }
        */
        
        if (!paramok || pParam->paramCount > 2)
        {
            AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_INVALID_PARA));
        }

        sms.type = SMS_SUBMIT;
        sms.smsSubmit.format = FORMAT_TEXT;
        strcpy(sms.smsSubmit.addr,(char*)da);
        if (!pParam->iExDataLen)
        {
            at_CmdRespOutputPrompt(pParam->engine);
            at_CmdSetPromptMode(pParam->engine);
        }
        else
        {
            if (pParam->pExData[pParam->iExDataLen - 1] == 27)
            {
                KING_SysLog("AT_CmdFunc_CMGS: end with esc, cancel send");
                AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
            }
            else if (pParam->pExData[pParam->iExDataLen - 1] != 26 || pParam->iExDataLen <= 1)
            {
                KING_SysLog("AT_CmdFunc_CMGS: not end with ctl+z, err happen");
                AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine,ERR_AT_CMS_INVALID_LEN));
            }
            else
            {
                if(pParam->iExDataLen > SMS_TEXT_MAX_BYTES)
                {
                    AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine,ERR_AT_CMS_INVALID_LEN));
                }
                sms.smsSubmit.dataLen = pParam->iExDataLen - 1;
                memcpy(sms.smsSubmit.data,pParam->pExData,sms.smsSubmit.dataLen);
                if(SUCCESS != KING_SmsMsgSend(nSim,SMS_STORAGE_SIM,&sms))
                {
                    AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine,ERR_AT_CMS_ME_FAIL));
                }
                else
                {
                    AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
                }
            }
        }
        
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
}

void AT_CmdFunc_CNMI(AT_CMD_PARA *pParam)
{
    uint8_t nSim = AT_SIM_ID(pParam->nDLCI);
    bool paramok = true;
    if (pParam->iType == AT_CMD_SET)
    {
        uint32_t bmList[2] = {0, 2};
        unsigned mode = at_ParamDefUintInRange(pParam->params[0], gATCurrentAtSMSInfo[nSim].sCnmi.nMode, 0, 2, &paramok);
        unsigned mt = at_ParamDefUintInRange(pParam->params[1], gATCurrentAtSMSInfo[nSim].sCnmi.nMt, 0, 2, &paramok);
        unsigned bm = at_ParamDefUintInList(pParam->params[2], gATCurrentAtSMSInfo[nSim].sCnmi.nBm, bmList,2, &paramok);
        unsigned ds = at_ParamDefUintInRange(pParam->params[3], gATCurrentAtSMSInfo[nSim].sCnmi.nDs, 0, 2, &paramok);
        unsigned bfr = at_ParamDefUintInRange(pParam->params[4], gATCurrentAtSMSInfo[nSim].sCnmi.nBfr, 0, 1, &paramok);
        if (!paramok || pParam->paramCount > 5)
            AT_CMD_RETURN(at_CmdRespCmsError(pParam->engine,ERR_AT_CMS_INVALID_PARA));
        
        gATCurrentAtSMSInfo[nSim].sCnmi.nMode = mode;
        gATCurrentAtSMSInfo[nSim].sCnmi.nMt = mt;
        gATCurrentAtSMSInfo[nSim].sCnmi.nBm = bm;
        gATCurrentAtSMSInfo[nSim].sCnmi.nDs = ds;
        gATCurrentAtSMSInfo[nSim].sCnmi.nBfr = bfr;
        at_CmdRespOK(pParam->engine);
    }
    else if(pParam->iType == AT_CMD_READ)
    {
        char rsp[64];
        sprintf(rsp, "+CNMI: %u,%u,%u,%u,%u",
                gATCurrentAtSMSInfo[nSim].sCnmi.nMode, gATCurrentAtSMSInfo[nSim].sCnmi.nMt,
                gATCurrentAtSMSInfo[nSim].sCnmi.nBm, gATCurrentAtSMSInfo[nSim].sCnmi.nDs,
                gATCurrentAtSMSInfo[nSim].sCnmi.nBfr);
        at_CmdRespInfoText(pParam->engine, (uint8*)rsp);
        at_CmdRespOK(pParam->engine);
    }
    else if(pParam->iType == AT_CMD_TEST)
    {
        at_CmdSetPromptMode(pParam->engine);
        at_CmdRespInfoText(pParam->engine, (uint8*)"+CNMI: (0-2),(0-3),(0,2),(0-2),(0,1)");
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmsError(pParam->engine, ERR_AT_CMS_OPER_NOT_SUPP);
    }
}

void SmsNotifyFun(uint32 eventID, void* pData, uint32 len)
{
    uint8 nSim = 0;
    //AT_CMD_ENGINE_T* engine = at_CmdGetByChannel(at_ModuleGetSimDlci(nSim));
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pData;
    uint32 index = (uint8)data->smsEvent.nParam;
    char rsp[128];
    switch(eventID)
    {
        case SMS_EVENT_STATE_NEW_SMS:
        {
            SMS_MESSAGE_T smsMsg = {0};
            KING_SmsMsgGet(nSim, SMS_STORAGE_SIM,index,&smsMsg);
            
            uint8_t nMt = gATCurrentAtSMSInfo[nSim].sCnmi.nMt;
            
            if (nMt == 1) /* dump report MT == 1 */
            {
                sprintf(rsp, "+CMTI: \"SM\",%u", index);
                at_CmdReportUrcText(0, (uint8*)rsp);
            
            }
            else if (nMt == 2) /* dump report MT == 2 */
            {
                sprintf(rsp, "+CMT: \"%s\",%s",smsMsg.smsDeliver.addr, smsMsg.smsDeliver.scts);
                at_CmdReportUrcText(0, (uint8*)rsp);
                at_CmdReportOutputText(0, smsMsg.smsDeliver.data);
                at_CmdReportOutputText(0, (uint8*)"\r\n");
            }
        }
        break;

        default:break;
    }
}


void AT_SMSInit()
{
    KING_RegNotifyFun(DEV_CLASS_SMS, 0, SmsNotifyFun);
}

