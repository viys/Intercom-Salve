#ifndef __KING_PLAT_H__
#define __KING_PLAT_H__
#include "kingdef.h"

#define SIM_ICCID_NUM_LEN      10
#define DEBUG_LOG_MAX_LEN      512

#define SYS_CFG_GET            0
#define SYS_CFG_SET            1

#define LOCK_BAND_LIST_MAX_NUM      19

/**
 * @brief API LEVEL 说明
 *
 * @attention API的使用对API LEVEL有要求，没有特别说明的API，则该API对于所有的
 * API LEVEL都支持。
 *
 */
#define API_LEVEL_1        1
#define API_LEVEL_2        2 ///< start from 2021.1.29

// HW device class, including virtual AT uart,
// NetWork, and Socket

typedef enum{
    DEV_CLASS_GPIO,
    DEV_CLASS_UART,
    DEV_CLASS_I2C,
    DEV_CLASS_I2S, 
    DEV_CLASS_SPI, 
    DEV_CLASS_PWM, 
    DEV_CLASS_KEY, 
    DEV_CLASS_AUDIO,
    //DEV_CLASS_TTS, //luxiao removed because duplicated with audio
    DEV_CLASS_AT,
    DEV_CLASS_NW,
    DEV_CLASS_SOCKET,
    DEV_CLASS_ALARM,
    DEV_CLASS_SIM_STATE,
    DEV_CLASS_SIM_EVENT,
    DEV_CLASS_CALL_CONTROL,
    DEV_CLASS_SMS,
    DEV_CLASS_WIFI,
    DEV_CLASS_CHARGE,
    DEV_CLASS_USB,
    DEV_CLASS_BT,
    DEV_CLASS_DUALSIM,
    DEV_CLASS_LAST,
    DEV_CLASS_MAX = 0x7FFFFFFF
}DEV_CLASS;
    
typedef enum _psmWakeUpMode {
    /** Input, sleep with pull up, and wakeup at falling edge */
    PSM_WAKE_FALL_EDGE,
    /** Input, sleep with pull down, and wakeup at rising edge */
    PSM_WAKE_RISE_EDGE,
    /** Input, sleep with pull none, and wakeup at falling or rising edge */
    PSM_WAKE_ANY_EDGE,
    PSM_WAKE_MAX = 0x7FFFFFFF
} PSM_WAKE_UP_MODE;

typedef enum {
    SYS_CFG_ID_SIM_HOT_PLUG, ///< refer to SIM_HOT_PLUG_CFG_T
    SYS_CFG_ID_AUDIO_CALI_PARAM, ///< refer to AUDIO_CALI_PARAM_CFG_T
    SYS_CFG_ID_ASSERT_REBOOT, ///< 死机自动重启配置：0 - 不重启，用于死机调试；1 - 重启
    SYS_CFG_ID_PSM_WAKEIP, ///< PSM唤醒脚配置，8811平台支持GPIO2-7用作PSM唤醒，只支持配置，不支持读取
    SYS_CFG_ID_LOCK_BAND, ///< 锁频，8811平台支持
    SYS_CFG_ID_CC_RING_MODE,  ///< 来电响铃模式 0 - 自动播放默认铃声; 1 - APP自行控制来电铃声(目前仅8910平台支持)
    SYS_CFG_ID_PWR_KEY_7S_RESET,  ///< 配置是否启用长按POWER KEY 7秒复位功能：0 - 禁用；1 - 启用
    SYS_CFG_ID_MAX = 0x7FFFFFFF
}SYS_CFG_ID_E;

typedef struct {
    uint8 simID;
    uint8 enable; ///< SIM卡热插拔配置功能开关。 0：关闭；1：开启
    uint8 level; ///< SIM卡热插拔在位电平。 0：低电平在位；1：高电平在位
}SIM_HOT_PLUG_CFG_T;

typedef struct {
    uint8 channel; ///< useless for UIS8910DM
    uint8 cfg_file[64]; ///< audio config file path and name
}AUDIO_CALI_PARAM_CFG_T;

typedef struct {
    uint8 gpio;
    PSM_WAKE_UP_MODE mode;
}PSM_WAKEUP_PIN_CFG_T;

typedef struct {
    uint8 bandNum; //1-19
    uint8 bandList[LOCK_BAND_LIST_MAX_NUM];
}LOCK_BAND_CFG_T;

