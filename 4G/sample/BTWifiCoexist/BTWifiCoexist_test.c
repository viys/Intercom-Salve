/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingBT.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingCSTD.h"
#include "kingwifi.h"
#include "kingCbData.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("BTWifiCoexist: "fmt, ##args); } while(0)
#define IBEACON_UUID "FDA50693A4E24FB1AFCFC6EB07647825"

static THREAD_HANDLE threadHandle = NULL;
static SEM_HANDLE wifiSem = NULL;

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/

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

static void BTCallback(uint32 eventID, void* pData, uint32 len)
{
    LogPrintf("BTCallback: id = 0x%x, data = 0x%p, len = %d", eventID, pData, len);
    switch(eventID)
    {
        case BT_BLE_CLI_DISCONNECT_IND:
            LogPrintf("BT_BLE_CLI_DISCONNECT_IND");
            break;
        case BT_BLE_SRV_CONNECT_IND:
            LogPrintf("BT_BLE_SRV_CONNECT_IND");
            break;

        case BT_BLE_SRV_DISCONNECT_IND:
            LogPrintf("BT_BLE_SRV_DISCONNECT_IND");
            break;

        case BT_BLE_SEND_DATA_COMPLETE_IND:
            LogPrintf("BT_BLE_SEND_DATA_COMPLETE_IND");
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

static void wifiScan_EventCb(uint32 eventID, void* pdata, uint32 len)
{
    KH_CB_DATA_U* data = (KH_CB_DATA_U*)pdata;

    LogPrintf("wifiScan_EventCb get event %d len %d", eventID, len);
    if (eventID == WIFI_EVENT_SCAN_RESULT)
    {
        int i;

        LogPrintf("wifiScan_EventCb found= %d", data->wifi.result.found);
        for (i = 0; i < data->wifi.result.found; i++)
        {
            LogPrintf("wifiScan_EventCb find i= %d",i);
            LogPrintf("wifiScan_EventCb found ap - {mac address : %04X%08X, rssival: %i dBm, channel: %u}",
                        data->wifi.result.aps[i].bssid_high, data->wifi.result.aps[i].bssid_low, 
                        data->wifi.result.aps[i].rssival, data->wifi.result.aps[i].channel);
        }
    }
    else if (eventID == WIFI_EVENT_SCAN_STOP)
    {
        LogPrintf("WIFI_EVENT_SCAN_STOP");
        KING_SemPut(wifiSem);
    }

}

void wifiScan_test(void)
{
    int ret = -1;
    LogPrintf("wifiScan_test start");

    ret = KING_WifiScanInit();
    if (SUCCESS != ret)
    {
        LogPrintf("KING_WifiScanInit() Failed! errcode=0x%x", KING_GetLastErrCode());
    }

    ret = KING_WifiScanStart(1);
    if (SUCCESS != ret)
    {
        LogPrintf("KING_WifiScanStart() Failed! errcode=0x%x", KING_GetLastErrCode());
        goto exit;
    }
    ret = KING_SemGet(wifiSem, 10000);
    if (SUCCESS != ret)
    {
        LogPrintf("KING_MutexLock() Failed! errcode=0x%x", KING_GetLastErrCode());
        goto exit;
    }

exit:
    KING_WifiScanStop();
    KING_WifiScanDeinit(); 
    LogPrintf("wifiScan_test stop");
}

void Bt_Test(void)
{
    int ret = -1;
    BT_ADDR_T bt_addr = {
        .addr = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc},
    };
    char name[BT_DEVICE_NAME_SIZE + 1];
    BT_ADV_PARAM_T param;

    LogPrintf("Bt_Test start");

    ret = KING_BtInit();
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtInit() Failed! errcode=0x%x", KING_GetLastErrCode());
        goto exit;
    } 

    ret = KING_BtAddrSet(BT_ADDR_PUBLIC , &bt_addr);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtAddrSet() Failed! errcode=0x%x", KING_GetLastErrCode());
        goto exit;
    }

    LogPrintf("%s: enable BT", __FUNCTION__);
    ret = KING_BtEnable(1);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtEnable() Failed! errcode=0x%x", KING_GetLastErrCode());
        goto exit;
    }

    strcpy(name, "iBeacon_Test");
    ret = KING_BtNameSet(name, strlen(name));
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtNameSet() Failed! errcode=0x%x", KING_GetLastErrCode());
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
        LogPrintf("KING_BtAdvParamSet() Failed! errcode=0x%x", KING_GetLastErrCode());
        goto exit;
    }

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
        LogPrintf("KING_BtIbeaconDataSet() Failed! errcode=0x%x", KING_GetLastErrCode());
        goto exit;
    }
    KING_Sleep(1000);

    //enable advertising
    LogPrintf("%s: enable advertising", __FUNCTION__);
    ret = KING_BtAdvEnable(1);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtAdvEnable() Failed! errcode=0x%x", KING_GetLastErrCode());
        goto exit;
    }
    KING_Sleep(15000);

exit:
    //全部断开连接
    ret = KING_BtBleDisconnect(NULL);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtBleDisconnect() Failed! errcode=0x%x", KING_GetLastErrCode());
    }
    KING_Sleep(500);

    ret = KING_BtAdvEnable(0);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtAdvEnable() Failed! errcode=0x%x", KING_GetLastErrCode());
    }
    KING_Sleep(500);

    ret = KING_BtEnable(0);
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtEnable() Failed! errcode=0x%x", KING_GetLastErrCode());
    }
    KING_Sleep(500);

    ret = KING_BtDeinit();
    if(SUCCESS != ret)
    {
        LogPrintf("KING_BtDeinit() Failed! errcode=0x%x", KING_GetLastErrCode());
    }
    LogPrintf("Bt_Test stop");
}

static void BTWifiCoexist_ThreadFunc(void *param)
{
    int ret = -1;
    ret = KING_SemCreate("wifi_Sem", 0, &wifiSem);
    if (SUCCESS != ret)
    {
        LogPrintf("KING_SemCreate() Failed! errcode=0x%x", KING_GetLastErrCode());
        return;
    }
    KING_RegNotifyFun(DEV_CLASS_WIFI, 0, wifiScan_EventCb);
    KING_RegNotifyFun(DEV_CLASS_BT, 0, BTCallback);

    while(1)
    { 
        Bt_Test();
        KING_Sleep(3000);
        wifiScan_test();
        KING_Sleep(1000);
    }
}

void BTWifiCoexist_Test()
{
    int ret;
    THREAD_ATTR_S threadAttr;

    //Thread create 
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = BTWifiCoexist_ThreadFunc;
    threadAttr.priority = THREAD_PRIORIT4;
    threadAttr.stackSize = 4096;
    ret = KING_ThreadCreate("BTWifiCoexist Test", &threadAttr, &threadHandle);
    if (SUCCESS != ret)
    {
        LogPrintf("KING_ThreadCreate() Failed! errcode=0x%x", KING_GetLastErrCode());
    }
}
