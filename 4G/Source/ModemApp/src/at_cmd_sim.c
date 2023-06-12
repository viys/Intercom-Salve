#include "At_common.h"
#include "At_errcode.h"
#include "At_module.h"

#include "kingUsim.h"
#include "kingcstd.h"
#include "kingplat.h"
#include "kingDualsim.h"
#include "KingCbData.h"

extern UINT8 g_rspStr[];

typedef struct
{
    uint32_t value;  ///< integer value
    const char *str; ///< string value
} osiValueStrMap_t;

static const osiValueStrMap_t gCpinBlockStr[] = {
    {SIM_LOCKEDSTATE_READY, "+CPIN: PIN1 READY"},
    {SIM_LOCKEDSTATE_SIM_PIN, "+CPIN: SIM PIN"},
    {SIM_LOCKEDSTATE_SIM_PUK, "+CPIN: SIM PUK"},
    {SIM_LOCKEDSTATE_PH_SIM_PIN, "+CPIN: PH-SIM PIN"},
    {SIM_LOCKEDSTATE_PH_FSIM_PIN, "+CPIN: PH-FSIM PIN"},
    {SIM_LOCKEDSTATE_PH_FSIM_PUK, "+CPIN: PH-FSIM PUK"},
    {SIM_LOCKEDSTATE_SIM_PIN2, "+CPIN: SIM PIN2"},
    {SIM_LOCKEDSTATE_SIM_PUK2, "+CPIN: SIM PUK2"},
    {SIM_LOCKEDSTATE_PH_NET_PIN, "+CPIN: PH-NET PIN"},
    {SIM_LOCKEDSTATE_PH_NET_PUK, "+CPIN: PH-NET PUK"},
    {SIM_LOCKEDSTATE_PH_NETSUB_PIN, "+CPIN: PH-NETSUB PIN"},
    {SIM_LOCKEDSTATE_PH_NETSUB_PUK, "+CPIN: PH-NETSUB PUK"},
    {SIM_LOCKEDSTATE_PH_SP_PIN, "+CPIN: PH-SP PIN"},
    {SIM_LOCKEDSTATE_PH_SP_PUK, "+CPIN: PH-SP PUK"},
    {SIM_LOCKEDSTATE_PH_CORP_PIN, "+CPIN: PH-CORP PIN"},
    {SIM_LOCKEDSTATE_PH_CORP_PUK, "+CPIN: PH-CORP PUK"},
    {0, NULL},
};

static SIM_STATES s_sim_state[SIM_2];
static bool s_qsimstat = FALSE;



const osiValueStrMap_t *osiVsmapFindByVal(const osiValueStrMap_t *vsmap, uint32_t value)
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

const char *osiVsmapFindStr(const osiValueStrMap_t *vsmap, uint32_t value, const char *defval)
{
    const osiValueStrMap_t *vs = osiVsmapFindByVal(vsmap, value);

    return (vs == NULL) ? defval : vs->str;
}


static void SimStateCb(uint32 eventID, void* pData, uint32 len)
{
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pData;
    char simrsp[32] = {0};
    SIM_ID nSimId = SIM_0;

    KING_DualSimCurrent(&nSimId);

    KING_SysLog("%s: eventID=%d, classID=%d", __func__, eventID, data->usimState.classID);
    if (eventID == SIM_EVENT_STATE && data->usimState.classID == DEV_CLASS_SIM_STATE)
    {
        uint32 state = data->usimState.state;
        SIM_ID evt_sim = (SIM_ID)data->usimState.simID;
        KING_SysLog("%: sim = %d, state=%d", __func__, evt_sim, state);

        switch(state)
        {
            case SIM_ABSENT:
                KING_SysLog("%s: SIM_ABSENT", __func__);
                if ((s_qsimstat) && (nSimId == evt_sim))
                {
                    sprintf(simrsp, "+QSIMSTAT: %d,%d", s_qsimstat, 0);
                    at_CmdReportUrcText(0, (const uint8_t *)simrsp);

                    sprintf(simrsp, "+CPIN: NOT READY");
                    at_CmdReportUrcText(0, (const uint8_t *)simrsp);
                }
                s_sim_state[evt_sim] = SIM_ABSENT;
                break;

            case SIM_INSERTED:
                KING_SysLog("%s: SIM_INSERTED", __func__);
                if ((s_qsimstat) && (nSimId == evt_sim))
                {
                    sprintf(simrsp, "+QSIMSTAT: %d,%d", s_qsimstat, 1);
                    at_CmdReportUrcText(0, (const uint8_t *)simrsp);
                }
                s_sim_state[evt_sim] = SIM_INSERTED;
                break;

            case SIM_READY:
                KING_SysLog("%s: SIM_READY", __func__);
                if ((s_qsimstat) && (nSimId == evt_sim))
                {
                    if (s_sim_state[evt_sim] != SIM_READY)
                    {
                        sprintf(simrsp, "+CPIN: READY");
                        at_CmdReportUrcText(0, (const uint8_t *)simrsp);
                    }
                }
                s_sim_state[evt_sim] = SIM_READY;
                break;

            case SIM_BLOCKED:
                break;

            default:
                break;
        }
    }
}

