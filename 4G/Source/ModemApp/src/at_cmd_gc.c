#include "at_common.h"
#include "at_dispatch.h"
#include "at_errcode.h"
#include "at_module.h"
#include "at_define.h"
#include "KingNet.h"
#include "KingPowerManage.h"
#include "KingRtc.h"
#include "KingGpio.h"
#include "at_lp_ctrl.h"
#include "kingfilesystem.h"

#define in_range(c, lo, up)  ((uint8)c >= lo && (uint8)c <= up)
#define isdigit(c)           in_range(c, '0', '9')
#define ADC_CHANNEL_0  2
#define ADC_CHANNEL_1  3

extern UINT8 g_rspStr[];
static uint32_t usb_baud = 0;

BOOL AT_TimeFormat_isAvailable(UINT8 *pString)
{
    UINT8 *p;
    UINT8 i, iLen;

    if (NULL == pString)
    {
        return FALSE;
    }

    p = pString;

    iLen = (UINT8)AT_StrLen(p);

    // "yy/mm/dd,hh:mm:ss"
    // 01234567890123456

    if (iLen > 17)
    {
        if ((p[17] != '+'))
        {
            if ((p[17] != '-'))
                return FALSE;
        }

        if ((p[2] != '/') || (p[5] != '/') || (p[8] != ',') || (p[11] != ':') || (p[14] != ':'))
            return FALSE;

        for (i = 0; i < iLen; i++)
        {
            if ((i == 2) || (i == 5) || (i == 8) || (i == 11) || (i == 14) || (i == 17))
            {
            }
            else
            {
                if (!(isdigit(p[i])))
                    return FALSE;
            }
        }
    }
    else
    {
        if ((p[2] != '/') || (p[5] != '/') || (p[8] != ',') || (p[11] != ':') || (p[14] != ':'))
            return FALSE;

        for (i = 0; i < iLen; i++)
        {
            if ((i == 2) || (i == 5) || (i == 8) || (i == 11) || (i == 14))
            {
            }
            else
            {
                if (!(isdigit(p[i])))
                    return FALSE;
            }
        }
    }

    return TRUE;
}

static INT32 GetCalendarInfo(char *time_string, CALENDAR_TIME *calendar)
{
    UINT32 ret = 0;
    UINT8 temp;
    char *p;
    UINT8 year;
    UINT8 month;
    CALENDAR_TIME old_time;

    if (TRUE == AT_TimeFormat_isAvailable((UINT8 *)time_string))
    {
        // "yy/mm/dd,hh:mm:ss"
        //year
        temp = (UINT8) atoi(time_string);
        if (0 == IS_VALID_YEAR(temp))
        {
            goto exit;
        }
        else
        {
            year = temp;
            calendar->yy = BIN2BCD(temp);
        }

        //month
        p = strchr(time_string, '/');
        temp = (UINT8) atoi(p + 1);
        if (0 == IS_VALID_MONTH(temp))
        {
            goto exit;
        }
        else
        {
            month = temp;
            calendar->MM = BIN2BCD(temp);
        }

        //day
        p = strchr(p + 1, '/');
        temp = (UINT8) atoi(p + 1);
        if (0 == IS_VALID_DAY(temp))
        {
            goto exit;
        }
        else
        {
            switch(month)
            {
            case 2:
                if (IS_LEAP_YEAR(year))
                {
                    if (temp > 29)
                    {
                        goto exit;
                    }
                }
                else
                {
                    if (temp > 28)
                    {
                        goto exit;
                    }
                }
                break;

            case 4:
            case 6:
            case 9:
            case 11:
                if (31 == temp)
                {
                    goto exit;
                }
                break;

            default:
                break;
            }
            calendar->dd = BIN2BCD(temp);
        }

        //hour
        p = strchr(p + 1, ',');
        temp = (UINT8) atoi(p + 1);
        if (0 == IS_VALID_HOUR(temp))
        {
            goto exit;
        }
        else
        {
            calendar->hh = BIN2BCD(temp);
        }

        //minute
        p = strchr(p + 1, ':');
        temp = (UINT8) atoi(p + 1);
        if (0 == IS_VALID_MINUTE(temp))
        {
            goto exit;
        }
        else
        {
            calendar->mm = BIN2BCD(temp);
        }

        //second
        p = strchr(p + 1, ':');
        temp = (UINT8) atoi(p + 1);
        if (0 == IS_VALID_SECOND(temp))
        {
            goto exit;
        }
        else
        {
            calendar->ss_h = temp / 10;
            calendar->ss_l = temp % 10;
        }

        //timezone
        if (strlen(time_string) > 17)
        {
            if ('+' == time_string[17])
            {
                calendar->ew = 0;
            }
            else
            {
                calendar->ew = 1;
            }

            temp = (UINT8) atoi(time_string + 18);
            if ((('+' == time_string[17]) && (0 <= temp) && (temp <= 56)) ||
                (('-' == time_string[17]) && (0 <= temp) && (temp <= 48)))
            {
                calendar->tz = temp;
            }
            else
            {
                goto exit;
            }
        }
        else
        {
            //no timezone info
            if (SUCCESS == KING_LocalTimeGet(&old_time))
            {
                calendar->ew = old_time.ew;
                calendar->tz = old_time.tz;
            }
            else
            {
                goto exit;
            }
        }

        calendar->ms = 0;
        ret = 1;
    }

exit:
    return ret;
}