/**
 * 平台消息回调函数，对于不同类型的事件，pData有各自
 * 不同的数据结构。具体参考KingCbData.h。此函数内不可休眠，不可做太耗时的操作。
 * @param[in]  eventID   事件类型，可以是硬件事件类型，或者是网络事件类型
 * @param[in]  pData     与事件相应的数据内容，如无内容则为NULL。
 * @param[in]  len       Length of pData
 */
typedef void (*NOTIFY_FUN)(uint32 eventID, void* pData, uint32 len);

/**
 * @brief 平台通用消息通知回调函数。
 *
 * @note SOCKET事件devID说明：APP注册回调时，devID设置为socket ID.
 *
 * @param[in]  devClass  设备类型，如GPIO, UART, AT等
 * @param[in]  devID     设备ID，如果UART ID (虚拟AT通道设置为0), socket ID.
 * @param[in]  fun 回调函数
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_RegNotifyFun(DEV_CLASS devClass, uint32 devID, NOTIFY_FUN fun);

/**
 * 平台通用消息通知去注册
 *
 * @note SOCKET事件devID说明：APP注册回调时，devID设置为socket ID.
 *
 * @param[in]  devClass  设备类型，如GPIO, UART, AT等
 * @param[in]  devID     设备ID，如果UART ID (虚拟AT通道设置为0), socket ID.
 * @param[in]  fun 去激活的回调函数。可以为NULL
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_UnRegNotifyFun(DEV_CLASS devClass, uint32 devID, NOTIFY_FUN fun);

/**
 * Get last error code.
 *
 * @return last error code.
 */
int KING_GetLastErrCode(void);

/**
 * Set last error code.
 *
 * @return 0 SUCCESS  -1 FAIL.
 */
int KING_SetLastErrCode(int code);

/**
 * 获取SDK版本
 *
 * @param[out]  pLevel          sdk api版本
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ApiLevel(uint32* pLevel);

/**
 * 获取平台版本信息
 *
 * @return 平台版本信息字符串
 */
char* KING_PlatformInfo(void);

/**
 * 获取sim卡ready状态
 *
 * @param[out]  pReady       0 - not ready  1- ready
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimReadyGet(uint32* pReady);

/**
 * 返回平台的IMEI号
 *
 * @param[out] pImei  IMEI字符串指针
 * @return 0 success  -1 fail
 */
int KING_GetSysImei(uint8 *pImei);

/**
 * Output log to low level debug channel.
 */
void KING_SysLog(const char *fmt, ...);

/**
 * 获取设备对应的MEID号。
 * 仅适用于支持CDMA制式的平台。
 *
 * @param[in] simID  Sim ID. 0 or 1
 * @param[inout] meid 存放MEID的buffer
 *
 * @return  0 SUCCESS   -1 FAIL
 */
int KING_SysMEIDGet(uint8 simID, uint8 *meid);

/**
 * 恢复出厂设置
 * 用于恢复底包软件为出厂设置，APP保存的配置信息、文件等由APP自己负责恢复。
 *
 * @return  0 SUCCESS   -1 FAIL
 */
int KING_RestoreFactorySettings(void);

/**
 * 获取BootApp软件版本信息
 *
 * 版本信息定义在version/bootapp_version.h中
 *
 * @return 如果没有BootApp返回NULL，否则返回版本信息字符串([major].[minor].[buildnumber])
 */
char* KING_BootAppVer(void);

/**
 * 获取ChipId
 *
 * @param[inout] uuid 存放获取ChipId的buffer
 * @return  0 SUCCESS   -1 FAIL
 * @attention require API Level 2
 */
int KING_GetChipId(uint8 uuid[16]);

/**
 * 读取/设置系统配置项
 *
 * @param[in] ops 0: 读取；1：设置
 * @param[in] cfg_id 配置项ID
 * @param[inout] buf 读取/设置配置项内容
 * @param[inout] size 读取/设置配置项的大小
 * @return  0 SUCCESS   -1 FAIL
 * @attention require API Level 2
 */
int KING_SysCfg(uint8 ops, SYS_CFG_ID_E cfg_id, void *buf, uint32 *size);

/**
 * 返回平台的IMEI号
 *
 * @param[in]  simID  which IMEI
 * @param[out] pImei  IMEI字符串指针
 * @return 0 success  -1 fail
 */
int KING_GetSysImeiV2(uint8 simID, uint8 *pImei);

#endif

