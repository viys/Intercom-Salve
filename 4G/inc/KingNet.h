#ifndef __KING_NET_H__
#define __KING_NET_H__

#include "KingDef.h"

/**
 * Network Events
 */
#define NW_EVENT_SIM_READY              0x0001  //sim car ready
#define NW_EVENT_SIM_NOT_READY          0x0002  //sim car not ready

#define NW_EVENT_PS_ONOFF_RSP           0x0003  // response for PS on and off
#define NW_EVENT_ATTACH_RSP             0x0004  // response for GPRS attach and deattach
#define NW_EVENT_PDP_ACT_RSP            0x0005  // response for PDP active and deactive

#define NW_EVENT_GSM_REGISTERED_IND     0x0006  // GSM ok
#define NW_EVENT_GSM_UNREGISTERED_IND   0x0007  // unregister from GSM
#define NW_EVENT_GPRS_ATTACH_IND        0x0008  // attach to GPRS
#define NW_EVENT_GPRS_DEATTACH_IND      0x0009  // deattach from GPRS
#define NW_EVENT_PDP_DEACT_IND          0x0010  // pdp is deactived by network

#define NW_EVENT_CELL_INFO_RSP          0x0011

#define NW_EVENT_SMS_READY              0x0012  // sms ready
#define NW_EVENT_SMS_SEND_SUCCESS       0x0013  // sms send success
#define NW_EVENT_SMS_SEND_FAIL          0x0014  // sms send fail
#define NW_EVENT_SMS_READ_CNF           0x0015  // sms read result
#define NW_EVENT_SMS_IND                0x0016  // new sms

#define NW_EVENT_CALL_INCOMING_IND      0x0017  // call incoming
#define NW_EVENT_CALL_DISCONNECTED_IND  0x0018

#define NW_EVENT_LBS_GET_POS_RSP        0x0019
#define NW_EVENT_NETTIME_SYNC_IND       0x001A  // Net time has been synced


#define NW_EVENT_FOTA_APP_CHECK_RSP     0x0101  //fota app update check
#define NW_EVENT_FOTA_DOWNLOAD_RSP      0x0102
#define NW_EVENT_FOTA_UPDATE_RSP        0x0103

#define NW_EVENT_ENTER_PSM_IND    0x0200  // stack enter psm ind
#define NW_EVENT_EXIT_PSM_IND    0x0201   // stack exit psm ind

/**
 * Net modes defined for KING_NetModeSet()
 */
typedef enum {
    NET_MODE_GSM  = 0,        /**< GSM */
    NET_MODE_UMTS ,         /**< UMTS */  
    NET_MODE_LTE,            /**< LTE */    
    NET_MODE_GSM_UMTS,        /**< GSM_UMTS */    
    NET_MODE_GSM_LTE,        /**< GSM_LTE */
    NET_MODE_UMTS_LTE,        /**< UMTS_LTE */    
    NET_MODE_GSM_UMTS_LTE,    /**< GSM_UMTS_LTE */
    NET_MODE_NB,
    NET_MODE_MAX = 0x7FFFFFFF
} KH_NET_MODE;


typedef struct
{
    uint32 rxlev;
    uint32 ber;
    uint32 rscp;
    uint32 ecno;
    uint32 rsrq;
    int32  rsrp;
    int32  snr;
}NET_CESQ_S;

typedef enum
{
    NET_NO_SERVICE = 0,
    NET_NORMAL_GSM,
    NET_NORMAL_EDGE,
    NET_NORMAL_CDMA,
    NET_NORMAL_WCDMA,
    NET_NORMAL_TDSCDMA,
    NET_NORMAL_LTE,
    NET_NORMAL_LTE_CAT_M1,
    NET_NORMAL_LTE_CAT_NB1, // 8
    NET_EMERGENCY,
    NET_STATE_MAX = 0x7FFFFFFF
}NET_STATE_E;


typedef struct
{
    uint32  cell_exist;
    uint16  arfcn;
    uint8   bsic;
    int16   rxlev;
    uint16  mcc;
    uint16  mnc;
    uint16  mnc_digit_num;
    uint16  lac;
    uint32  cell_id;
} NCELLS_INFO_S;

typedef struct
{
    uint32  cell_exist;
    uint16  arfcn;
    uint8   bsic;
    int16   rxlev;
    uint16  mcc;
    uint16  mnc;
    uint16  mnc_digit_num;
    uint16  lac;
    uint32  cell_id;
    uint32  eci;  //E-UTRAN cell ID
} SCELL_INFO_S;

typedef struct
{
    NCELLS_INFO_S ncells[6];
    SCELL_INFO_S      scell;
}CELLS_INFO_S;

typedef enum{
    NET_ACTION_DEACTIVE,
    NET_ACTION_ACTIVE,
    NET_ACTION_DETACHED,    
    NET_ACTION_ATTACHED,
    NET_ACTION_PSONOFF,
    NET_ACTION_MAX = 0x7FFFFFFF
}NET_ACTION;

typedef struct
{
    uint32    value;
    uint32    result;
    NET_ACTION    action;
}RSP_INFO_S;

/**
 * 获取当前网络状态
 *
 * @param[out] pState 给出当前网络状态
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetStateGet(NET_STATE_E* pState);

/**
 * 查询网络信号
 *
 * @param[out] pCesq 给出当前网络信号
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetSignalQualityGet(NET_CESQ_S* pCesq);

/**
 * 设置网络模式，可由多种模式 或 组成
 *
 * @param[in] mode 要设置的模式
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetModeSet(KH_NET_MODE mode);

/**
 * 查询配置的网络模式
 * 注意是配置的网络模式，而不是当前实际网络模式
 * @param[out] pMode 给出配置的网络模式
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetModeGet(KH_NET_MODE* pMode);

/**
 * 激活指定的pdp
 *
 * @param[in] cid    PDP Context ID, 1-4.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetPdpActive(uint32 cid);

/**
 * 去激活指定的pdp
 *
 * @param[in] cid    PDP Context ID, 1-4.
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetPdpDeactive(uint32 cid);

/**
 * 获取指定的pdp的激活状态
 *
 * @param[in] cid       PDP Context ID, 1-4.
 * @param[out] pValue   PDP激活状态 0 - 未激活 1 - 已激活
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetPdpStatusGet(uint32 cid, uint32* pValue);

/**
 * 附着网络
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetAttach(void);

/**
 * 去附着网络
 *
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetDeattach(void);

/**
 * 查询网络附着状态
 *
 * @param[out] pValue  网络附着状态 0 - 未附着 1 - 已附着
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetAttachStatusGet(uint32* pValue);


/**
 * 飞行模式开关
 *
 * @param[in] enable  0-关闭飞行模式  1-开启飞行模式
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetAirplaneModeSet(uint32 enable);

/**
 * 获取当前是否处于飞行模式
 *
 * @param[out] pEnable  0-正常模式  1-飞行模式
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_NetAirplaneModeGet(uint32* pEnable);

#endif
