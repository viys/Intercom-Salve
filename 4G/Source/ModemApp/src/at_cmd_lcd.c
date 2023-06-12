#include "at_common.h"
#include "at_errcode.h"
#include "at_module.h"
#include "KingDisplay.h"

static uint32_t enable = 0;
static bool init_flag = false;

void AT_CmdFunc_CZLCDONOFF(AT_CMD_PARA *pParam)
{
    if(init_flag == false)
    {
        if(0 == KING_LcdInit(0, NULL))
        {
            init_flag = true;
        }
        else
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    switch (pParam->iType)
    {
    case AT_CMD_SET:
        {
            bool paramok = true;
            enable = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }
            if(1 == enable)
            {
                if(0 !=KING_LcdScreenOn(0))
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                if(0 !=KING_LcdScreenOff(0))
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            at_CmdRespOK(pParam->engine);
        }
        break;

    case AT_CMD_READ:
        {
            char rsp[32];
            sprintf(rsp, "+CZLCDONOFF: %d", enable);
            at_CmdRespInfoText(pParam->engine, (uint8_t *)rsp);
            at_CmdRespOK(pParam->engine);
        }
        break;
    
    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CZLCDONOFF: (0,1)");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}

void AT_CmdFunc_CZLCDBRIGHTNESS(AT_CMD_PARA *pParam)
{
    if(init_flag == false)
    {
        if(0 == KING_LcdInit(0, NULL))
        {
            init_flag = true;
        }
        else
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
    }
    switch (pParam->iType)
    {
    case AT_CMD_SET:
        {
            uint32 value = 0;
            bool paramok = true;
            value = at_ParamUintInRange(pParam->params[0], 0, 100, &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }

            if(1 == enable)
            {
                if(0 != KING_LcdBrightnessSet(0, value))
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                at_CmdRespOK(pParam->engine);
            }
            else
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        }
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CZLCDBRIGHTNESS: (0-100)");
        at_CmdRespOK(pParam->engine);
        break;
        
    case AT_CMD_READ:
        {   
            uint32_t level = 0;
            char rsp[32];

            if(0 != KING_LcdBrightnessGet(0 , &level))
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            sprintf(rsp, "+CZLCDBRIGHTNESS: %d", level);
            at_CmdRespInfoText(pParam->engine, (uint8_t *)rsp);
            at_CmdRespOK(pParam->engine);
        }
        break;
    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}
