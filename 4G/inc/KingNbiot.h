#ifndef __KING_NBIOT_H__
#define __KING_NBIOT_H__


#include "KingDef.h"

/**
 * POWER MODE
 * @note 如果系统进入PSM休眠模式，唤醒开机后网络状态有可能不更新。
 * 包括PDP激活状态，RSRP信号值等都可能不更新。APP可以不判断PDP激活状态，
 * 直接进行socket创建和连接的操作。
 */
#define POWER_MODE_0    0   ///< 8811 platform, enable DRX, disable eDRX, PSM. Module in PM0
#define POWER_MODE_1    1   ///< 8811 platform, enable DRX, eDRX, disable PSM. Module in PM0
#define POWER_MODE_2    2   ///< 8811 platform, enable DRX, PSM, disable eDRX. Module in PM0
#define POWER_MODE_3    3   ///< 8811 platform, enable DRX, eDRX, PSM. Module in PM0
#define POWER_MODE_4    4   ///< 8811 platform, enable DRX, eDRX, PSM. Module in PM2
#define POWER_MODE_5    5   ///< 8811 platform, enable DRX, eDRX, PSM, Module in PM3.

typedef struct edrx_cfg_t
{
    uint8 edrxEnable;   ///< eDRX enable flag. 1: enable. 0: disable.
    uint8 edrxValue;    ///< eDRX value. Integer value range is 0 ~ 15.
    uint8 edrxPtw;      ///< eDRX PTW. Integer value range is 0 ~ 15.
} NBIOT_EDRX_CFG;

/**
 * PSM settings. For now only support below two settings.
 * 1. Requested_Periodic-TAU, 8bit String of Byte. e.g. "01000111"
 *  where "010" is the unit, and "00111" is the value.
 *  For the coding and the value range, see the GPRS Timer 3 IE in 3GPP TS 24.008 [8]
 *  Table 10.5.163a/3GPP TS 24.008. See also 3GPP TS 23.682 [149] and 3GPP TS 23.401 [82].
 *
 * 2. Requested_Active-Time, 8bit String of Byte. e.g. "00100101"
 *  where "001" is the unit, and "00101" is the value.
 *  For the coding and the value range, see the GPRS Timer 2 IE in 3GPP TS 24.008 [8]
 *  Table 10.5.163/3GPP TS 24.008. See also 3GPP TS 23.682 [149], 3GPP TS 23.060 [47] and
 *  3GPP TS 23.401 [82].
 */
typedef struct psm_setting_t
{
    uint8 reqPeriodicTAU[9];  ///< Requested_Periodic-TAU
    uint8 reqActiveTime[9];   ///< Requested_Active-Time
} NBIOT_PSM_SETTING;


/**
 * Get eDRX Value and PTW
 *
 * @param edrxCfg eDRX value and eDRX PTW.
 * @return 0 SUCCESS -1 FAIL
 */
int KING_NbiotGetEdrxCfg(NBIOT_EDRX_CFG *edrxCfg);

/**
 * Set eDRX Value (will auto enable eDRX)
 *
 * @param edrxValue eDRX value. Integer value range is 0 ~ 15.
 * @return 0 SUCCESS -1 FAIL
 */
int KING_NbiotSetEdrxValue(uint8 edrxValue);

/**
 * Set eDRX PTW (will auto enable eDRX)
 *
 * @param edrxPtw eDRX PTW. Integer value range is 0 ~ 15.
 * @return 0 SUCCESS -1 FAIL
 */
int KING_NbiotSetEdrxPtw(uint8 edrxPtw);

/**
 * Get PSM settings.
 *
 * @param psmSet Requested_Periodic-TAU and Requested_Active-Time
 * @return 0 SUCCESS -1 FAIL
 */
int KING_NbiotGetPsmSetting(NBIOT_PSM_SETTING *psmSet);

/**
 * Set Requested_Periodic-TAU
 *
 * @param pValue Requested_Periodic-TAU, 8bit String of Byte. e.g. "01000111"
 * @param length Size of data in pValue. Must be 8.
 * @return 0 SUCCESS -1 FAIL
 */
int KING_NbiotSetPsmT3412(uint8 *pValue, uint8 length);

/**
 * Set Requested_Active-Time
 *
 * @param pValue Requested_Active-Time, 8bit String of Byte. e.g. "01100101"
 * @param length Size of data in pValue. Must be 8.
 * @return 0 SUCCESS -1 FAIL
 */
int KING_NbiotSetPsmT3324(uint8 *pValue, uint8 length);

/**
 * Set power mode.
 *
 * @param pm POWER_MODE_0/1/2/3
 * @return 0 SUCCESS -1 FAIL
 */
int KING_NbiotSetPwrMode(uint8 pm);

/**
 * Get power mode.
 *
 * @param pm POWER_MODE_0/1/2/3
 * @return 0 SUCCESS -1 FAIL
 */
int KING_NbiotGetPwrMode(uint8 *pm);

/**
 * @brief Request to enter RAI mode in certain PDP context.
 *
 * @note System will enter PSM faster in RAI mode.
 *       Normally to call this API after all stuff are done
 *       between device and remote server, and prepare to
 *       enter deep sleeping.
 *
 * @param[in] cid   PDP Context ID, starts from 1.
 *
 * @return 0 SUCCESS -1 FAIL
 */
int KING_NbiotEnterRai(uint8 cid);

#endif // __KING_NBIOT_H__

