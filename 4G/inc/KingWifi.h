#ifndef __KING_WIFI_H__
#define __KING_WIFI_H__
#include "kingdef.h"

#define WIFI_EVENT_SCAN_START           0x0001
#define WIFI_EVENT_SCAN_STOP            0x0002
#define WIFI_EVENT_SCAN_RESULT          0x0003

typedef void (*WifiScanCallback)(void *data);
#define KH_MAX_SCAN_NUM 20

typedef struct
{    
    uint32 bssid_low;  ///< mac address low    
    uint16 bssid_high; ///< mac address high    
    uint8 channel;     ///< channel id    
    int rssival;      ///< signal strength
} KH_ApInfo_t;


typedef struct
{    
    uint32 found;      ///< set by wifi, actual found ap count    
    KH_ApInfo_t aps[KH_MAX_SCAN_NUM];   ///< room for aps
} KH_ScanRequest_t;


/**
 * wifi scan初始化。
 * 平台可以决定具体的实现。例如：底层已经初始化过，
 * 可以直接返回成功。或者平台不支持，可以直接返回失败。
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_WifiScanInit(void);

/**
 * wifi scan去初始化。
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_WifiScanDeinit(void);


/**
 * wifi scan启动
 *
 * @param[in]  probe 0：不发送probe报文；1：发送probe报文
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_WifiScanStart(uint32 probe);


/**
 * wifi scan停止
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_WifiScanStop(void);


#endif

