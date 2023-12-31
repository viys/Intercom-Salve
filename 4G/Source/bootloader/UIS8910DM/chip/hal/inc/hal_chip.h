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

#ifndef _HAL_CHIP_H_
#define _HAL_CHIP_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmsis_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_CHIP_ID_UNKNOWN (0xffff0000)
#define HAL_CHIP_ID_8955 (0x89550000)
#define HAL_CHIP_ID_8909 (0x89090000)
#define HAL_CHIP_ID_8910 (0x89100000)

#define HAL_FREQ_16K (16384)
#define HAL_FREQ_32K (32768)
#define HAL_FREQ_2M (2 * 1000000)
#define HAL_FREQ_13M (13 * 1000000)
#define HAL_FREQ_20M (20 * 1000000)
#define HAL_FREQ_26M (26 * 1000000)
#define HAL_FREQ_39M (39 * 1000000)
#define HAL_FREQ_45M (45 * 1000000)
#define HAL_FREQ_52M (52 * 1000000)
#define HAL_FREQ_57M (57 * 1000000)
#define HAL_FREQ_62M (62 * 1000000)
#define HAL_FREQ_69M (69 * 1000000)
#define HAL_FREQ_78M (78 * 1000000)
#define HAL_FREQ_89M (89 * 1000000)
#define HAL_FREQ_104M (104 * 1000000)
#define HAL_FREQ_113M (113 * 1000000)
#define HAL_FREQ_125M (125 * 1000000)
#define HAL_FREQ_139M (139 * 1000000)
#define HAL_FREQ_156M (156 * 1000000)
#define HAL_FREQ_178M (178 * 1000000)
#define HAL_FREQ_208M (208 * 1000000)
#define HAL_FREQ_250M (250 * 1000000)
#define HAL_FREQ_312M (312 * 1000000)

#define HAL_NAME_DMA BOOT_MAKE_TAG('D', 'M', 'A', ' ')
#define HAL_NAME_LTE_F8 BOOT_MAKE_TAG('L', 'T', 'F', '8')
#define HAL_NAME_SIM BOOT_MAKE_TAG('S', 'I', 'M', ' ')
#define HAL_NAME_XCV BOOT_MAKE_TAG('X', 'C', 'V', ' ')
#define HAL_NAME_CALIB BOOT_MAKE_TAG('C', 'A', 'L', 'I')
#define HAL_NAME_PAL BOOT_MAKE_TAG('P', 'A', 'L', ' ')
#define HAL_NAME_USB BOOT_MAKE_TAG('U', 'S', 'B', ' ')
#define HAL_NAME_NETCARD BOOT_MAKE_TAG('N', 'E', 'T', 'C')
#define HAL_NAME_CAM BOOT_MAKE_TAG('C', 'A', 'M', ' ')
#define HAL_NAME_PWM BOOT_MAKE_TAG('P', 'W', 'M', ' ')

#define REG_APPLY_TYPE_END 0
#define REG_APPLY_TYPE_UDELAY 1

#define REG_APPLY_WRITE(address, value) (uint32_t)(address), (uint32_t)(value)
#define REG_APPLY_UDELAY(us) (uint32_t)(REG_APPLY_TYPE_UDELAY), (uint32_t)(us)
#define REG_APPLY_END REG_APPLY_TYPE_END

enum
{
    HAL_CLK_DIV1P0 = 2,
    HAL_CLK_DIV1P5,
    HAL_CLK_DIV2P0,
    HAL_CLK_DIV2P5,
    HAL_CLK_DIV3P0,
    HAL_CLK_DIV3P5,
    HAL_CLK_DIV4P0,
    HAL_CLK_DIV4P5,
    HAL_CLK_DIV5P0,
    HAL_CLK_DIV5P5,
    HAL_CLK_DIV6P0 = 12,
    HAL_CLK_DIV7P0 = 14,
    HAL_CLK_DIV8P0 = 16,
    HAL_CLK_DIV12P0 = 24,
    HAL_CLK_DIV16P0 = 32,
    HAL_CLK_DIV24P0 = 48,
};

enum
{
    HAL_METAL_ID_U01 = 1,
    HAL_METAL_ID_U02,
    HAL_METAL_ID_U03,
    HAL_METAL_ID_U04,
};

