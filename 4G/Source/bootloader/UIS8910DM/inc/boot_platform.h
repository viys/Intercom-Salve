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
#include "boot_adi_bus.h"
#include "boot_trace.h"
#include "boot_time.h"
#include "boot_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOOT_STRONG_ALIAS(alias, sym) __asm(".global " #alias "\n" #alias " = " #sym)


#define CONFIG_DEFAULT_SYSAHB_FREQ 200000000
#define CONFIG_CACHE_LINE_SIZE 32

#define halAdiBusWrite bootAdiBusWrite
#define halAdiBusRead bootAdiBusRead
#define halAdiBusChange bootAdiBusChange
#define halAdiBusBatchChange bootAdiBusBatchChange


#define BOOT_ASSERT(expect_true, info) BOOT_DO_WHILE0(if (!(expect_true)) bootPanic();)


#ifndef SEEK_SET
#define SEEK_SET 0
#endif /* SEEK_SET */

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif /* SEEK_CUR */

#ifndef SEEK_END
#define SEEK_END 2
#endif /* SEEK_END */

/**
 * boot mode
 *
 * Besides normal boot, there are several other boot modes. For each
 * platform, not all boot modes are supported.
 *
 * Usually, boot mode can be determined by hardware (for example, some
 * GPIO) or software (for example, by flags written at \p osiShutdown).
 */
typedef enum bootMode
{
    BOOTMODE_NORMAL = 0,           ///< normal boot
    BOOTMODE_DOWNLOAD = 0x444e,    ///< 'DN' boot to download mode
    BOOTMODE_CALIB = 0x434c,       ///< 'CL' boot to calibration mode
    BOOTMODE_CALIB_POST = 0x4350,  ///< 'CP' boot to calibration post mode
    BOOTMODE_NB_CALIB = 0x4e43,    ///< 'NC' boot to NB calibration mode
    BOOTMODE_BBAT = 0x4241,        ///< 'BA' boot to BBAT mode
    BOOTMODE_UPGRADE = 0x4654,     ///< 'FT' boot to bootloader upgrade
    BOOTMODE_PSM_RESTORE = 0x5053, ///< 'PS' boot to PSM restore
} bootMode_t;


/**
 * \brief boot cause
 *
 * This list is for cold boot cause. Though it is rare, it is possible
 * there exist multiple boot causes simultanuously.
 *
 * Usually boot cause is determined from hardware status registers.
 */
typedef enum bootCause
{
    BOOTCAUSE_UNKNOWN = 0,           ///< placeholder for unknown reason
    BOOTCAUSE_PWRKEY = (1 << 0),     ///< boot by power key
    BOOTCAUSE_PIN_RESET = (1 << 1),  ///< boot by pin reset
    BOOTCAUSE_ALARM = (1 << 2),      ///< boot by alarm
    BOOTCAUSE_CHARGE = (1 << 3),     ///< boot by charge in
    BOOTCAUSE_WDG = (1 << 4),        ///< boot by watchdog
    BOOTCAUSE_PIN_WAKEUP = (1 << 5), ///< boot by wakeup
    BOOTCAUSE_PSM_WAKEUP = (1 << 6), ///< boot from PSM wakeup
    BOOTCAUSE_PANIC = (1 << 9),      ///< boot by panic reset
} bootCause_t;

/**
 * shudown mode
 *
 * For each platform, not all shutdown modes are supported.
 */
typedef enum shutdownMode
{
    SHUTDOWN_RESET = 0,               ///< normal reset
    SHUTDOWN_FORCE_DOWNLOAD = 0x5244, ///< 'RD' reset to force download mode
    SHUTDOWN_DOWNLOAD = 0x444e,       ///< 'DN' reset to download mode
    SHUTDOWN_CALIB_MODE = 0x434c,     ///< 'CL' reset to calibration mode
    SHUTDOWN_NB_CALIB_MODE = 0x4e43,  ///< 'NC' reset to NB calibration mode
    SHUTDOWN_BBAT_MODE = 0x4241,      ///< 'BA' reset to BBAT mode
    SHUTDOWN_UPGRADE = 0x4654,        ///< 'FT' reset to upgrade mode
    SHUTDOWN_POWER_OFF = 0x4f46,      ///< 'OF' power off
    SHUTDOWN_PSM_SLEEP = 0x5053,      ///< 'PS' power saving mode
} shutdownMode_t;

typedef enum bootResetMode
{
    BOOT_RESET_NORMAL,         ///< normal reset
    BOOT_RESET_FORCE_DOWNLOAD, ///< reset to force download mode
} bootResetMode_t;

// do { ... } while (0) is common trick to avoid if/else error
#define BOOT_DO_WHILE0(expr) \
    do                      \
    {                       \
        expr                \
    } while (0)

// just a dead loop
#define BOOT_DEAD_LOOP BOOT_DO_WHILE0(for (;;);)