VOID AT_CmdFunc_CCLK(AT_CMD_PARA *pParam)
{
    UINT8 iCount = 0;
    UINT8 cfw_para[24] = {0};
    UINT16 len = 24;
    INT32 iResult;
    CALENDAR_TIME time;

    AT_MemSet(cfw_para, 0x00, 20);

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

        //AT_Util_TrimAll(cfw_para);

        if (!AT_StrLen(cfw_para) || (AT_StrLen(cfw_para) > 20))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_INVALID_CHAR_INTEXT);
            return;
        }

        if (0 == GetCalendarInfo((char *)cfw_para, &time))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }

        if (SUCCESS == KING_LocalTimeSet(&time))
        {
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        break;

    case AT_CMD_TEST:
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_READ:
        if (FAIL == KING_LocalTimeGet(&time))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            return;
        }

        sprintf((char *)g_rspStr, "+CCLK: \"%02x/%02x/%02x,%02x:%02x:%x%x%c%02d\"", time.yy, time.MM, time.dd,
                            time.hh, time.mm, time.ss_h, time.ss_l, (0 == time.ew) ? '+' : '-',
                            time.tz);
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }

    return;
}

VOID AT_CmdFunc_E(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT8 iCount = 0;
    UINT8 atl_para;
    UINT8 cfw_para;
    UINT16 len = 0;

    if (pParam->iType == AT_CMD_EXE)
    {
        eResult = AT_Util_GetParaCount(pParam->pPara, &iCount);

        if (eResult == 0)
        {
            if ((iCount != 1) && (iCount != 0))
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            else if (iCount == 0)
            {
                gATCurrenteCommandEchoMode = 1;
                at_CmdRespOK(pParam->engine);
                return;
            }
            else
            {
                len = 1;

                if (0 == AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &atl_para, &len))
                {
                    cfw_para = (UINT8)atl_para;

                    if (cfw_para > 1)
                    {
                        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                        return;
                    }

                    gATCurrenteCommandEchoMode = atl_para;

                    at_CmdRespOK(pParam->engine);
                    return;
                }
                else
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    return;
                }
            }
        }
        else
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            return;
        }
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        return;
    }
}

