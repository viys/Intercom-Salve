#include "at_common.h"
#include "at_errcode.h"
#include "at_module.h"
#include "KingPlat.h"
#include "KingCSTD.h"
#include "app_version.h"
#include "at_utility.h"

extern UINT8 g_rspStr[];

void AT_CmdFunc_GMR(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
    case AT_CMD_EXE:
        sprintf((char *)g_rspStr, "%s", KING_PlatformInfo());
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}

void AT_CmdFunc_CZESVER(AT_CMD_PARA *pParam)
{
    char str[80];
    char *begin;
    char *end;
    
    switch (pParam->iType)
    {
    case AT_CMD_EXE:
        sprintf(str, "%s", KING_PlatformInfo());
        begin = strchr(str, '_');
        end = strchr(begin + 1, '_');
        *end = '\0';
        sprintf((char *)g_rspStr, "+CZESVER: %s", begin + 1);
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}

VOID AT_CmdFunc_CGSN(AT_CMD_PARA *pParam)
{
    UINT8 nImei[15];
    UINT8 i;
    int len = 0;

    switch (pParam->iType)
    {
    case AT_CMD_EXE:
        if (SUCCESS == KING_GetSysImei(nImei))
        {
            for (i = 0; i < 15; i++)
            {
                len += snprintf((char *)g_rspStr + len, AT_RSP_STR_LEN - len, "%c", nImei[i]);
            }
            at_CmdRespInfoText(pParam->engine, g_rspStr);
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        }
        break;

    case AT_CMD_TEST:
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }

    return;
}

void AT_CmdFunc_WSOP(AT_CMD_PARA *pParam)
{
    UINT8 iCount = 0;
    UINT8 cfw_para[10] = {0};
    UINT16 len = 10;
    INT32 iResult;
    
    if (pParam->pPara == NULL)
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_MEMORY_FAILURE);
        return;
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        iResult = AT_Util_GetParaCount(pParam->pPara, &iCount);

        if (iResult != 0 || (iCount != 1))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        iResult = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_STRING, (PVOID)cfw_para, &len);

        if (0 != iResult || len == 0)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_INVALID_CHAR_INTEXT);
            return;
        }

        if (0 == strcmp((char *)cfw_para, "ATI"))
        {
            sprintf((char *)g_rspStr, "%s_%d.%d.%d.%d_%02d%02d%02d%02d_%s", CZ_PROJECT_NAME, CZ_SW_MAJOR, CZ_SW_MINOR,
                CZ_SW_BUILDNUMBER, CZ_SVN_REV, CZ_BUILD_YEAR, CZ_BUILD_MONTH, CZ_BUILD_DAY, CZ_BUILD_HOUR,
                CZ_BUILD_FLAG);
            at_CmdRespInfoText(pParam->engine, g_rspStr);
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        break;

    case AT_CMD_TEST:
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }
}
void AT_CmdFunc_CZCHIPID(AT_CMD_PARA *pParam)
{
    int ret;
    uint8 uuid[16];
    uint8 offset = 0;
    switch (pParam->iType)
    {
    case AT_CMD_EXE:
        ret = KING_GetChipId(uuid);
        if (-1 == ret)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        memset(g_rspStr, 0x00, AT_RSP_STR_LEN);
        offset += sprintf((char *)g_rspStr + offset, "+CZCHIPID: ");
        for (int i = 0; i < 16; i++)
        {
            offset += sprintf((char *)g_rspStr + offset, "%02X", uuid[i]);
        }
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
        break;
    }    
}


