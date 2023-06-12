#include "At_common.h"
#include "At_errcode.h"
#include "At_module.h"
#include "at_utils.h"
#include "at_cmd_utils.h"

#include "KingDef.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingErrCode.h"
#include "KingBT.h"
#include "KingFileSystem.h"

typedef enum
{
    BT_DISABLED = 0,
    BT_ENABLED,
    BT_ADVING,
    BT_SCANING,
    BT_SCANSTOP,
    BT_CLI_CONNECTED,
    BT_SRV_CONNECTED,
    BT_STATUS_MAX = 0x7FFFFFFF
} BT_STATUS_E;

#define BT_MAC_ADDR_STR_LEN 17 //XX:XX:XX:XX:XX:XX
#define IBEACON_CONFIG_FILE "/ibeacon.cfg"

#define PKG_HEADER_LEN  3

extern UINT8 g_rspStr[];

static AT_CMD_ENGINE_T *at_engine = NULL;
static bool g_btinit = false;
static BT_IBEACON_DATA_T g_beacon;
static BT_ADV_PARAM_T g_advcfg;
static bool g_adven = false;
static uint32 g_mtu = 0;
static uint8 adv_data[31] = {0};
static uint32 adv_len = 0;
static BT_STATUS_E bt_status;

static void BTCallback(uint32 eventID, void* pData, uint32 len)
{
    uint32 i;
    uint8 *p;
    bool urc = false;
    size_t offset;
    uint8 conncet_state;
    p = (uint8*)pData;

    KING_SysLog("BTCallback: id = 0x%x, data = %p, len = %d", eventID, pData, len);
/*
    if (len != 0)
    {
        for (i = 0; i <len; i++)
        {
            KING_SysLog("BTCallback: data[%d] = 0x%02x", i, p[i]);
        }
    }
*/
    switch(eventID)
    {
    case BT_BLE_CLI_SCAN_RSP_IND:
        if(bt_status == BT_SCANING)
        {
            BT_SCAN_INFO *scan_info = (BT_SCAN_INFO *)pData;
            if (scan_info->name_length > 0)
            {
                urc = true;
                sprintf((char *)g_rspStr, "+CBLESCANIND: %s,%d,\"%02X:%02X:%02X:%02X:%02X:%02X\",%d", scan_info->data, scan_info->addr_type, 
                                scan_info->bdAddress.addr[0],scan_info->bdAddress.addr[1],scan_info->bdAddress.addr[2],scan_info->bdAddress.addr[3],
                                scan_info->bdAddress.addr[4],scan_info->bdAddress.addr[5],scan_info->rssi);
            }
        }
      break;

    case BT_BLE_CLI_CONNECT_IND:
        urc = true;
        conncet_state = *(uint8 *)pData;
        KING_SysLog("BT_BLE_CLI_CONNECT_IND %d",conncet_state);
        if(conncet_state == 0)
        {
            sprintf((char *)g_rspStr, "+CBTIND: 0");
        }
        else
        {
            sprintf((char *)g_rspStr, "+CBTIND: 1");
            bt_status = BT_CLI_CONNECTED;
        }
        break;

    case BT_BLE_CLI_DISCONNECT_IND:
        urc = true;
        sprintf((char *)g_rspStr, "+CBTIND: 2");
        g_mtu = 0;
        bt_status = BT_SCANSTOP;
        break;

    case BT_BLE_SRV_CONNECT_IND:
        urc = true;        
        sprintf((char *)g_rspStr, "+CBTIND: %d,6,\"%02X:%02X:%02X:%02X:%02X:%02X\"",
            eventID, p[0], p[1], p[2], p[3], p[4], p[5]);
        bt_status = BT_SRV_CONNECTED;
        break;

    case BT_BLE_SRV_DISCONNECT_IND:
        urc = true;
        sprintf((char *)g_rspStr, "+CBTIND: %d", eventID);
        g_mtu = 0;
        if(bt_status == BT_SRV_CONNECTED)
            bt_status = BT_ADVING;
        break;

    case BT_BLE_SEND_DATA_COMPLETE_IND:
        urc = true;
        sprintf((char *)g_rspStr, "+CBTIND: %d", eventID);
        if(bt_status == BT_SRV_CONNECTED)
            at_CmdRespOK(at_engine);
        break;

    case BT_BLE_RECV_DATA_IND:
        urc = true;
        sprintf((char *)g_rspStr, "+CBTIND: %d,%d,\"", eventID, len);
        offset = strlen((char *)g_rspStr);
        for (i = 0; i < len; i++)
        {
            offset += sprintf((char *)g_rspStr + offset, "%02X", p[i]);
        }
        strcat((char *)g_rspStr,"\"");
        break;

    case BT_BLE_MTU_IND:
        urc = true;
        g_mtu = *(uint32 *)pData;
        sprintf((char *)g_rspStr, "+CBTIND: %d,%d", eventID, g_mtu);
        break;

    case BT_CHIP_RESET_IND:
        urc = true;
        sprintf((char *)g_rspStr, "+CBTIND: %d", eventID);
        break;

    default:
        break;
    }

    if (true == urc)
    {
        at_CmdReportUrcText(0, g_rspStr);
    }
}

