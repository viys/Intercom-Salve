/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#include "at_module.h"
#include "at_sa.h"
#include "at_utils.h"
#include "at_dispatch.h"
#include "at_errcode.h"
#include "kingrtos.h"
#include "at_parse.h"
#include "at_cmux_engine.h"
#include "at_cmd_fota.h"
#include "at_cmd_dualsim.h"

#define FSM_STATUS_MASK_POWER_DOWN 0x00000001
#define FSM_STATUS_MASK_NORMAL 0x00000002
#define FSM_STATUS_MASK_CHGR_NORMAL 0x00000004
#define FSM_STATUS_MASK_CHGR_ONLY 0x00000008
#define FSM_STATUS_MASK_ALARM_NORMAL 0x00000010
#define FSM_STATUS_MASK_ALARM_ONLY 0x00000020
#define FSM_STATUS_MASK_SLEEP 0x00000040
#define PROC_MODE_MASK_COMMAND 0x00000001
#define PROC_MODE_MASK_ONLINE_CMD 0x00000002
#define PROC_MODE_MASK_CS 0x00000004
#define PROC_MODE_MASK_PS 0x00000008


// //////////////////////////////////////////////////
// System States
// //////////////////////////////////////////////////
typedef enum _sa_fsm_state_t {
    SA_FSM_STATE_POWER_DOWN,
    SA_FSM_STATE_NORMAL,
    SA_FSM_STATE_CHGR_NORMAL,
    SA_FSM_STATE_CHGR_ONLY,
    SA_FSM_STATE_ALARM_NORMAL,
    SA_FSM_STATE_ALARM_ONLY,
    SA_FSM_STATE_SLEEP,
    SA_FSM_STATE_COUNT_,   // counter of this enum.
    SA_FSM_STATE_INVALID_, // invalid value of this enum.
} sa_fsm_state_t;

typedef enum _sa_proc_mode_t {
    SA_PROC_MODE_COMMAND,
    SA_PROC_MODE_ONLINE_CMD,
    SA_PROC_MODE_CS,
    SA_PROC_MODE_PS,
    SA_PROC_MODE_COUNT_,   // counter of this enum.
    SA_PROC_MODE_INVALID_, // invalid value of this enum.
} sa_proc_mode_t;

// //////////////////////////////////////////////////
// GLOBAL VARIABLES DEFINITION
// //////////////////////////////////////////////////
//static sa_info_t g_SaInfo = {0};
//static sa_info_t g_SaInfoMux[MAX_DLC_NUM] = {
// {0},
//};

extern AT_ModuleInfo g_AtModule;
extern struct AT_DISPATCH_T *g_dispatch[AT_DISPATCH_ID_MAX];
AT_ENGINE_MODE_T g_AtEngineMode = AT_ENGINE_MODE_NORMAL;
void GpsCreateTask(void);

// //////////////////////////////////////////////////
// PROTOTYPE DEFINITIONS OF INTERNAL FUNCTIONS
// //////////////////////////////////////////////////
VOID BAL_ATTask(void* argv);

// //////////////////////////////////////////////////
// PROTOTYPE DEFINITIONS OF EXTERNAL FUNCTIONS
// //////////////////////////////////////////////////

#define MMI_VERSION_STRING "MMIv1.0.1"

extern int atSimInit(void);
extern void atNwInit(void);
extern void atCCInit(void);
extern void AudplayInit(void);
extern void AT_SMSInit();
extern void tcpip_init(void);
extern void icamqtt_Init(void);
extern void AT_http_init(void);
extern void AT_Adc_Init(void);

BOOL SA_IsCmdEnabledByMode(const SA_COMMAND_TABLE_T *desc)
{
    if ((desc->mode_enable_flags & (1 << g_AtEngineMode)) == 0)
        return FALSE;
    return TRUE;
}