int atSimInit(void)
{
    KING_RegNotifyFun(DEV_CLASS_SIM_STATE, 0, SimStateCb);

    return 0;
}

void AT_CmdFunc_CIMI(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_EXE:
        {
            int ret = SUCCESS;
            IMSI_S imsi;
            SIM_ID nSimId = SIM_0;
            char strRsp[30] = {0};

            ret = KING_DualSimCurrent(&nSimId);
            if (ret != 0)
            {
                nSimId = SIM_0;
            }

            memset(&imsi, 0, sizeof(IMSI_S));
            ret = KING_SimImsiGet(nSimId, &imsi);
            if (ret != 0)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                break;
            }
            memset(strRsp, 0x00, 30);
            sprintf(strRsp, "%s", imsi.imsi_val);
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }

    return;
}

void AT_CmdFunc_CCID(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_EXE:
        {
            int ret = SUCCESS;
            ICCID_S iccid;
            SIM_ID nSimId = SIM_0;
            char strRsp[30] = {0};

            ret = KING_DualSimCurrent(&nSimId);
            if (ret != 0)
            {
                nSimId = SIM_0;
            }

            memset(&iccid, 0, sizeof(ICCID_S));
            ret = KING_SimICCIDGet(nSimId, &iccid);
            if (ret != 0)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                break;
            }
            memset(strRsp, 0x00, 30);
            sprintf(strRsp, "%s", iccid.id_num);
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }

    return;
}

void AT_CmdFunc_CPIN(AT_CMD_PARA *pParam)
{
    SIM_ID nSimId = SIM_0;
    int ret = SUCCESS;

    ret = KING_DualSimCurrent(&nSimId);
    if (ret != 0)
    {
        nSimId = SIM_0;
    }

    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_READ:
        {
            SIM_STATES simState = SIM_STATE_UNKNOWN;
            SIM_LOCK_STATE lockState = SIM_LOCKEDSTATE_UNKNOWN;
            const char *cpinStr = NULL;
            char strRsp[64] = {0};
            
            ret = KING_SimCardInformationGet(nSimId, &simState);
            KING_SysLog("AT_CmdFunc_CPIN ret = %d, simState=%d", ret, simState);
            if (ret != 0)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }

            memset(strRsp, 0x00, 64);
            if (simState == SIM_BLOCKED)
            {
                ret = KING_SimCardLockStateGet(nSimId, &lockState);
                if (ret != 0)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    break;
                }
                cpinStr = osiVsmapFindStr(gCpinBlockStr, lockState, NULL);
                if (cpinStr == NULL)
                {
                    if (ret != 0)
                    {
                        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                        break;
                    }
                }
                sprintf(strRsp, "%s", cpinStr);
            }
            else
            {
                if (simState == SIM_READY)
                {
                    //UINT8 *tempStr = "READY";
                    sprintf(strRsp, "+CPIN: %s", "READY");
                }
                else if(simState == SIM_ABSENT)
                {
                    sprintf(strRsp, "+CPIN: %s", "NO SIM");
                }
                else
                {
                    sprintf(strRsp, "+CPIN: %s", "");
                }
            }
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;

        case AT_CMD_SET:
        {
            bool paramok = true;
            const uint8_t *pin = NULL;
            const uint8_t *newpin = NULL;
            int errCode = 0;

            if (pParam->paramCount > 2)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }

            pin = at_ParamOptStr(pParam->params[0], &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            //pin_size = strlen((char *)pin);

            newpin = at_ParamDefStr(pParam->params[1], NULL, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            //newpin_size = (newpin == NULL) ? 0 : strlen((char *)newpin);

            if (pParam->paramCount == 2)
            {
                ret = KING_SimPukEnter(nSimId, (const char*)pin, (const char*)newpin);
                errCode = ERR_AT_CME_SIM_PUK_REQUIRED;
            }
            else
            {
                ret = KING_SimPinEnter(nSimId, (const char*)pin);
                errCode = ERR_AT_CME_SIM_PIN_REQUIRED;
            }
            if (ret != 0)
            {
                at_CmdRespCmeError(pParam->engine, errCode);
                break;
            }
            at_CmdRespOK(pParam->engine);
        }
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }

    return;
}

void AT_CmdFunc_SIMCROSS(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            at_CmdRespInfoText(pParam->engine, (uint8 *)"+SIMCROSS: (0,1)");
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_READ:
        {
            int ret = SUCCESS;
            SIM_ID simId = 0;
            char strRsp[30] = {0};
        
            ret = KING_SimCurrentIdGet(&simId);
            if (ret != SUCCESS)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                break;
            }
            sprintf(strRsp, "+SIMCROSS: %d", simId);
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
        }
            break;

        case AT_CMD_SET:
        {
            bool paramok = true;
            uint8_t simId = 0;
            int ret = SUCCESS;

            simId = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                return;
            }

            ret = KING_SimIdSelect(simId);
            if (ret != SUCCESS)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                break;
            }
            at_CmdRespOK(pParam->engine);
        }
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
}

