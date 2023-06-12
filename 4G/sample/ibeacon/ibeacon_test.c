/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingBT.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingCSTD.h"

//BT send 最大为200字节
/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("BT: "fmt, ##args); } while(0)
#define IBEACON_UUID                "FDA50693A4E24FB1AFCFC6EB07647825"
#define PRIMARY_SERVICE_UUID        "0000891000001000800000805F9B34FB"
#define RW_CHARACTERISTIC_UUID      "0000FEE100001000800000805F9B34FB"
#define NOTIFY_CHARACTERISTIC_UUID  "0000FEE200001000800000805F9B34FB"
#define ADV_DATA                    "0201061AFF4C000215FDA50693A4E24FB1AFCFC6EB0764782504D2162EC5"

static MSG_HANDLE BtHandle = NULL;

enum Msg_Evt
{
    MSG_EVT_BASE = 0,
    MSG_EVT_CONNECT_SUCCESS,
    MSG_EVT_SEND_SUCCESS,
    MSG_EVT_END = 0x7FFFFFFF,
};

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
 
static bool CZ_ConvertHexToBin
(    // FALSE: There is invalid char
    char   *hex_ptr,     // in: the hexadecimal format string
    uint16 length,       // in: the length of hexadecimal string
    char   *bin_ptr      // out: pointer to the binary format string
)
{
    char  *dest_ptr = bin_ptr;
    int32 i = 0;
    uint8 ch;

    if (NULL == hex_ptr || bin_ptr == NULL || (length % 2) != 0)
    {
        return FALSE;
    }

    for(i = 0; i < length; i += 2)
    {
        // the bit 8,7,6,5
        ch = hex_ptr[i];

        // digital 0 - 9
        if(ch >= '0' && ch <= '9')
        {
            *dest_ptr = (uint8)((ch - '0') << 4);
        }
        // a - f
        else if(ch >= 'a' && ch <= 'f')
        {
            *dest_ptr = (uint8)((ch - 'a' + 10) << 4);
        }
        // A - F
        else if(ch >= 'A' && ch <= 'F')
        {
            *dest_ptr = (uint8)((ch - 'A' + 10) << 4);
        }
        else
        {
            return FALSE;
        }

        // the bit 1,2,3,4
        ch = hex_ptr[i+1];

        // digtial 0 - 9
        if(ch >= '0' && ch <= '9')
        {
            *dest_ptr |= (uint8)(ch - '0');
        }
        // a - f
        else if(ch >= 'a' && ch <= 'f')
        {
            *dest_ptr |= (uint8)(ch - 'a' + 10);
        }
        // A - F
        else if(ch >= 'A' && ch <= 'F')
        {
            *dest_ptr |= (uint8)(ch - 'A' + 10);
        }
        else
        {
            return FALSE;
        }

        dest_ptr++;
    }

    return TRUE;
}

void CZ_ConvertBinToHex
(
    uint8 *bin_ptr,     // in: the binary format string
    uint16 length,       // in: the length of binary string
    uint8 *hex_ptr,      // out: pointer to the hexadecimal format string
    uint8 charcase      // in: char case  0 -- big case; 1 -- little case.
)
{
    uint8 semi_octet;
    int32 i;

    if (NULL == hex_ptr || bin_ptr == NULL || (charcase != 0 && charcase != 1))
    {
        return;
    }

    for(i = 0; i < length; i++)
    {
        // get the high 4 bits
        semi_octet = (uint8)((bin_ptr[i] & 0xF0) >> 4);

        if(semi_octet <= 9)  //semi_octet >= 0
        {
            *hex_ptr = (uint8)(semi_octet + '0');
        }
        else
        {
            if (0 == charcase)
            {
                *hex_ptr = (uint8)(semi_octet + 'A' - 10);
            }
            else
            {
                *hex_ptr = (uint8)(semi_octet + 'a' - 10);
            }
        }

        hex_ptr++;

        // get the low 4 bits
        semi_octet = (uint8)(bin_ptr[i] & 0x0f);

        if(semi_octet <= 9)  // semi_octet >= 0
        {
            *hex_ptr = (uint8)(semi_octet + '0');
        }
        else
        {
            if (0 == charcase)
            {
                *hex_ptr = (uint8)(semi_octet + 'A' - 10);
            }
            else
            {
                *hex_ptr = (uint8)(semi_octet + 'a' - 10);
            }
        }

        hex_ptr++;
    }
}


static void BTCallback(uint32 eventID, void* pData, uint32 len)
{
    MSG_S msg;
    LogPrintf("BTCallback: id = 0x%x, data = 0x%p, len = %d", eventID, pData, len);
    switch(eventID)
    {
        case BT_BLE_CLI_SCAN_RSP_IND:
            {
                LogPrintf("BT_BLE_CLI_SCAN_RSP_IND");
                BT_SCAN_INFO *scan_info = (BT_SCAN_INFO *)pData;
                char adv_data[100] = {0};
                uint8 len = 0;

                if (scan_info->name_length > 0)
                {
                    LogPrintf("SCAN: %s,%d,\"%02X:%02X:%02X:%02X:%02X:%02X\",%d", scan_info->data, scan_info->addr_type, 
                                scan_info->bdAddress.addr[0],scan_info->bdAddress.addr[1],scan_info->bdAddress.addr[2],scan_info->bdAddress.addr[3],
                                scan_info->bdAddress.addr[4],scan_info->bdAddress.addr[5],scan_info->rssi);
                }
                else
                {
                    len = snprintf(adv_data, 100, "SCAN ADV: ");
                    CZ_ConvertBinToHex(scan_info->data, scan_info->data_length, (uint8 *)(adv_data + len), 0);
                    LogPrintf("%s,%d,\"%02X:%02X:%02X:%02X:%02X:%02X\",%d", adv_data, scan_info->addr_type, 
                                scan_info->bdAddress.addr[0],scan_info->bdAddress.addr[1],scan_info->bdAddress.addr[2],scan_info->bdAddress.addr[3],
                                scan_info->bdAddress.addr[4],scan_info->bdAddress.addr[5],scan_info->rssi);
                }
            }
            break;
        case BT_BLE_CLI_CONNECT_IND:
            LogPrintf("BT_BLE_CLI_CONNECT_IND");
            if(BtHandle != NULL)
            {
                msg.messageID = MSG_EVT_CONNECT_SUCCESS;
                KING_MsgSend(&msg, BtHandle);
            }
            break;
        case BT_BLE_CLI_DISCONNECT_IND:
            LogPrintf("BT_BLE_CLI_DISCONNECT_IND");
            break;
        case BT_BLE_SRV_CONNECT_IND:
            LogPrintf("BT_BLE_SRV_CONNECT_IND");
            if(BtHandle != NULL)
            {
                msg.messageID = MSG_EVT_CONNECT_SUCCESS;
                KING_MsgSend(&msg, BtHandle);
            }
            break;

        case BT_BLE_SRV_DISCONNECT_IND:
            LogPrintf("BT_BLE_SRV_DISCONNECT_IND");
            break;

        case BT_BLE_SEND_DATA_COMPLETE_IND:
            LogPrintf("BT_BLE_SEND_DATA_COMPLETE_IND");
            if(BtHandle != NULL)
            {
                msg.messageID = MSG_EVT_SEND_SUCCESS;
                KING_MsgSend(&msg, BtHandle);
            }
            break;

        case BT_BLE_RECV_DATA_IND:
            {
                LogPrintf("BT_BLE_RECV_DATA_IND");
                int i;
                char rsp[400];
                char *p = (char *)pData;
                memset(rsp,0x00,sizeof(rsp));
                for(i = 0; i < len; i++)
                {
                    sprintf(rsp + i*2,"%02x",*p);
                    p++;
                }
                LogPrintf("len:%d data:%s",len,rsp);
            }
            break;

        case BT_BLE_MTU_IND:
            LogPrintf("mtu:%d", *(uint32*)pData);
            break;

        default:
            break;
    }
}

int Bt_Client_Test(void)
{
    int ret;
    BT_BLE_SCAN_PARAM_T ble_scan_param;
    BT_ADDR_T bt_addr = {
        .addr = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc},
    };
    
    //enable BT  
    ret = KING_BtEnable(1);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtEnable() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }

    //BT Scan on
    ble_scan_param.enable = 1;
    ble_scan_param.scan_type = 1;
    ble_scan_param.interval = 500;
    ble_scan_param.window = 200;
    ble_scan_param.filter_policy = 0;
    ble_scan_param.addr_type = BT_ADDR_PUBLIC;
    ret = KING_BtBleScan(&ble_scan_param);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtBleScan() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }
    KING_Sleep(2000);

    //BT Scan off
    ble_scan_param.enable = 0;
    ret = KING_BtBleScan(&ble_scan_param);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtBleScan() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }

    //BT Connect
    ret = KING_BtBleConnect(&bt_addr, BT_ADDR_PUBLIC);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtBleConnect() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }
    return 0;
