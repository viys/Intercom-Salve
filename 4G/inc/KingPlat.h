#ifndef __KING_PLAT_H__
#define __KING_PLAT_H__
#include "kingdef.h"

#define SIM_ICCID_NUM_LEN      10
#define DEBUG_LOG_MAX_LEN      512

#define SYS_CFG_GET            0
#define SYS_CFG_SET            1

#define LOCK_BAND_LIST_MAX_NUM      19

/**
 * @brief API LEVEL ˵��
 *
 * @attention API��ʹ�ö�API LEVEL��Ҫ��û���ر�˵����API�����API�������е�
 * API LEVEL��֧�֡�
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
    SYS_CFG_ID_ASSERT_REBOOT, ///< �����Զ��������ã�0 - �������������������ԣ�1 - ����
    SYS_CFG_ID_PSM_WAKEIP, ///< PSM���ѽ����ã�8811ƽ̨֧��GPIO2-7����PSM���ѣ�ֻ֧�����ã���֧�ֶ�ȡ
    SYS_CFG_ID_LOCK_BAND, ///< ��Ƶ��8811ƽ̨֧��
    SYS_CFG_ID_CC_RING_MODE,  ///< ��������ģʽ 0 - �Զ�����Ĭ������; 1 - APP���п�����������(Ŀǰ��8910ƽ̨֧��)
    SYS_CFG_ID_PWR_KEY_7S_RESET,  ///< �����Ƿ����ó���POWER KEY 7�븴λ���ܣ�0 - ���ã�1 - ����
    SYS_CFG_ID_MAX = 0x7FFFFFFF
}SYS_CFG_ID_E;

typedef struct {
    uint8 simID;
    uint8 enable; ///< SIM���Ȳ�����ù��ܿ��ء� 0���رգ�1������
    uint8 level; ///< SIM���Ȳ����λ��ƽ�� 0���͵�ƽ��λ��1���ߵ�ƽ��λ
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
 * ƽ̨��Ϣ�ص����������ڲ�ͬ���͵��¼���pData�и���
 * ��ͬ�����ݽṹ������ο�KingCbData.h���˺����ڲ������ߣ�������̫��ʱ�Ĳ�����
 * @param[in]  eventID   �¼����ͣ�������Ӳ���¼����ͣ������������¼�����
 * @param[in]  pData     ���¼���Ӧ���������ݣ�����������ΪNULL��
 * @param[in]  len       Length of pData
 */
typedef void (*NOTIFY_FUN)(uint32 eventID, void* pData, uint32 len);

/**
 * @brief ƽ̨ͨ����Ϣ֪ͨ�ص�������
 *
 * @note SOCKET�¼�devID˵����APPע��ص�ʱ��devID����Ϊsocket ID.
 *
 * @param[in]  devClass  �豸���ͣ���GPIO, UART, AT��
 * @param[in]  devID     �豸ID�����UART ID (����ATͨ������Ϊ0), socket ID.
 * @param[in]  fun �ص�����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_RegNotifyFun(DEV_CLASS devClass, uint32 devID, NOTIFY_FUN fun);

/**
 * ƽ̨ͨ����Ϣ֪ͨȥע��
 *
 * @note SOCKET�¼�devID˵����APPע��ص�ʱ��devID����Ϊsocket ID.
 *
 * @param[in]  devClass  �豸���ͣ���GPIO, UART, AT��
 * @param[in]  devID     �豸ID�����UART ID (����ATͨ������Ϊ0), socket ID.
 * @param[in]  fun ȥ����Ļص�����������ΪNULL
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
 * ��ȡSDK�汾
 *
 * @param[out]  pLevel          sdk api�汾
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_ApiLevel(uint32* pLevel);

/**
 * ��ȡƽ̨�汾��Ϣ
 *
 * @return ƽ̨�汾��Ϣ�ַ���
 */
char* KING_PlatformInfo(void);

/**
 * ��ȡsim��ready״̬
 *
 * @param[out]  pReady       0 - not ready  1- ready
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SimReadyGet(uint32* pReady);

/**
 * ����ƽ̨��IMEI��
 *
 * @param[out] pImei  IMEI�ַ���ָ��
 * @return 0 success  -1 fail
 */
int KING_GetSysImei(uint8 *pImei);

/**
 * Output log to low level debug channel.
 */
void KING_SysLog(const char *fmt, ...);

/**
 * ��ȡ�豸��Ӧ��MEID�š�
 * ��������֧��CDMA��ʽ��ƽ̨��
 *
 * @param[in] simID  Sim ID. 0 or 1
 * @param[inout] meid ���MEID��buffer
 *
 * @return  0 SUCCESS   -1 FAIL
 */
int KING_SysMEIDGet(uint8 simID, uint8 *meid);

/**
 * �ָ���������
 * ���ڻָ��װ����Ϊ�������ã�APP�����������Ϣ���ļ�����APP�Լ�����ָ���
 *
 * @return  0 SUCCESS   -1 FAIL
 */
int KING_RestoreFactorySettings(void);

/**
 * ��ȡBootApp����汾��Ϣ
 *
 * �汾��Ϣ������version/bootapp_version.h��
 *
 * @return ���û��BootApp����NULL�����򷵻ذ汾��Ϣ�ַ���([major].[minor].[buildnumber])
 */
char* KING_BootAppVer(void);

/**
 * ��ȡChipId
 *
 * @param[inout] uuid ��Ż�ȡChipId��buffer
 * @return  0 SUCCESS   -1 FAIL
 * @attention require API Level 2
 */
int KING_GetChipId(uint8 uuid[16]);

/**
 * ��ȡ/����ϵͳ������
 *
 * @param[in] ops 0: ��ȡ��1������
 * @param[in] cfg_id ������ID
 * @param[inout] buf ��ȡ/��������������
 * @param[inout] size ��ȡ/����������Ĵ�С
 * @return  0 SUCCESS   -1 FAIL
 * @attention require API Level 2
 */
int KING_SysCfg(uint8 ops, SYS_CFG_ID_E cfg_id, void *buf, uint32 *size);

/**
 * ����ƽ̨��IMEI��
 *
 * @param[in]  simID  which IMEI
 * @param[out] pImei  IMEI�ַ���ָ��
 * @return 0 success  -1 fail
 */
int KING_GetSysImeiV2(uint8 simID, uint8 *pImei);

#endif