void AT_CmdFunc_SIMHOTPLUGCFG(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_TEST://type(0:read,1:write),simid(0:SIM_0,1:SIM_1),enable(0:close,1:open),level(0:low level,1:high level)
            at_CmdRespInfoText(pParam->engine, (uint8 *)"+SIMHOTPLUGCFG: (0,1),(0,1),(0,1),(0,1)");
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_SET:
        {
            bool paramok = true;
            int ret = SUCCESS;
            SIM_HOT_PLUG_CFG_T plug_cfg;
            uint32 size = sizeof(SIM_HOT_PLUG_CFG_T);
            uint8 nSimId = 0, nType = 0, enable = 0, nLevel = 0;
            char strRsp[30] = {0};
            
            if (pParam->paramCount < 2 || pParam->paramCount > 4)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }
            nType = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                return;
            }
            
            nSimId = at_ParamUintInRange(pParam->params[1], 0, 1, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                return;
            }

            if (pParam->paramCount >= 3)
            {
                enable = at_ParamUintInRange(pParam->params[2], 0, 1, &paramok);
                if (!paramok)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    return;
                }
                
                nLevel = at_ParamUintInRange(pParam->params[3], 0, 1, &paramok);
                if (!paramok)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    return;
                }
            }
            
            memset(&plug_cfg, 0x00, size);
            plug_cfg.simID = nSimId;
            if (nType == 0 && pParam->paramCount == 2)//read
            {
                ret = KING_SysCfg(0, SYS_CFG_ID_SIM_HOT_PLUG, (void *)(&plug_cfg), &size);
                if (ret != SUCCESS)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    break;
                }
                enable = plug_cfg.enable;
                nLevel = plug_cfg.level;
                
                sprintf(strRsp, "+SIMHOTPLUGCFG: %d,%d,%d", nSimId, enable, nLevel);
                at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
                at_CmdRespOK(pParam->engine);
            }
            else if (nType == 1 && pParam->paramCount == 4)//write
            {
                plug_cfg.enable = enable;
                plug_cfg.level = nLevel;
                plug_cfg.simID = nSimId;
                ret = KING_SysCfg(1, SYS_CFG_ID_SIM_HOT_PLUG, (void *)&plug_cfg, &size);
                if (ret != SUCCESS)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
                    break;
                }

                at_CmdRespOK(pParam->engine);
            }
            else
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
}

void AT_CmdFunc_QSIMDET(AT_CMD_PARA *pParam)
{
    SIM_ID nSimId = SIM_0;
    bool paramok = true;
    int ret = SUCCESS;
    SIM_HOT_PLUG_CFG_T cfg = {0};
    uint32 size = sizeof(SIM_HOT_PLUG_CFG_T);
    bool enable = FALSE;
    bool trig_level = FALSE;
    char strRsp[30] = {0};

    ret = KING_DualSimCurrent(&nSimId);

    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            at_CmdRespInfoText(pParam->engine, (uint8 *)"+QSIMDET: (0,1)[,(0,1)]");
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_READ:
            cfg.simID = nSimId;
            ret = KING_SysCfg(0, SYS_CFG_ID_SIM_HOT_PLUG, (void *)&cfg, &size);
            if (ret != SUCCESS)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                break;
            }
            sprintf(strRsp, "+QSIMDET: %d,%d", cfg.enable, cfg.level);
            at_CmdRespInfoText(pParam->engine, (uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_SET:
            if (pParam->paramCount > 2)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }

            enable = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }

            if ((enable == 0) && (pParam->paramCount != 1))
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }

            if (enable == 1)
            {
                trig_level = at_ParamUintInRange(pParam->params[1], 0, 1, &paramok);
                if (!paramok)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    return;
                }
            }

            cfg.simID = nSimId;
            cfg.enable = enable;
            cfg.level = trig_level;
            ret = KING_SysCfg(1, SYS_CFG_ID_SIM_HOT_PLUG, (void *)&cfg, &size);
            if (ret != SUCCESS)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                break;
            }
            at_CmdRespOK(pParam->engine);
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
}


void AT_CmdFunc_QSIMSTAT(AT_CMD_PARA *pParam)
{
    int ret = SUCCESS;
    SIM_ID simId = 0;
    char strRsp[30] = {0};
    bool paramok = true;
    uint32 sim_ready = 0;

    KING_SimCurrentIdGet(&simId);

    switch (pParam->iType)
    {
        case AT_CMD_TEST:
            at_CmdRespInfoText(pParam->engine, (uint8 *)"+QSIMSTAT: (0,1)");
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_READ:
            ret = KING_SimReadyGetById(simId, &sim_ready);
            if (0 != ret)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                return;
            }
            sprintf(strRsp, "+QSIMSTAT: %d,%d", s_qsimstat,sim_ready);
            at_CmdRespInfoText(pParam->engine, (const uint8 *)strRsp);
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_SET:
            s_qsimstat = (bool)at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }

            at_CmdRespOK(pParam->engine);
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
}