#define HAL_METAL_BEFORE(metal, n) ((metal) < (n))
#define HAL_METAL_FROM(metal, n) ((metal) >= (n))

enum
{
    HAL_VCORE_USER_SYS,
    HAL_VCORE_USER_VOC,
    HAL_VCORE_USER_COUNT,
};

enum
{
    HAL_VCORE_LEVEL_LOW,
    HAL_VCORE_LEVEL_MEDIUM,
    HAL_VCORE_LEVEL_HIGH,
    HAL_VCORE_LEVEL_COUNT,
};

enum
{
    HAL_VCORE_MODE_LDO,
    HAL_VCORE_MODE_DCDC
};

enum
{
    HAL_POWER_DCDC_GEN,
    HAL_POWER_CORE,
    HAL_POWER_BUCK_PA,
    HAL_POWER_SPIMEM,
    HAL_POWER_MEM,
    HAL_POWER_AVDD18,
    HAL_POWER_RF15,
    HAL_POWER_WCN,
    HAL_POWER_DDR12,
    HAL_POWER_CAMD,
    HAL_POWER_CAMA,
    HAL_POWER_USB,
    HAL_POWER_LCD,
    HAL_POWER_SD,
    HAL_POWER_SIM0,
    HAL_POWER_SIM1,
    HAL_POWER_VIBR,
    HAL_POWER_DCXO,
    HAL_POWER_VDD28,
    HAL_POWER_VIO18,
    HAL_POWER_KEYLED,
    HAL_POWER_BACK_LIGHT,
    HAL_POWER_ANALOG,
};

typedef enum
{
    HAL_SIM_VOLT_CLASS_A = 0, // 5v
    HAL_SIM_VOLT_CLASS_B = 1, // 3.3v
    HAL_SIM_VOLT_CLASS_C = 2, // 1.8v
    HAL_SIM_VOLT_OFF = 3,
} halSimVoltClass_t;

typedef enum
{
    SENSOR_VDD_3300MV = 3300,
    SENSOR_VDD_3000MV = 3000,
    SENSOR_VDD_2800MV = 2800,
    SENSOR_VDD_2500MV = 2500,
    SENSOR_VDD_1800MV = 1800,
    SENSOR_VDD_1500MV = 1500,
    SENSOR_VDD_1300MV = 1300,
    SENSOR_VDD_1200MV = 1200,
    SENSOR_VDD_CLOSED,
    SENSOR_VDD_UNUSED
} cameraVDD_t;


typedef enum
{
    POWER_LEVEL_3200MV = 3200,
    POWER_LEVEL_3100MV = 3100,
    POWER_LEVEL_3000MV = 3000,
    POWER_LEVEL_2900MV = 2900,
    POWER_LEVEL_2800MV = 2800,
    POWER_LEVEL_2700MV = 2700,
    POWER_LEVEL_2600MV = 2600,
    POWER_LEVEL_2500MV = 2500,
    POWER_LEVEL_2400MV = 2400,
    POWER_LEVEL_2300MV = 2300,
    POWER_LEVEL_2200MV = 2200,
    POWER_LEVEL_2100MV = 2100,
    POWER_LEVEL_2000MV = 2000,
    POWER_LEVEL_1900MV = 1900,
    POWER_LEVEL_1800MV = 1800,
    POWER_LEVEL_1700MV = 1700,
    POWER_LEVEL_1600MV = 1600,
    POWER_LEVEL_1500MV = 1500,
    POWER_LEVEL_1400MV = 1400,
    POWER_LEVEL_1300MV = 1300,
    POWER_LEVEL_1200MV = 1200,
    POWER_LEVEL_1100MV = 1100,
    POWER_LEVEL_UNUSED
} powerLevel_t;


/**
 * \brief read chip id
 *
 * The returned chip id is not the raw data of hardware registers, it is the
 * *logical* chip id.
 *
 * Due to the method to detect chip id is different among various chips, the
 * implementation depends on chip. When the hardware register is unexpected,
 * it returns \p HAL_CHIP_ID_UNKNOWN.
 *
 * \return  *logical* chip id
 */
uint32_t halGetChipId(void);

