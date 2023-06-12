/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _BOOT_PMIC_INTR_H_
#define _BOOT_PMIC_INTR_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * PMIC interrupt types
 */
enum bootPmicIntrType_t
{
    BOOT_PMIC_INTR_ADC, ///< PMIC ADC interrupt
    BOOT_PMIC_INTR_RTC, ///< PMIC RTC interrupt
    BOOT_PMIC_INTR_WDG, ///< PMIC watchdog interrupt
    BOOT_PMIC_INTR_FGU, ///< PMIC fuel gauge unit interrupt
    BOOT_PMIC_INTR_EIC, ///< PMIC EIC interrupt
    BOOT_PMIC_INTR_AUD, ///< PMIC audio interrupt
    BOOT_PMIC_INTR_TMR, ///< PMIC timer interrupt
    BOOT_PMIC_INTR_CAL, ///< PMIC oscillator calibration interrupt

    BOOT_PMIC_INTR_COUNT
};

/**
 * PMIC EIC interrupt types
 */
enum bootPmicEicType_t
{
    BOOT_PMIC_EIC_CHGR_INT,              ///< charge indicator
    BOOT_PMIC_EIC_PBINT,                 ///< power on 1
    BOOT_PMIC_EIC_PBINT2,                ///< power on 2
    BOOT_PMIC_EIC_AUDIO_HEAD_BUTTON,     ///< ??
    BOOT_PMIC_EIC_CHGR_CV,               ///< ??
    BOOT_PMIC_EIC_AUDIO_HEAD_INSERT,     ///< ??
    BOOT_PMIC_EIC_VCHG_OVI,              ///< ??
    BOOT_PMIC_EIC_AUDIO_HEAD_INSERT2,    ///< ??
    BOOT_PMIC_EIC_BATDET_OK,             ///< ??
    BOOT_PMIC_EIC_EXT_RSTN,              ///< ??
    BOOT_PMIC_EIC_EXT_XTL_EN0,           ///< ??
    BOOT_PMIC_EIC_AUDIO_HEAD_INSERT3,    ///< ??
    BOOT_PMIC_EIC_AUDIO_HEAD_INSERT_ALL, ///< ??
    BOOT_PMIC_EIC_EXT_XTL_EN1,           ///< ??
    BOOT_PMIC_EIC_EXT_XTL_EN2,           ///< ??
    BOOT_PMIC_EIC_EXT_XTL_EN3,           ///< ??

    BOOT_PMIC_EIC_COUNT
};

/**
 * @brief PMIC EIC source current level
 *
 * @param eic   PMIC EIC interrupt type (\a bootPmicEicType_t)
 * @return
 *      - true for high level
 *      - false for low level, or invalid type
 */
bool bootPmicEicGetLevel(unsigned eic);

void bootPmicInit(void);


#ifdef __cplusplus
}
#endif
#endif