void Bluetooth_Init(void)
{
    KING_RegNotifyFun(DEV_CLASS_BT, 0, BTCallback);

    if (SUCCESS == KING_BtInit())
    {
        g_btinit = true;
    }
}

void AT_CmdFunc_CBTENABLE(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    uint32_t enable;
    uint32_t status;

    if (false == g_btinit)
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }

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
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        if(FAIL == KING_BtStateGet(&status))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            break;
        }

        if(status == enable)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            break;
        }

        if (SUCCESS == KING_BtEnable(enable))
        {
            if(enable)
                bt_status = BT_ENABLED;
            else
                bt_status = BT_DISABLED;
            at_engine = pParam->engine;
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            ATLOGE("enable bt fail: 0x%x", KING_GetLastErrCode());
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        break;

    case AT_CMD_READ:
        KING_BtStateGet(&enable);
        sprintf((char *)g_rspStr, "+CBTENABLE: %d", enable);
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CBTENABLE: (0,1)");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }
}

void AT_CmdFunc_CBTNAME(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    char name[BT_DEVICE_NAME_SIZE + 1] = {0};
    uint32 len = 0;
    const uint8_t *p;

    if (false == g_btinit)
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if (pParam->paramCount != 1)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        p = at_ParamStr(pParam->params[0], &paramok);
        if (!paramok || (strlen((char *)p) > BT_DEVICE_NAME_SIZE))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        len = strlen((char *)p);
        if (SUCCESS == KING_BtNameSet((char *)p, len))
        {
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            ATLOGE("set bt name fail: 0x%x", KING_GetLastErrCode());
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        break;

    case AT_CMD_READ:
        KING_BtNameGet(name, &len);
        sprintf((char *)g_rspStr, "+CBTNAME: \"%s\"", name);
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CBTNAME: \"<name>\"");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }
}


bool BT_MacStr2Mac(const uint8_t *str, BT_ADDR_T *addr)
{
    int i;
    bool ret = true;
    char mac[6];

    memset(mac, 0x00, 6);

    //check if string format is valid
    for (i = 0; i < 6; i++)
    {
        if (!atUtils_ConvertHexToBin((char *)str + i * 3, 2, mac + i))
        {
            ret = false;
            break;
        }

        if (i < 5)
        {
            if (':' != str[i * 3 + 2])
            {
                ret = false;
                break;
            }
        }
    }

    if (true == ret)
    {
        for (i = 0; i < 6; i++)
        {
            addr->addr[i] = (uint8)mac[i];
        }
    }

    return ret;
}