VOID AT_CmdFunc_CFUN(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    uint32_t funValutList[3] = {0, 1, 4};
    uint32_t fun = 0, rst = 0;
    char OutStr[30] = {0};

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if ((1 != pParam->paramCount) && (2 != pParam->paramCount))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        // get first param <fun>
        fun = at_ParamDefUintInList(pParam->params[0], 1, funValutList, 3, &paramok);
        if (true != paramok)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        // get second param <rst>
        if (pParam->paramCount == 2)
        {
            rst = at_ParamDefUintInRange(pParam->params[1], 1, 0, 1, &paramok);
            if (true != paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            }
        }

        switch (fun)
        {
        case 0:
        case 4:
            {
                if(rst)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
                }
                else
                {
                    if (SUCCESS == KING_NetAirplaneModeSet(1))
                    {
                        at_CmdRespOK(pParam->engine);
                    }
                    else
                    {
                        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    }
                }
            }
            break;

        case 1:
            {
                if (rst)
                {
                    at_CmdRespOK(pParam->engine);
                    KING_PowerReboot();
                }
                else
                {
                    if (SUCCESS == KING_NetAirplaneModeSet(0))
                    {
                        at_CmdRespOK(pParam->engine);
                    }
                    else
                    {
                        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                    }
                }
            }
            break;

        default:
            break;
        }
        break;
    
    case AT_CMD_TEST:
        strcpy(OutStr, "+CFUN: (0,1,4),(0,1)");
        at_CmdRespInfoText(pParam->engine, (uint8_t *)OutStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_READ:
        if (SUCCESS == KING_NetAirplaneModeGet(&fun))
        {
            sprintf(OutStr, "+CFUN: %d", !fun);
            at_CmdRespInfoText(pParam->engine, (uint8_t *)OutStr);
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        return;
    
    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        return;
    }
}

VOID AT_CmdFunc_IPR(AT_CMD_PARA *pParam)
{
    AT_DEVICE_T *device = at_GetDeviceByChannel(pParam->nDLCI);
    uint8_t devno = 0;

    devno = at_DeviceGetNumber(device);

    if ((devno != 0) && (devno != 1)) // 0:uart, 1:usb
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED));

    if (1 == devno)
    {
        if (0 == usb_baud)
        {
            usb_baud = gATCurrentu32nBaudRate;
        }
    }

    if (pParam->iType == AT_CMD_SET)
    {
        if (pParam->paramCount != 1)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        bool paramok = true;
        uint32_t baud = at_ParamUint(pParam->params[0], &paramok);
        if (!paramok)
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        if (!at_DeviceIsBaudSupported(device, baud))
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));

        
        if (1 == devno)
        {
            if (usb_baud != baud)
            {
                usb_baud = baud;  
                at_CmdDeviceSetFormatNeeded(pParam->engine);
            }
        }
        else
        {
            if (gATCurrentu32nBaudRate != baud)
            {
                gATCurrentu32nBaudRate = baud;  
                at_CmdDeviceSetFormatNeeded(pParam->engine);
            }
        }

        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_TEST)
    {
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+IPR: (4800,9600,19200,38400,57600,115200),(2400,4800,9600,14400,19200,28800,33600,38400,57600,115200,230400,460800,921600,1843200)");
        at_CmdRespOK(pParam->engine);
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        char rsp[32];
        if (1 == devno)
        {
            if (0 == usb_baud)
            {
                usb_baud = gATCurrentu32nBaudRate;
            }
            AT_Sprintf(rsp, "+IPR: %u", usb_baud);
        }
        else
        {
            AT_Sprintf(rsp, "+IPR: %u", gATCurrentu32nBaudRate);
        }
        at_CmdRespInfoText(pParam->engine, (uint8_t *)rsp);
        at_CmdRespOK(pParam->engine);
    }
    else
    {
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

VOID AT_CmdFunc_AndW(AT_CMD_PARA *pParam)
{
    INT32 eResult;
    UINT16 iCount = 0;
    BOOL err_code;
    uint32_t profile = 0;
    bool paramok = true;

    if (pParam->iType == AT_CMD_EXE)
    {
        eResult = AT_Util_GetParaCount(pParam->pPara, (UINT8 *)&iCount);

        if (eResult == 0)
        {
            switch (iCount)
            {
            case 1:
                profile = at_ParamUint(pParam->params[0], &paramok);
                if ((!paramok) || (0 != profile))
                {
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }
                /* !!! continue to save !!! */
            case 0:
                err_code = at_CfgSetAtSettings(profile);

                if (TRUE == err_code)
                {
                    at_CmdRespOK(pParam->engine);
                }
                else
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
                }
                break;

            default:
                at_CmdRespError(pParam->engine);
                break;
            }
        }
        else
        {
            at_CmdRespError(pParam->engine);
        }
    }
    else
    {
        at_CmdRespError(pParam->engine);
    }

    return;
}