#define BOOT_SECTION(sect) __attribute__((section(#sect)))
#define BOOT_SECTION_SRAM_BOOT_TEXT BOOT_SECTION(.sramboottext)
#define BOOT_SECTION_SRAM_TEXT BOOT_SECTION(.sramtext)
#define BOOT_SECTION_SRAM_DATA BOOT_SECTION(.sramdata)
#define BOOT_SECTION_SRAM_BSS BOOT_SECTION(.srambss)
#define BOOT_SECTION_SRAM_UNINIT BOOT_SECTION(.sramuninit)
#define BOOT_SECTION_SRAM_UC_DATA BOOT_SECTION(.sramucdata)
#define BOOT_SECTION_SRAM_UC_BSS BOOT_SECTION(.sramucbss)
#define BOOT_SECTION_SRAM_UC_UNINIT BOOT_SECTION(.sramucuninit)
#define BOOT_SECTION_RAM_TEXT BOOT_SECTION(.ramtext)
#define BOOT_SECTION_RAM_DATA BOOT_SECTION(.ramdata)
#define BOOT_SECTION_RAM_BSS BOOT_SECTION(.rambss)
#define BOOT_SECTION_RAM_UNINIT BOOT_SECTION(.ramuninit)
#define BOOT_SECTION_RAM_UC_DATA BOOT_SECTION(.ramucdata)
#define BOOT_SECTION_RAM_UC_BSS BOOT_SECTION(.ramucbss)
#define BOOT_SECTION_RAM_UC_UNINIT BOOT_SECTION(.ramucuninit)
#define BOOT_SECTION_BOOT_TEXT BOOT_SECTION(.boottext)
#define BOOT_SECTION_RO_KEEP __attribute__((used, section(".rokeep")))
#define BOOT_SECTION_RW_KEEP __attribute__((used, section(".rwkeep")))
#define BOOT_FORCE_INLINE                  __attribute__((always_inline)) inline

uint32_t bootEnterCritical(void);
uint32_t bootExitCritical(uint32_t flag);


// macro for fourcc tag
#define BOOT_MAKE_TAG(a, b, c, d) ((unsigned)(a) | ((unsigned)(b) << 8) | ((unsigned)(c) << 16) | ((unsigned)(d) << 24))

#define DRV_NAME_DEBUGUART BOOT_MAKE_TAG('D', 'U', 'R', 'T')
#define DRV_NAME_CP BOOT_MAKE_TAG('C', 'P', ' ', ' ')
#define DRV_NAME_SCI1 BOOT_MAKE_TAG('S', 'C', 'I', '1')
#define DRV_NAME_SCI2 BOOT_MAKE_TAG('S', 'C', 'I', '2')
#define DRV_NAME_SPI1 BOOT_MAKE_TAG('S', 'P', 'I', '1')
#define DRV_NAME_SPI2 BOOT_MAKE_TAG('S', 'P', 'I', '2')
#define DRV_NAME_SPI3 BOOT_MAKE_TAG('S', 'P', 'I', '3')
#define DRV_NAME_UART1 BOOT_MAKE_TAG('U', 'R', 'T', '1')
#define DRV_NAME_UART2 BOOT_MAKE_TAG('U', 'R', 'T', '2')
#define DRV_NAME_UART3 BOOT_MAKE_TAG('U', 'R', 'T', '3')
#define DRV_NAME_UART4 BOOT_MAKE_TAG('U', 'R', 'T', '4')
#define DRV_NAME_SPI_FLASH BOOT_MAKE_TAG('S', 'F', 'L', '1')
#define DRV_NAME_SPI_FLASH_EXT BOOT_MAKE_TAG('S', 'F', 'L', '2')
#define DRV_NAME_AON_WDT BOOT_MAKE_TAG('A', 'O', 'W', 'D')
#define DRV_NAME_USB11 BOOT_MAKE_TAG('U', 'S', 'B', '1')
#define DRV_NAME_USB20 BOOT_MAKE_TAG('U', 'S', 'B', '2')
#define DRV_NAME_USRL_COM0 BOOT_MAKE_TAG('U', 'S', 'L', '0')
#define DRV_NAME_USRL_COM1 BOOT_MAKE_TAG('U', 'S', 'L', '1')
#define DRV_NAME_USRL_COM2 BOOT_MAKE_TAG('U', 'S', 'L', '2')
#define DRV_NAME_USRL_COM3 BOOT_MAKE_TAG('U', 'S', 'L', '3')
#define DRV_NAME_USRL_COM4 BOOT_MAKE_TAG('U', 'S', 'L', '4')
#define DRV_NAME_USRL_COM5 BOOT_MAKE_TAG('U', 'S', 'L', '5')
#define DRV_NAME_USRL_COM6 BOOT_MAKE_TAG('U', 'S', 'L', '6')
#define DRV_NAME_USRL_COM7 BOOT_MAKE_TAG('U', 'S', 'L', '7')
#define DRV_NAME_SDMMC1 BOOT_MAKE_TAG('S', 'D', 'C', '1')
#define DRV_NAME_SDMMC2 BOOT_MAKE_TAG('S', 'D', 'C', '2')
#define DRV_NAME_CAMERA1 BOOT_MAKE_TAG('C', 'A', 'M', '1')
#define DRV_NAME_I2C1 BOOT_MAKE_TAG('I', '2', 'C', '1')
#define DRV_NAME_I2C2 BOOT_MAKE_TAG('I', '2', 'C', '2')
#define DRV_NAME_I2C3 BOOT_MAKE_TAG('I', '2', 'C', '3')
#define DRV_NAME_WCN BOOT_MAKE_TAG('W', 'C', 'N', ' ')
#define DRV_NAME_PWM1 BOOT_MAKE_TAG('P', 'W', 'M', '1')

void bootPanic(void);
void bootDebugEvent(uint32_t event);
void bootReset(bootResetMode_t mode);
void bootPowerOff(void);
bool bootIsFromPsmSleep(void);
int bootPowerOnCause(void);
void bootDCacheClean(const void *address, size_t size);
void bootDCacheInvalidate(const void *address, size_t size);
void bootDCacheCleanAll();

#ifdef __cplusplus
}
#endif
#endif