/**
 * \brief read chip metal id
 *
 * When the hardware register is unexpected, the returned value is the value
 * the latest metal id. For example, when current revision is \p U02, all
 * unexpected values will be regarded as \p U02.
 *
 * When it is needed to distinguish metal id, **never** use the raw hardware
 * register, and always use macros for metal id check. Examples:
 *
 * - HAL_METAL_BEFORE(metal, HAL_METAL_ID_U02): true for metal id *less than*
 *   U02, and not including U02.
 * - HAL_METAL_FROM(metal, HAL_METAL_ID_U02): true for metal id
 *   *larger or equal than* U02, and including U02.
 *
 * \return  *logical* metal id
 */
uint32_t halGetMetalId(void);

/**
 * \brief bare metal boot initialization
 *
 * This will be called before run time initialization. Only the small piece of
 * codes are copied from flash into SRAM, namely *.sramboottext* section. All
 * *.data* and *.bss* sections are not initialized. Typically, it can access:
 *
 * - stack
 * - flash codes
 * - osiDebugEvent, osiDelayUS, halApplyRegisters, halApplyRegisterList
 */
void halChipPreBoot(void);

/**
 * \brief clock and memory initialization
 *
 * When this is called, only partial run time is initialized. All sections on
 * SRAM can be accessed, and sections on external RAM (PSRAM or DDR) can't be
 * accessed.
 */
void halClockInit(void);

/**
 * \brief PSRAM/DDR initialization at cold boot
 */
void halRamInit(void);

/**
 * \brief PSRAM/DDR initialization at resume
 */
void halRamWakeInit(void);

/**
 * \brief batch write write registers, and executed on SRAM
 *
 * This function should be located in *.sramboottext* section, and the section
 * will be loaded to SRAM at the beginning of boot.
 *
 * The main purpose of this is to write registers which may affect memory
 * access. For example, it will make flash unaccessible to write some flash
 * controller registers.
 *
 * It use variadic variables. The variadic variable format is:
 *
 * - (address, value): write the value to specified register
 * - (REG_APPLY_TYPE_UDELAY, value): delay specified microseconds
 * - (REG_APPLY_TYPE_END): indicate the end of variadic variables
 *
 * \param [in] address  register address, or REG_APPLY_TYPE_UDELAY,
 *                      REG_APPLY_TYPE_END
 */
void halApplyRegisters(uint32_t address, ...);

/**
 * \brief batch write write registers, and executed on SRAM
 *
 * This function should be located in *.sramboottext* section, and the section
 * will be loaded to SRAM at the beginning of boot.
 *
 * It is similar to \p halApplyRegisters, just the parameters are located in
 * memory pointed by \p data.
 *
 * \param [in ] data    batch write parameter pointer
 */
void halApplyRegisterList(const uint32_t *data);

uint32_t halFreqToPllDivider(uint32_t freq);
void hal_SysSetBBClock(uint32_t freq);
void hal_SysStartBcpu(void *bcpu_main, void *stack_start_addr);

void halChangeSysClk(uint32_t freq, bool extram_access);

void halIspiInit(void);
uint16_t halPmuRead(volatile uint32_t *reg);
void halPmuWrite(volatile uint32_t *reg, uint16_t value);
uint16_t halAbbRead(volatile uint32_t *reg);
void halAbbWrite(volatile uint32_t *reg, uint16_t value);

void halPmuInit(void);

uint32_t halGetVcoreMode(void);
uint32_t halGetVcoreLevel(void);
uint8_t halRequestVcoreRegs(uint32_t id, uint32_t level, uint16_t *addr, uint16_t *value);

bool halPmuSwitchPower(uint32_t id, bool enabled, bool lp_enabled);
bool halPmuSetPowerLevel(uint32_t id, uint32_t mv);

void halPmuEnterPm1(void);
void halPmuAbortPm1(void);
void halPmuExitPm1(void);

void halPmuEnterPm2(void);
void halPmuAbortPm2(void);
void halPmuExitPm2(void);

bool halPmuSetSimVolt(int idx, halSimVoltClass_t volt);
bool halPmuSelectSim(int idx);

__NO_RETURN void halShutdown(int mode, int64_t wake_uptime);
void halPmuSet7sReset(bool enable);


#ifdef __cplusplus
}
#endif
#endif