VOID AT_CmdFunc_QPOWD(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    UINT32 mode = 0;

    switch(pParam->iType)
    {
    case AT_CMD_EXE:
    case AT_CMD_SET:
        if ((1 != pParam->paramCount) && (0 != pParam->paramCount))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        mode = at_ParamDefUintInRange(pParam->params[0], 1, 0, 1, &paramok);
        if (true != paramok)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        if (1 == mode)
        {
            AT_Sprintf((char *)g_rspStr, "POWERED DOWN");
            at_CmdRespOK(pParam->engine);
            at_CmdRespInfoText(pParam->engine, g_rspStr);
        }

        KING_PowerOff();
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+QPOWD: (0,1)");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
        break;
    }
}

VOID AT_CmdFunc_QSCLK(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    uint32_t sclk;

    switch(pParam->iType)
    {
    case AT_CMD_SET:
        if (pParam->paramCount != 1)
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        sclk = at_ParamUintInRange(pParam->params[0], 0, 2, &paramok);
        if (!paramok)
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        //TODO:
        //1. DTR not support?
        //2. Timer, WakeLock, WakeUnlock
        if (AT_DEVICE_LP_BY_DTR == (AT_DEVICE_LP_MODE_T)sclk)
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
        }

        at_LpCtrlSetMode((AT_DEVICE_LP_MODE_T)sclk);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_READ:
        sprintf((char *)g_rspStr, "+QSCLK: %u", (uint32_t)at_LpCtrlGetMode());
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (UINT8 *)"+QSCLK: (0,2)");
        at_CmdRespOK(pParam->engine);
        break;
    
    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

