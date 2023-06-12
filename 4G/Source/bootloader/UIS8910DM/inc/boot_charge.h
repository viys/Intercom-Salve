/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _BOOT_CHARGE_H_
#define _BOOT_CHARGE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CHARGER_CURRENT_300MA 300
#define CHARGER_CURRENT_350MA 350
#define CHARGER_CURRENT_400MA 400
#define CHARGER_CURRENT_450MA 450
#define CHARGER_CURRENT_500MA 500
#define CHARGER_CURRENT_550MA 550
#define CHARGER_CURRENT_600MA 600
#define CHARGER_CURRENT_650MA 650
#define CHARGER_CURRENT_700MA 700
#define CHARGER_CURRENT_750MA 750
#define CHARGER_CURRENT_800MA 800
//#define CHARGER_CURRENT_850MA   850
#define CHARGER_CURRENT_900MA 900
//#define CHARGER_CURRENT_950MA   950
#define CHARGER_CURRENT_1000MA 1000
//#define CHARGER_CURRENT_1050MA  1050
#define CHARGER_CURRENT_1100MA 1100
#define CHARGER_CURRENT_1200MA 1200
#define CHARGER_CURRENT_MAX 1300


void bootChargerInit(void);
void bootChargerTurnOn(void);
void bootChargerTurnOff(void);
uint32_t bootChargerGetVbatVol(void);
bool bootChargePlugged();
void bootChargerSetEndVolt(uint32_t mv);
void bootChargerSetChangerCurrent(uint16_t current);
bool bootChargerGetCVStatus(void);
void bootSinkVIBRCfg(bool level);


#ifdef __cplusplus
}
#endif
#endif