BOOL SA_IsCmdEnabled(AT_CMD_ENGINE_T *engine, const SA_COMMAND_TABLE_T *desc)
{
    UINT32 sys_stat = 0;

    if ((desc->mode_enable_flags & (1 << g_AtEngineMode)) == 0)
        return FALSE;

    sys_stat = SA_FSM_STATE_NORMAL; // SA_GetSysStatusMux(engine, SA_INNER_FSM_STATUS_);

    KING_SysLog("enable_mask.fsm_status_mask %d", desc->enable_mask.fsm_status_mask);

    switch (sys_stat)
    {
    case SA_FSM_STATE_POWER_DOWN:
        return FALSE;

    case SA_FSM_STATE_NORMAL:
        if ((desc->enable_mask.fsm_status_mask & FSM_STATUS_MASK_NORMAL) != FSM_STATUS_MASK_NORMAL)
            return FALSE;
        break;

    case SA_FSM_STATE_CHGR_NORMAL:
        if ((desc->enable_mask.fsm_status_mask & FSM_STATUS_MASK_CHGR_NORMAL) !=
            FSM_STATUS_MASK_CHGR_NORMAL)
            return FALSE;

        break;

    case SA_FSM_STATE_CHGR_ONLY:
        if ((desc->enable_mask.fsm_status_mask & FSM_STATUS_MASK_CHGR_ONLY) != FSM_STATUS_MASK_CHGR_ONLY)
            return FALSE;

        break;

    case SA_FSM_STATE_ALARM_NORMAL:
        if ((desc->enable_mask.fsm_status_mask & FSM_STATUS_MASK_ALARM_NORMAL) !=
            FSM_STATUS_MASK_ALARM_NORMAL)
            return FALSE;

        break;

    case SA_FSM_STATE_ALARM_ONLY:
        if ((desc->enable_mask.fsm_status_mask & FSM_STATUS_MASK_ALARM_ONLY) != FSM_STATUS_MASK_ALARM_ONLY)
            return FALSE;

        break;

    case SA_FSM_STATE_SLEEP:
        return FALSE;

    default:
        return FALSE;
    }

    sys_stat = SA_PROC_MODE_COMMAND; // SA_GetSysStatusMux(engine, SA_INNER_PROC_MODE_);

    KING_SysLog("SA_GetSysStatus %d", sys_stat);
    KING_SysLog("enable_mask.proc_mode_mask %d", desc->enable_mask.proc_mode_mask);

    switch (sys_stat)
    {
    case SA_PROC_MODE_COMMAND:
        if ((desc->enable_mask.proc_mode_mask & PROC_MODE_MASK_COMMAND) != PROC_MODE_MASK_COMMAND)
            return FALSE;

        break;

    case SA_PROC_MODE_ONLINE_CMD:
        if ((desc->enable_mask.proc_mode_mask & PROC_MODE_MASK_ONLINE_CMD) != PROC_MODE_MASK_ONLINE_CMD)
            return FALSE;

        break;

    // [[hameina[mod] 2008-4-22 :Ps, cs are different modes, can't do the same check.
    case SA_PROC_MODE_PS:
        if ((desc->enable_mask.proc_mode_mask & PROC_MODE_MASK_PS) != PROC_MODE_MASK_PS)
            return FALSE;

        break;

    case SA_PROC_MODE_CS:
        if ((desc->enable_mask.proc_mode_mask & PROC_MODE_MASK_CS) != PROC_MODE_MASK_CS)
            return FALSE;

        break;

    default:
        return FALSE;
    }

    return TRUE;
}


/***************************************************************************************************
  SYSTEM AGENT
 ****************************************************************************************************/
/* The default AT (MMI) Task Handle. */
THREAD_HANDLE g_hAtTask;
MSG_HANDLE g_hAtMsg;
//
// Marcos to create default MMI Task.
//
// ////////////////////////////////////////////////////////////////////////////////////////////////
// Old stack size is too small to process the case, this is there maybe lots of commands in one command line.
// Now, one command node will cost about 112 bytes, so if there are 20 commands in one command line,
// it will cost 112 * 20 = 2k bytes, so we must increase the stack size!
// Maybe we have another solution for this issue, I think we can reduce the memory cost of one command noce, and I think
// 112 per command node is too big!!!

static BOOL BAL_ATModeInit(AT_ENGINE_MODE_T mode)
{
    THREAD_ATTR_S threadAttr;
    
    ATLOGI("BAL_ApplicationInit Started...");

    g_AtEngineMode = mode;

    // Create a default MMI task by CSW automatically.
    // You only can change the task stack size.

    memset(&threadAttr, 0x00, SIZEOF(THREAD_ATTR_S));
    threadAttr.stackSize = 5 * 1024;
    threadAttr.queueNum = 64;
    threadAttr.fun = BAL_ATTask;
    threadAttr.priority = THREAD_PRIORIT1;
    if (FAIL == KING_ThreadCreate("AT Task", &threadAttr, &g_hAtTask))
    {
        ATLOGI("Create AT TASK fail: %d", KING_GetLastErrCode());
    }

    if (g_AtEngineMode != AT_ENGINE_MODE_NORMAL)
        return TRUE;
    
    return TRUE;
}

BOOL BAL_ATInit(void)
{
    return BAL_ATModeInit(AT_ENGINE_MODE_NORMAL);
}

BOOL BAL_CalibATInit(void)
{
    return BAL_ATModeInit(AT_ENGINE_MODE_CALIB);
}

extern PVOID g_CswBaseAdd;
extern UINT32 g_CswHeapSize;

