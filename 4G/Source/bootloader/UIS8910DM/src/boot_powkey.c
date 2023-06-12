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

#include "boot_powkey.h"
#include "boot_platform.h"
#include "boot_mem.h"
#include "boot_pmic_intr.h"
#include "hwregs.h"
#include <stdlib.h>

/*****************************************************************************/
//  Description:    Get charge plug status
/*****************************************************************************/
bool bootGetPowKey()
{
    bool level = bootPmicEicGetLevel(BOOT_PMIC_EIC_PBINT);
    return level;
}