exit:
    LogPrintf("%s: fail!", __FUNCTION__);
    return -1;

}

int Bt_Server_Test(void)
{
    int ret = -1;
    BT_ADDR_T bt_addr = {
        .addr = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc},
    };
    BT_ADDR_T temp_addr;
    char name[BT_DEVICE_NAME_SIZE + 1];
    BT_ADV_PARAM_T param;
    uint32 len;
    uint8 uuid[16] = {0};
    
    //change BT public addr if needed
    //you don't need to write bluetooth device addresss in most of the time.
    LogPrintf("%s: set BT addr", __FUNCTION__);
    ret = KING_BtAddrGet(BT_ADDR_PUBLIC , &temp_addr);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtAddrGet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }
    LogPrintf("%s: current addr: %02X:%02X:%02X:%02X:%02X:%02X", __FUNCTION__,
            temp_addr.addr[5], temp_addr.addr[4], temp_addr.addr[3],
            temp_addr.addr[2], temp_addr.addr[1], temp_addr.addr[0]);

    ret = KING_BtAddrSet(BT_ADDR_PUBLIC , &bt_addr);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtAddrSet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }

    //enable BT    
    LogPrintf("%s: enable BT", __FUNCTION__);
    ret = KING_BtStateGet(&len);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtStateGet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }
    LogPrintf("%s: Current BT State: %d", __FUNCTION__, len);

    ret = KING_BtEnable(1);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtEnable() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }

    //set BT name 
    LogPrintf("%s: set BT name", __FUNCTION__);
    ret = KING_BtNameGet(name, &len);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtNameGet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }
    LogPrintf("%s: Get BT name: %s", __FUNCTION__, name);

    strcpy(name, "iBeacon_Test");
    ret = KING_BtNameSet(name, strlen(name));
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtNameSet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }

    //set BT advertisement parameter
    LogPrintf("%s: set adv param", __FUNCTION__);
    param.min_time = 96; // 60ms
    param.max_time = 128; // 80ms
    param.type = BT_ADV_IND;
    param.addr = BT_ADDR_PUBLIC;
    param.channel = 7;
    ret = KING_BtAdvParamSet(&param);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtAdvParamSet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }

