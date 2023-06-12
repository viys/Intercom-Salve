/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "kingwifi.h"
#include "kingplat.h"
#include "kingCbData.h"
#include "kingrtos.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define LogPrintf(fmt, args...)    do { KING_SysLog("wifiscan: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
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
        int ret = -1; 
        
        ret = KING_WifiScanStop();
        LogPrintf("KING_WifiScanStop ret= %d", ret);

        ret = KING_WifiScanDeinit();
        LogPrintf("KING_WifiScanDeinit ret= %d", ret);
    }
    
}

void wifiScan_test(void)
{
    int ret = -1;
    
    KING_RegNotifyFun(DEV_CLASS_WIFI, 0, wifiScan_EventCb);

    ret = KING_WifiScanInit();
    LogPrintf("KING_WifiScanInit ret= %d", ret);
    
    ret = KING_WifiScanStart(0);
    LogPrintf("KING_WifiScanStart 0 ret= %d", ret);

    ret = KING_WifiScanStart(1);
    LogPrintf("KING_WifiScanStart 1 ret= %d", ret);
}