VOID AT_CmdFunc_QURCCFG(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    typedef struct urc_info {
        AT_URC_PORT_TYPE type;
        char info[20];
    } URC_INFO_T;
    URC_INFO_T infos[AT_URC_PORT_MAX] = {
        {AT_URC_PORT_UART1, "uart1"},
        {AT_URC_PORT_USBAT, "usbat"}
    };
    const uint8_t *p;
    uint8_t count = sizeof(infos)/sizeof(infos[0]);
    int i;

    switch(pParam->iType)
    {
    case AT_CMD_SET:
        if ((1 != pParam->paramCount) && (2 != pParam->paramCount))
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        p = at_ParamStr(pParam->params[0], &paramok);
        if (!paramok || strcasecmp((char *)p, "urcport"))
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        else
        {
            if (1 == pParam->paramCount)
            {
                sprintf((char *)g_rspStr, "+QURCCFG: \"urcport\",\"%s\"", infos[gAtCurAsSetting.urcport].info);
                at_CmdRespInfoText(pParam->engine, g_rspStr);
                at_CmdRespOK(pParam->engine);
            }
            else
            {
                p = at_ParamStr(pParam->params[1], &paramok);
                if (!paramok)
                {
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
                }

                for (i = 0; i < count; i++)
                {
                    if (!strcasecmp((char *)p, infos[i].info))
                    {
                        at_SetUrcPort(infos[i].type);
                        at_CmdRespOK(pParam->engine);
                        return;
                    }
                }
                
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }
        }        
        break;

    case AT_CMD_READ:
        sprintf((char *)g_rspStr, "+QURCCFG: \"urcport\",\"%s\"", infos[gAtCurAsSetting.urcport].info);
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (UINT8 *)"+QURCCFG: \"urcport\",(\"usbat\",\"uart1\")");
        at_CmdRespOK(pParam->engine);
        break;
    
    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void AT_Adc_Init(void)
{
    int ret = 0;
    ADC_CFG_S adcCfg;

    KING_GpioInit();

    memset(&adcCfg,0x00,sizeof(ADC_CFG_S));
    adcCfg.vdd_ref = ADC_VDD_REF_3;
    ret = KING_AdcConfig(ADC_CHANNEL_0, &adcCfg);
    if (FAIL == ret)
    {
        ATLOGE("%s: KING_AdcConfig errcode=0x%x", __FUNCTION__,KING_GetLastErrCode());
    }
    ret = KING_AdcConfig(ADC_CHANNEL_1, &adcCfg);
    if (FAIL == ret)
    {
        ATLOGE("%s: KING_AdcConfig errcode=0x%x", __FUNCTION__,KING_GetLastErrCode());
    }
}

void AT_CmdFunc_QADC(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    uint32 port = 0;
    uint8 status = 0;
    uint32 adc_vol = 0;
    int ret = -1;

    switch(pParam->iType)
    {
    case AT_CMD_SET:
        if (pParam->paramCount != 1)
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        port = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (!paramok)
        {
            ATLOGE("%s: port invalid", __FUNCTION__);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }
        
        if(0 == port)
        {
            port = ADC_CHANNEL_0;
        }
        else if(1 == port)
        {
            port = ADC_CHANNEL_1;
        }

        ret = KING_AdcGet(port, &adc_vol);
        if (-1 == ret)
        {
            ATLOGE("%s: adc get fail errcode=0x%x", __FUNCTION__, KING_GetLastErrCode());
            status = 0;
            adc_vol = 0;
        }
        else
        {
            status = 1;
        }
        sprintf((char *)g_rspStr, "+QADC: %d,%d", status, adc_vol);
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (UINT8 *)"+QADC: (0,1)");
        at_CmdRespOK(pParam->engine);
        break;
    
    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void AT_CmdFunc_CZEDUMPEN(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    uint32 val = 0;
    uint8 reboot_en = 1;
    uint32 size = 1;
    int ret = -1;

    switch(pParam->iType)
    {
    case AT_CMD_SET:
        if (pParam->paramCount != 1)
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        val = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (!paramok)
        {
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
        }

        //allow dump
        if (1 == val)
        {
            reboot_en = 0;
        }

        ret = KING_SysCfg(SYS_CFG_SET, SYS_CFG_ID_ASSERT_REBOOT, &reboot_en, &size);
        if (-1 == ret)
        {
            ATLOGE("%s: errcode=0x%x", __FUNCTION__, KING_GetLastErrCode());
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }

        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_READ:
        ret = KING_SysCfg(SYS_CFG_GET, SYS_CFG_ID_ASSERT_REBOOT, &reboot_en, &size);
        if (-1 == ret)
        {
            ATLOGE("%s: errcode=0x%x", __FUNCTION__, KING_GetLastErrCode());
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
        }

        if (0 == reboot_en)
        {
            val = 1;
        }
        sprintf((char *)g_rspStr, "+CZEDUMPEN: %d", val);
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (UINT8 *)"+CZEDUMPEN: (0,1)");
        at_CmdRespOK(pParam->engine);
        break;
    
    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_ALLOWED);
    }
}

void AT_CmdFunc_CZLDO(AT_CMD_PARA *pParam)
{
    bool paramok1 = true;
    bool paramok2 = true;
    bool paramok3 = true;
    char rsp[64];
    int ret = -1;
    
    switch (pParam->iType)
    {
        case AT_CMD_SET:
            {
                LDO_ID_E ldo_id;
                LDO_VOL_E ldo_vol;
                bool ldo_onoff = true;

                ldo_id = at_ParamUintInRange(pParam->params[0], 0, 2, &paramok1);
                ldo_vol = at_ParamUintInRange(pParam->params[1], 1800, 3300, &paramok2);
                ldo_onoff = at_ParamUintInRange(pParam->params[2], 0, 1, &paramok3);
                if (!paramok1 || !paramok2 || !paramok3)
                {
                    at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                    break;
                }
                
                ret = KING_LdoConfigSet(ldo_id, ldo_vol, ldo_onoff);
                if(ret == -1)
                {
                    ATLOGE("%s: LDC SET FAIL", __FUNCTION__);
                    AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL));
                }

                ATLOGE("%s: LDC SET SUCCESS", __FUNCTION__);
                at_CmdRespOK(pParam->engine);
            }
            break;
        case AT_CMD_TEST:
            sprintf(rsp, "+CZLDO: (0-2),(1800,2800,3000,3300),(0-1)");
            at_CmdRespInfoText(pParam->engine, (uint8_t *)rsp);
            at_CmdRespOK(pParam->engine);
            break;
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;
    }
}