#if 1
    //set iBeacon data
    LogPrintf("%s: set iBeacon data", __FUNCTION__);
    BT_IBEACON_DATA_T beacon;
    CZ_ConvertHexToBin(IBEACON_UUID, strlen(IBEACON_UUID), (char *)beacon.uuid);
    beacon.major[0] = 0xD2;
    beacon.major[1] = 0x04;
    beacon.minor[0] = 0x2E;
    beacon.minor[1] = 0x16;
    ret = KING_BtIbeaconDataSet(&beacon);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtIbeaconDataSet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }
#else
    LogPrintf("%s: set adv data", __FUNCTION__);
    uint8 advdata[31] = {0};
    CZ_ConvertHexToBin(ADV_DATA, strlen(ADV_DATA), (char *)advdata);
    ret = King_BtAdvDataSet(advdata, 30);
    if(SUCCESS != ret)
    {
        LogPrintf("King_BtAdvDataSet Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }
#endif
    KING_Sleep(1000);

    //set UUID
    LogPrintf("%s: set UUID", __FUNCTION__);
    memset(uuid, 0x00, sizeof(uuid));
    CZ_ConvertHexToBin(PRIMARY_SERVICE_UUID, strlen(PRIMARY_SERVICE_UUID), (char *)uuid);
    ret = King_BtGattUuidsSet(uuid, BT_UUID_PRIMARY_SERVICE);
    if(SUCCESS != ret)
    {
        LogPrintf("King_BtGattUuidsSet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }
    memset(uuid, 0x00, sizeof(uuid));
    CZ_ConvertHexToBin(RW_CHARACTERISTIC_UUID, strlen(PRIMARY_SERVICE_UUID), (char *)uuid);
    ret = King_BtGattUuidsSet(uuid, BT_UUID_RW_CHARACTERISTIC);
    if(SUCCESS != ret)
    {
        LogPrintf("King_BtGattUuidsSet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }
    memset(uuid, 0x00, sizeof(uuid));
    CZ_ConvertHexToBin(NOTIFY_CHARACTERISTIC_UUID, strlen(PRIMARY_SERVICE_UUID), (char *)uuid);
    ret = King_BtGattUuidsSet(uuid, BT_UUID_NOTIFY_CHARACTERISTIC);
    if(SUCCESS != ret)
    {
        LogPrintf("King_BtGattUuidsSet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }
    
    //enable advertising
    LogPrintf("%s: enable advertising", __FUNCTION__);
    ret = KING_BtAdvEnable(1);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtAdvEnable() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }
    return 0;
exit:
    LogPrintf("%s: fail!", __FUNCTION__);
    return -1;
}

void ibeacon_test(void)
{
    int ret = -1;
    uint8 server_mode = 1;
    MSG_S msg;
    ret = KING_MsgCreate(&BtHandle);
    if(FAIL == ret)
    {
        LogPrintf("KING_MsgCreate() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    }
    
    //init BT
    LogPrintf("%s: init BT", __FUNCTION__);
    KING_RegNotifyFun(DEV_CLASS_BT, 0, BTCallback);
    ret = KING_BtInit();
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtInit() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        goto exit;
    } 
    
    if(server_mode)
    {
        Bt_Server_Test();
    }
    else
    {
        Bt_Client_Test();
    }
    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg, BtHandle, WAIT_OPT_INFINITE);
        switch (msg.messageID)
        {
            case MSG_EVT_CONNECT_SUCCESS:
            {
                KING_Sleep(4000);
                if (FAIL == KING_BtBleSendData((uint8 *)"abcd", strlen("abcd")))
                {
                    LogPrintf("KING_BtBleSendData() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
                    goto exit;
                }
                break;
            }
            case MSG_EVT_SEND_SUCCESS:
            {
                KING_Sleep(20000);
                if(!server_mode)
                {
                    //全部断开连接
                    ret = KING_BtBleDisconnect(NULL);
                    if(SUCCESS != ret)
                    {
                        LogPrintf("KING_BtBleDisconnect() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
                        goto exit;
                    }
                    KING_Sleep(1000);
                }
                
                ret = KING_BtEnable(0);
                if(SUCCESS != ret)
                {
                    LogPrintf("KING_BtEnable() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
                    goto exit;
                }
                ret = KING_BtDeinit();
                if(SUCCESS != ret)
                {
                    LogPrintf("KING_BtDeinit() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
                    goto exit;
                }
                break;
            }
            default:
            {
                LogPrintf("msg no know\r\n");
                break;
            }
        }
    }
exit:
    LogPrintf("%s: fail!", __FUNCTION__);
    return;

}

