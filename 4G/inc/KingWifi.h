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
 * wifi scan��ʼ����
 * ƽ̨���Ծ��������ʵ�֡����磺�ײ��Ѿ���ʼ������
 * ����ֱ�ӷ��سɹ�������ƽ̨��֧�֣�����ֱ�ӷ���ʧ�ܡ�
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_WifiScanInit(void);

/**
 * wifi scanȥ��ʼ����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_WifiScanDeinit(void);


/**
 * wifi scan����
 *
 * @param[in]  probe 0��������probe���ģ�1������probe����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_WifiScanStart(uint32 probe);


/**
 * wifi scanֹͣ
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_WifiScanStop(void);


#endif