static int fsReadAndWrite(char *pFileName)
{
    int ret = SUCCESS;
    FS_HANDLE FileHandle = NULL;
    uint32 fileSize = 0, readLen = 0, size = 0, writeSize = 0;
    FS_HANDLE sdFsHandle = NULL;
    uint8 buf[1024] = {0};
    char fileName[256] = {0};
    char *tempFileName = NULL;
    
    ret = KING_FsFileCreate(pFileName, FS_MODE_READ | FS_MODE_OPEN_EXISTING, 0, 0, &FileHandle);
    if (ret == -1 || FileHandle == NULL)
    {
        ATLOGE("creat new file fail errorcode: 0x%X\r\n", KING_GetLastErrCode());
        return -1;
    }
    ret = KING_FsFileSizeGet(FileHandle, &fileSize);
    if(ret == -1)
    {
        ATLOGE("get file size fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        return -1;
    }
    tempFileName = strrchr(pFileName, '/');
    if (tempFileName == NULL)
    {
        tempFileName = pFileName;
    }
    else
    {
        tempFileName++;
    }
    sprintf(fileName, "/mnt/sd/%s", tempFileName);
    ret = KING_FsFileCreate(fileName, FS_MODE_WRITE|FS_MODE_CREATE_ALWAYS, 0, 0, &sdFsHandle);
    if (ret == -1 || sdFsHandle == NULL)
    {
        ATLOGE("creat new file fail errorcode: 0x%X\r\n", KING_GetLastErrCode());
        return -1;
    }  
    while ( readLen < fileSize )
    {
        memset(buf, 0x00, 1024);
        ret = KING_FsFileRead(FileHandle, buf, 1024, &size);
        if(ret == -1)
        {
            ret = -1;
            break;
        }
        ret = KING_FsFileWrite(sdFsHandle, buf, size, &writeSize);
        if(ret == -1 || size != writeSize)
        {
            ret = -1;
            break;        
        }       
        readLen += size;
    }

    KING_FsCloseHandle(FileHandle);
    KING_FsCloseHandle(sdFsHandle);
    return ret;
}

void AT_CmdFunc_CZEM2SD(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_SET:
        {
            bool paramok = true;
            const uint8_t *pFileName = NULL;

            if (pParam->paramCount != 1)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            } 

            pFileName = at_ParamOptStr(pParam->params[0], &paramok);
            if (!paramok)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }
            if (strlen((char *)pFileName) > 256)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                return;
            }  
            
            if (fsReadAndWrite((char *) pFileName) == -1)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                at_CmdRespOK(pParam->engine);
            }
        }
            break;

        case AT_CMD_TEST:
            at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CZEM2SD: <filename>");
            at_CmdRespOK(pParam->engine);
            break;
            
        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;        
    }
}

void AT_CmdFunc_SET7SRESET(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    uint32_t enable;
    uint32_t size = 1;
    uint8_t cfg;

    switch (pParam->iType)
    {
        case AT_CMD_SET:
            if (pParam->paramCount != 1)
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                break;
            } 

            enable = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
            if (!paramok)
            {
                AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID));
            }

            cfg = (uint8_t)enable;
            if (FAIL == KING_SysCfg(SYS_CFG_SET, SYS_CFG_ID_PWR_KEY_7S_RESET, &cfg, &size))
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                at_CmdRespOK(pParam->engine);
            }
            break;

        case AT_CMD_READ:
            KING_SysCfg(SYS_CFG_GET, SYS_CFG_ID_PWR_KEY_7S_RESET, &cfg, &size);
            sprintf((char *)g_rspStr, "+SET7SRESET: %d", cfg);
            at_CmdRespInfoText(pParam->engine, g_rspStr);
            at_CmdRespOK(pParam->engine);
            break;

        case AT_CMD_TEST:
            at_CmdRespInfoText(pParam->engine, (uint8_t *)"+SET7SRESET: (0,1)");
            at_CmdRespOK(pParam->engine);
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_NOT_SURPORT);
            break;        
    }
}

