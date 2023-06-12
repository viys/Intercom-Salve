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

#ifndef _BOOT_PLATFORM_H_
#define _BOOT_PLATFORM_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "hwregs.h"
#include "boot_compiler.h"
#include "boot_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_NAME_UART1 BOOT_MAKE_TAG('U', 'R', 'T', '1')
#define BOOT_NAME_UART2 BOOT_MAKE_TAG('U', 'R', 'T', '2')
#define BOOT_NAME_UART3 BOOT_MAKE_TAG('U', 'R', 'T', '3')
#define BOOT_NAME_UART4 BOOT_MAKE_TAG('U', 'R', 'T', '4')
#define BOOT_NAME_UART5 BOOT_MAKE_TAG('U', 'R', 'T', '5')
#define BOOT_NAME_I2C1 BOOT_MAKE_TAG('I', '2', 'C', '1')
#define BOOT_NAME_I2C2 BOOT_MAKE_TAG('I', '2', 'C', '2')
#define BOOT_NAME_I2C3 BOOT_MAKE_TAG('I', '2', 'C', '3')

#ifndef SEEK_SET
#define SEEK_SET 0
#endif /* SEEK_SET */

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif /* SEEK_CUR */

#ifndef SEEK_END
#define SEEK_END 2
#endif /* SEEK_END */

typedef enum bootResetMode
{
    BOOT_RESET_NORMAL,         ///< normal reset
    BOOT_RESET_FORCE_DOWNLOAD, ///< reset to force download mode
} bootResetMode_t;

void bootReset(bootResetMode_t mode);
void bootPowerOff(void);
void bootPanic(void);
uint32_t bootEnterCritical(void);
uint32_t bootExitCritical(uint32_t flag);

#ifdef __cplusplus
}
#endif
#endif
