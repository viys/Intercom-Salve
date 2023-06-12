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

#include "hwregs.h"
#include "cs_types.h"
#include "hal_chip.h"
#include "boot_rtc.h"
#include "boot_app.h"
#include "boot_platform.h"
#include "boot_compiler.h"
#include "boot_adi_bus.h"
#include "boot_tick_unit.h"

void bootReadRtcTime(mytime_t* mytime,bool daylightSaving)
{
    if(s_bootAppParamPtr != NULL)
    {
        s_bootAppParamPtr->bootReadRtcTime(mytime,daylightSaving);
    }
}
