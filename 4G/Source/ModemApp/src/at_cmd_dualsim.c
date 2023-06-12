#include "at_common.h"
#include "at_dispatch.h"
#include "at_errcode.h"
#include "at_module.h"
#include "at_define.h"
#include "KingDualSim.h"
#include "KingCbData.h"


static bool s_is_switching = FALSE;

extern UINT8 g_rspStr[];

/**
 * 手动切换业务卡的回调，以及底层建议切换业务卡的提示
 */
void _DualSimSwitchCb(uint32 eventID, void* pData, uint32 len)
{
    if (DUALSIM_EVENT_SWITCH_RSP == eventID)
    { // 手动切换业务卡结果回调
        DUAL_SIM_RSP_DATA_S *data = (DUAL_SIM_RSP_DATA_S *)pData;
        SIM_ID sim = SIM_0;

        KING_DualSimCurrent(&sim);
        sprintf((char *)g_rspStr, "^DSSWITCH: %d,%d", data->ret, sim);
        at_CmdReportUrcText(0, g_rspStr);

        s_is_switching = FALSE;
    }
    else if (DUALSIM_EVENT_SWITCH_IND == eventID)
    { // 底层建议切换业务卡的提示
        DUAL_SIM_IND_DATA_S *data = (DUAL_SIM_IND_DATA_S *)pData;
        sprintf((char *)g_rspStr, "^DSURC: %d,%d", data->aim_sim, data->cause);
        at_CmdReportUrcText(0, g_rspStr);
    }
}

/**
 * 检测底层是否支持双卡功能，初始化双卡功能，注册双卡相关回调函数。
 */
VOID AT_DualSim_Init(void)
{
    uint8 enable = 0;
    SIM_ID def_sim = SIM_0;

    if (0 == KING_DualSimInit())
    {
        if (0 == KING_DualSimGet(&enable, &def_sim))
        {
            if (enable == 1)
            {
                KING_RegNotifyFun(DEV_CLASS_DUALSIM, 0, _DualSimSwitchCb);
            }
        }
    }
}

/** 
 * 开关双卡功能，并设置开机默认业务卡
 */
VOID AT_CmdFunc_DSS(AT_CMD_PARA *pParam)
{
    int ret = 0;
    bool paramok = true;
    uint32_t enable = 0;
    uint32_t def_sim = 0;

    if (pParam->pPara == NULL)
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_MEMORY_FAILURE);
        return;
    }

    if (0 != KING_DualSimInit())
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if (pParam->paramCount != 2)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        enable = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        def_sim = at_ParamUintInRange(pParam->params[1], 0, 1, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        ret = KING_DualSimSet((uint8)enable, (SIM_ID)def_sim);
        if (0 == ret)
        {
            at_CmdRespOK(pParam->engine);
            if (1 == enable)
            {
                KING_RegNotifyFun(DEV_CLASS_DUALSIM, 0, _DualSimSwitchCb);
            }
            else
            {
                KING_UnRegNotifyFun(DEV_CLASS_DUALSIM, 0, _DualSimSwitchCb);
            }
        }
        else
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        break;

    case AT_CMD_READ:
        ret = KING_DualSimGet((uint8 *)(&enable), (SIM_ID *)(&def_sim));
        if (0 == ret)
        {
            sprintf((char *)g_rspStr, "^DSS: %d,%d", enable, def_sim);
            at_CmdRespInfoText(pParam->engine, g_rspStr);
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        break;

    case AT_CMD_TEST:
        sprintf((char *)g_rspStr, "^DSS: (0,1),(0,1)");
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}


/** 
 * 手动动态切换业务卡。
 */
VOID AT_CmdFunc_SIMSWITCH(AT_CMD_PARA *pParam)
{
    int ret = 0;
    bool paramok = true;
    uint32_t enable = 0;
    SIM_ID sim = SIM_0;
    SIM_ID cur_sim = SIM_0;
    uint32_t aim_sim = 0;

    if (pParam->pPara == NULL)
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_MEMORY_FAILURE);
        return;
    }

    if (0 != KING_DualSimInit())
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if (pParam->paramCount != 1)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        ret = KING_DualSimGet((uint8 *)(&enable), &sim);
        if ((0 != ret) || (1 != enable))
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED));
        }

        aim_sim = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        KING_DualSimCurrent(&cur_sim);

        if (aim_sim == cur_sim)
        {
            at_CmdRespOK(pParam->engine);

            sprintf((char *)g_rspStr, "^DSSWITCH: %d,%d", 0, cur_sim);
            at_CmdReportUrcText(0, g_rspStr);
            break;
        }

        if (TRUE == s_is_switching)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
            break;
        }

        ret = KING_DualSimSwitch((SIM_ID)aim_sim);
        if (0 == ret)
        {
            s_is_switching = TRUE;
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        break;

    case AT_CMD_READ:
        ret = KING_DualSimGet((uint8 *)(&enable), &sim);
        if ((0 != ret) || (1 != enable))
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED));
        }

        ret = KING_DualSimCurrent(&sim);
        if (0 == ret)
        {
            sprintf((char *)g_rspStr, "^DSSWITCH: %d", sim);
            at_CmdRespInfoText(pParam->engine, g_rspStr);
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        break;

    case AT_CMD_TEST:
        sprintf((char *)g_rspStr, "^DSSWITCH: (0,1)");
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}