void AT_CmdFunc_CBTMAC(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    BT_ADDR_T addr;
    const uint8_t *p;

    if (false == g_btinit)
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if (pParam->paramCount != 1)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        p = at_ParamStr(pParam->params[0], &paramok);
        if (!paramok || (strlen((char *)p) != BT_MAC_ADDR_STR_LEN))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        if (false == BT_MacStr2Mac(p, &addr))
        {
            ATLOGE("%s: invalid string format", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
        }
        else
        {
            if (SUCCESS == KING_BtAddrSet(BT_ADDR_PUBLIC, &addr))
            {
                at_CmdRespOK(pParam->engine);
            }
            else
            {
                ATLOGE("%s: set fail: 0x%x", __FUNCTION__, KING_GetLastErrCode());
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
        }
        break;

    case AT_CMD_READ:
        if (SUCCESS == KING_BtAddrGet(BT_ADDR_PUBLIC, &addr))
        {
            sprintf((char *)g_rspStr, "+CBTMAC: \"%02X:%02X:%02X:%02X:%02X:%02X\"",
                    addr.addr[0], addr.addr[1], addr.addr[2], addr.addr[3], addr.addr[4], addr.addr[5]);
            at_CmdRespInfoText(pParam->engine, g_rspStr);
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            ATLOGE("%s: get fail: 0x%x", __FUNCTION__, KING_GetLastErrCode());
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CBTMAC: \"xx:xx:xx:xx:xx:xx\"");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }
}

void AT_CmdFunc_CBTIBEACON(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    const uint8_t *p;
    uint8 uuid[16] = {0};
    uint32 maj = 0;
    uint32 min = 0;
    uint8 str_uuid[33] = {0};
    BT_IBEACON_DATA_T beacon;

    if (false == g_btinit)
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if (pParam->paramCount != 3)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        p = at_ParamStr(pParam->params[0], &paramok);
        if (!paramok || (strlen((char *)p) != 32))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        paramok = atUtils_ConvertHexToBin((char *)p, 32, (char*)uuid);

        if (!paramok)
        {
            ATLOGE("%s: uuid invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        maj = at_ParamUintInRange(pParam->params[1], 0, 0xFFFF, &paramok);
        if (!paramok)
        {
            ATLOGE("%s: maj invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }
        
        min = at_ParamUintInRange(pParam->params[2], 0, 0xFFFF, &paramok);
        if (!paramok)
        {
            ATLOGE("%s: min invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        memcpy(beacon.uuid, uuid, 16);
        memcpy(beacon.major, &maj, 2);
        memcpy(beacon.minor, &min, 2);  
        if (SUCCESS == KING_BtIbeaconDataSet(&beacon))
        {
            memcpy(g_beacon.uuid, uuid, 16);
            memcpy(g_beacon.major, &maj, 2);
            memcpy(g_beacon.minor, &min, 2); 
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            ATLOGE("%s: set fail: 0x%x", __FUNCTION__, KING_GetLastErrCode());
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        break;

    case AT_CMD_READ:
        atUtils_ConvertBinToHex(g_beacon.uuid, 16, (uint8 *)str_uuid, 0);
        memcpy(&maj, g_beacon.major, 2);        
        memcpy(&min, g_beacon.minor, 2);        

        sprintf((char *)g_rspStr, "+CBTIBEACON: \"%s\",%u,%u", str_uuid, maj, min);
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CBTIBEACON: \"<uuid>\",(0-65535),(0-65535)");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }
}

void AT_CmdFunc_CBTADVCFG(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    BT_ADV_PARAM_T adv;

    if (false == g_btinit)
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if (pParam->paramCount != 5)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        adv.min_time = at_ParamUintInRange(pParam->params[0], 32, 16384, &paramok);
        if (!paramok)
        {
            ATLOGE("%s: min invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        adv.max_time = at_ParamUintInRange(pParam->params[1], 32, 16384, &paramok);
        if ((!paramok) || (adv.min_time > adv.max_time))
        {
            ATLOGE("%s: max invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        adv.type = at_ParamUintInRange(pParam->params[2], 0, 4, &paramok);
        if ((!paramok) || (BT_ADV_CONN_HIGH_DUTY_DIRECT == adv.type) || (BT_ADV_CONN_LOW_DUTY_DIRECT == adv.type))
        {
            ATLOGE("%s: type invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        adv.addr = at_ParamUintInRange(pParam->params[3], 0, 1, &paramok);
        if (!paramok)
        {
            ATLOGE("%s: addr invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        adv.channel = at_ParamUintInRange(pParam->params[4], 1, 7, &paramok);
        if (!paramok)
        {
            ATLOGE("%s: channel invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        if (SUCCESS == KING_BtAdvParamSet(&adv))
        {
            g_advcfg = adv;
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            ATLOGE("adv cfg fail: 0x%x", KING_GetLastErrCode());
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        break;

    case AT_CMD_READ:
        sprintf((char *)g_rspStr, "+CBTADVCFG: %d,%d,%d,%d,%d",
            g_advcfg.min_time, g_advcfg.max_time, g_advcfg.type, g_advcfg.addr, g_advcfg.channel);
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CBTADVCFG: (32-16384),(32-16384),(0-4),(0-1),(1-7)");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }
}

void AT_CmdFunc_CBTADVEN(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    uint32 enable;

    if (false == g_btinit)
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if(bt_status == BT_SCANING || bt_status == BT_CLI_CONNECTED || bt_status == BT_SRV_CONNECTED)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            break;
        }
        
        if (pParam->paramCount != 1)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        enable = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (!paramok)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        if (SUCCESS == KING_BtAdvEnable(enable))
        {
            if(enable)
                bt_status = BT_ADVING;
            else
                bt_status = BT_ENABLED;

            g_adven = enable;
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            ATLOGE("enable adv fail: 0x%x", KING_GetLastErrCode());
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        break;

    case AT_CMD_READ:
        sprintf((char *)g_rspStr, "+CBTADVEN: %d", g_adven);
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CBTADVEN: (0,1)");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }
}

void AT_CmdFunc_CBTBCONCFG(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    const uint8_t *p;
    uint8 uuid[16] = {0};
    uint32 maj = 0;
    uint32 min = 0;
    uint8 str_uuid[33] = {0};
    BT_IBEACON_DATA_T data;
    FS_HANDLE fd = NULL;
    unsigned length = 0;
    uint32 bytes = 0;

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if (pParam->paramCount != 3)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }

        p = at_ParamStr(pParam->params[0], &paramok);
        if (!paramok || (strlen((char *)p) != 32))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }

        paramok = atUtils_ConvertHexToBin((char *)p, 32, (char*)uuid);

        if (!paramok)
        {
            ATLOGE("%s: uuid invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }

        maj = at_ParamUintInRange(pParam->params[1], 0, 0xFFFF, &paramok);
        if (!paramok)
        {
            ATLOGE("%s: maj invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }
        
        min = at_ParamUintInRange(pParam->params[2], 0, 0xFFFF, &paramok);
        if (!paramok)
        {
            ATLOGE("%s: min invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }

        memcpy(data.uuid, uuid, 16);
        memcpy(data.major, &maj, 2);
        memcpy(data.minor, &min, 2);
        
        if (FAIL == KING_FsFileCreate(IBEACON_CONFIG_FILE, FS_MODE_WRITE|FS_MODE_CREATE_ALWAYS, 0, 0, &fd))
        {
            ATLOGE("%s: failed to open config file", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            goto exit;
        }
        
        length = sizeof(BT_IBEACON_DATA_T);
        if (KING_FsFileWrite(fd, (uint8*)&data, length, &bytes) != SUCCESS)
        {
            ATLOGE("%s: failed to write config file", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            KING_FsCloseHandle(fd);
            goto exit;
        }

        KING_FsCloseHandle(fd);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_READ:
        if (FAIL == KING_FsFileCreate(IBEACON_CONFIG_FILE, FS_MODE_READ|FS_MODE_OPEN_EXISTING, 0, 0, &fd))
        {
            ATLOGE("%s: failed to open config file", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            goto exit;
        }
        
        length = sizeof(BT_IBEACON_DATA_T);
        if(KING_FsFileRead(fd, (uint8*)&data, length, &bytes) != SUCCESS)
        {
            ATLOGE("%s: failed to read config file", __FUNCTION__);        
            KING_FsCloseHandle(fd);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            goto exit;
        }
        
        //ATLOGE("[atcfg]]%s: %d, %d", __FUNCTION__, sizeof(at_setting_t), bytesRead);
        KING_FsCloseHandle(fd);
        atUtils_ConvertBinToHex(data.uuid, 16, (uint8 *)str_uuid, 0);
        memcpy(&maj, data.major, 2);        
        memcpy(&min, data.minor, 2);        

        sprintf((char *)g_rspStr, "+CBTBCONCFG: \"%s\",%u,%u", str_uuid, maj, min);
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CBTBCONCFG: \"<uuid>\",(0-65535),(0-65535)");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }

exit:
    return;
}

void AT_CmdFunc_CBLESEND(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    const uint8_t *p;
    uint8 data[244] = {0};
    uint32_t length = 0;

    if (false == g_btinit)
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if(bt_status != BT_SRV_CONNECTED && bt_status != BT_CLI_CONNECTED)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            break;
        }

        if (pParam->paramCount != 2)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }

        if (0 == g_mtu)
        {
            ATLOGE("%s: invalid mtu=0", __FUNCTION__);
            AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
        }

        length = at_ParamUintInRange(pParam->params[0], 1, 244, &paramok);
        if ((!paramok) || (length > (g_mtu - PKG_HEADER_LEN)))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }

        p = at_ParamStr(pParam->params[1], &paramok);
        if (!paramok || (strlen((char *)p) == 0) || (strlen((char *)p) > (244 * 2)) \
             || (strlen((char *)p)/2 != length))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }

        paramok = atUtils_ConvertHexToBin((char *)p, strlen((char *)p), (char*)data);
        if (!paramok)
        {
            ATLOGE("%s: data string invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }

        if (FAIL == KING_BtBleSendData(data, (uint16)length))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            if(bt_status == BT_CLI_CONNECTED)
                at_CmdRespOK(pParam->engine);
        }
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CBLESEND: (1-244),\"<data>\"");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }

exit:
    return;
}

void AT_CmdFunc_CBLEDISCONN(AT_CMD_PARA *pParam)
{
    if (false == g_btinit)
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }

    switch (pParam->iType)
    {
    case AT_CMD_EXE:
        if(bt_status != BT_SRV_CONNECTED && bt_status != BT_CLI_CONNECTED)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            break;
        }

        if (FAIL == KING_BtBleDisconnect(NULL))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            g_mtu = 0;
            at_CmdRespOK(pParam->engine);
        }
        break;

    case AT_CMD_TEST:
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }

    return;
}

void AT_CmdFunc_CBLESCAN(AT_CMD_PARA * pParam)
{
    bool paramok = true;
    BT_BLE_SCAN_PARAM_T scan_param;
    if (false == g_btinit)
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if(bt_status == BT_ADVING || bt_status == BT_SRV_CONNECTED || bt_status == BT_CLI_CONNECTED)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            break;
        }

        if (pParam->paramCount == 1)
        { 
            memset(&scan_param,0x00,sizeof(BT_BLE_SCAN_PARAM_T));
            scan_param.enable = at_ParamUintInRange(pParam->params[0], 0, 0, &paramok);
            if (!paramok)
            {
                ATLOGE("%s: enable invalid", __FUNCTION__);
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto exit;
            }
            scan_param.interval = 4;
            scan_param.window = 4;

            if (FAIL == KING_BtBleScan(&scan_param))
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                bt_status = BT_SCANSTOP;
                at_CmdRespOK(pParam->engine);
            }
        }
        else if (pParam->paramCount == 6)
        { 
            memset(&scan_param,0x00,sizeof(BT_BLE_SCAN_PARAM_T));
            scan_param.enable = at_ParamUintInRange(pParam->params[0], 1, 1, &paramok);
            if (!paramok)
            {
                ATLOGE("%s: enable invalid", __FUNCTION__);
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto exit;
            }

            scan_param.scan_type = at_ParamUintInRange(pParam->params[1], 0, 1, &paramok);
            if (!paramok)
            {
                ATLOGE("%s: scan_type invalid", __FUNCTION__);
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto exit;
            }

            scan_param.interval = at_ParamUintInRange(pParam->params[2], 4, 16384, &paramok);
            if (!paramok)
            {
                ATLOGE("%s: interval invalid", __FUNCTION__);
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto exit;
            }

            scan_param.window = at_ParamUintInRange(pParam->params[3], 4, 16384, &paramok);
            if (!paramok)
            {
                ATLOGE("%s: window invalid", __FUNCTION__);
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto exit;
            }

            scan_param.filter_policy = at_ParamUintInRange(pParam->params[4], 0, 3, &paramok);
            if (!paramok)
            {
                ATLOGE("%s: filter policy invalid", __FUNCTION__);
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto exit;
            }

            scan_param.addr_type = at_ParamUintInRange(pParam->params[5], 0, 1, &paramok);
            if (!paramok)
            {
                ATLOGE("%s: addr type invalid", __FUNCTION__);
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
                goto exit;
            }

            if (FAIL == KING_BtBleScan(&scan_param))
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                bt_status = BT_SCANING;
                at_CmdRespOK(pParam->engine);
            }
        }
        else
        {
            ATLOGE("%s: param count invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }
        break;
        
    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CBLESCAN: (0,1),(0,1),(4-16384),(4-16384),(0-3),(0,1)");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
        
    }
exit:
    return;
}

void AT_CmdFunc_CBLECONN(AT_CMD_PARA * pParam)
{
    bool paramok = true;
    const uint8_t *pAddrStr;
    BT_ADDR_T btAddr;
    uint32_t type;
    if (false == g_btinit)
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }
    
    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if(bt_status != BT_SCANSTOP)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            break;
        }

        if (pParam->paramCount != 2)
        {
            ATLOGE("%s: param count invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }
        
        type = at_ParamUintInRange(pParam->params[0], 0, 1, &paramok);
        if (!paramok)
        {
            ATLOGE("%s: type invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }
        
        pAddrStr = at_ParamStr(pParam->params[1], &paramok);
        if (!paramok || (strlen((char *)pAddrStr) != 17))
        {
            ATLOGE("%s: Addr invalid len:%d", __FUNCTION__, strlen((char *)pAddrStr));
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }
        
        if (false == BT_MacStr2Mac(pAddrStr, &btAddr))
        {
            ATLOGE("%s: invalid string format", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }
                
        if (FAIL == KING_BtBleConnect(&btAddr, type))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            at_CmdRespOK(pParam->engine);
        }
        break;
        
    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CBLECONN: <addr_type>,\"<addr>\"");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }
exit:
    return;
}

void AT_CmdFunc_CBLEUUID(AT_CMD_PARA * pParam)
{
    bool paramok = true;
    uint8_t type;
    const uint8_t *pStr;
    uint8_t uuid[16] = {0};
    
    if (false == g_btinit)
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }
    switch (pParam->iType)
    {
    case AT_CMD_SET:
        
        if (pParam->paramCount != 2)
        {
            ATLOGE("%s: param count invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }
        
        type = at_ParamUintInRange(pParam->params[0], 0, 3, &paramok);
        if (!paramok)
        {
            ATLOGE("%s: type invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }
        
        pStr = at_ParamStr(pParam->params[1], &paramok);
        if (!paramok || (strlen((char *)pStr) != 32))
        {
            ATLOGE("%s: uuid invalid len:%d", __FUNCTION__,strlen((char *)pStr));
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }
        
        paramok = atUtils_ConvertHexToBin((char *)pStr, 32, (char*)uuid);
        if (!paramok)
        {
            ATLOGE("%s: data string invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            goto exit;
        }
       
        if (FAIL == King_BtGattUuidsSet(uuid, type))
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        else
        {
            at_CmdRespOK(pParam->engine);
        }
        break;
       
    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CBLEUUID=(0-3),\"<uuid>\"");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }
exit:
    return;
}

void AT_CmdFunc_CBLEMTU(AT_CMD_PARA *pParam)
{
    if (false == g_btinit)
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }

    switch (pParam->iType)
    {
    case AT_CMD_EXE:
    case AT_CMD_READ:
        sprintf((char *)g_rspStr, "+CBLEMTU: %d", g_mtu);
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }
}

void AT_CmdFunc_CBTADVDATA(AT_CMD_PARA *pParam)
{
    bool paramok = true;
    const uint8_t *p;
    uint8 data[31] = {0};
    uint32 len = 0;
    uint32 offset = 0;
    uint8 i = 0;
    
    if (false == g_btinit)
    {
        AT_CMD_RETURN(at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED));
    }

    switch (pParam->iType)
    {
    case AT_CMD_SET:
        if (pParam->paramCount != 2)
        {
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }
        
        len = at_ParamUintInRange(pParam->params[0], 1, 31, &paramok);
        if (!paramok)
        {
            ATLOGE("%s: adv len invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }
        
        p = at_ParamStr(pParam->params[1], &paramok);
        if (!paramok || (strlen((char *)p) > 62))
        {
            ATLOGE("%s: adv data invalid", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }

        paramok = atUtils_ConvertHexToBin((char *)p, (strlen((char *)p)), (char*)data);
        if (!paramok)
        {
            ATLOGE("%s: adv data HexToBin error", __FUNCTION__);
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_PARAM_INVALID);
            break;
        }
     
        if (SUCCESS == King_BtAdvDataSet(data, len))
        {
            adv_len = len;
            memset(adv_data, 0x00, ADV_DATA_LEN);
            memcpy(adv_data, data, adv_len);
            at_CmdRespOK(pParam->engine);
        }
        else
        {
            ATLOGE("%s: set fail: 0x%x", __FUNCTION__, KING_GetLastErrCode());
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
        }
        break;

    case AT_CMD_READ:
        sprintf((char *)g_rspStr, "+CBTADVDATA: %d,\"", adv_len);
        offset = strlen((char *)g_rspStr);
        for (i = 0; i < adv_len; i++)
        {
            offset += sprintf((char *)g_rspStr + offset, "%02X", adv_data[i]);
        }
        strcat((char *)g_rspStr,"\"");
        at_CmdRespInfoText(pParam->engine, g_rspStr);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespInfoText(pParam->engine, (uint8_t *)"+CBTADVDATA: <len>,\"<data>\"");
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }
}