VOID BAL_ATTask(void* argv)
{
    int ret;
    MSG_S msg;
    ATLOGI("Enter BAL_ATTask...");
    AT_ModuleInit();
    AT_InitUtiTable();
    atSimInit();
    AT_SMSInit();

    ATLOGI("AT_ThreadHandle is running...\r\n");

    memset(&msg, 0x00, sizeof(MSG_S));

    ret = KING_MsgCreate(&g_hAtMsg);
    if (ret == FAIL)
    {
        ATLOGI("BAL_ATTask: Failed to create Msg Handle!\r\n");
        KING_ThreadExit();
        return;
    }

    tcpip_init();
    atNwInit();
    atCCInit();
    AudplayInit();
    icamqtt_Init();
    AT_http_init();
    CZ_FotaInit();
    AT_DualSim_Init();
    AT_Adc_Init();
    for (;;)
    {
        KING_MsgRcv(&msg, g_hAtMsg, WAIT_OPT_INFINITE);
        
        switch(msg.messageID)
        {
            case EV_AT_CALLBACK:
            {
                AT_TASK_MSG_PARAM_T data = {0};
                memcpy((void*)&data,(AT_TASK_MSG_PARAM_T*)msg.pData,msg.DataLen);
                KING_MemFree(msg.pData);
                msg.pData = NULL;
                data.func(data.param);
            }
            break;

            default:
            break;
        }
    }
}

VOID SA_DefaultCmdHandler_Error(AT_CMD_PARA *pParam)
{
    at_CmdRespCmeError(pParam->engine, ERR_AT_UNKNOWN);
}

VOID SA_DefaultCmdHandler_AT(AT_CMD_PARA *pParam)
{
    at_CmdRespOK(pParam->engine);
}

VOID SA_DefaultCmdHandler_CMUX(AT_CMD_PARA *pParam)
{
    UINT8 msg[80] = {0};
    AT_CMUX_CONFIG_T muxcfg = gAtCmuxDefaultCfg;

    if (pParam->iType == AT_CMD_TEST)
    {
        strcpy((char*)msg, "+CMUX: (0,1),(0),(1-6),(1-2048),(1-255),(0-100),(2-255),(1-255),(1-7)");
        at_CmdRespInfoText(pParam->engine, msg);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        AT_CMUX_ENGINE_T *cmux = at_DispatchGetParentCmux(at_DispatchGetByChannel(pParam->nDLCI));
        if (cmux != NULL)
            muxcfg = at_CmuxGetConfig(cmux);
        
        sprintf((char *)msg, "+CMUX: %d,%d,%d,%d,%d,%d,%d,%d,%d",
                muxcfg.transparency, muxcfg.subset, muxcfg.portSpeed, muxcfg.maxFrameSize,
                muxcfg.ackTimer, muxcfg.maxRetransCount, muxcfg.respTimer, muxcfg.wakeupRespTimer,
                muxcfg.windowSize);
        
        at_CmdRespInfoText(pParam->engine, msg);
        AT_CMD_RETURN(at_CmdRespOK(pParam->engine));
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount > 9 || pParam->paramCount < 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        muxcfg.transparency = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        muxcfg.subset = at_ParamDefUintInRange(pParam->params[1], muxcfg.subset, 0, 0, &paramok);
        muxcfg.portSpeed = at_ParamDefUintInRange(pParam->params[2], muxcfg.portSpeed, 1, 6, &paramok);
        muxcfg.maxFrameSize = at_ParamDefUintInRange(pParam->params[3], muxcfg.maxFrameSize, 1, 2048, &paramok);
        muxcfg.ackTimer = at_ParamDefUintInRange(pParam->params[4], muxcfg.ackTimer, 1, 255, &paramok);
        muxcfg.maxRetransCount = at_ParamDefUintInRange(pParam->params[5], muxcfg.maxRetransCount, 0, 100, &paramok);
        muxcfg.respTimer = at_ParamDefUintInRange(pParam->params[6], muxcfg.respTimer, 2, 255, &paramok);
        muxcfg.wakeupRespTimer = at_ParamDefUintInRange(pParam->params[7], muxcfg.wakeupRespTimer, 1, 255, &paramok);
        muxcfg.windowSize = at_ParamDefUintInRange(pParam->params[8], muxcfg.windowSize, 1, 7, &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (at_CmdGetDispatch(pParam->engine) == g_dispatch[AT_DISPATCH_ID_USB])
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
        }

        at_CmdRespOK(pParam->engine);
        at_DispatchSetCmuxMode(at_DispatchGetByChannel(pParam->nDLCI), &muxcfg);
    }
    else
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT));
    }
}

