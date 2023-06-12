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

#ifndef _BOOT_GPIO_H_
#define _BOOT_GPIO_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_PMIC_INTR_GPIO 6


struct bootGpio;

/**
 * opaque data struct for GPIO instance
 */
typedef struct bootGpio bootGpio_t;

typedef enum
{
   GPIO_PULL_UP    = 0,
   GPIO_PULL_DOWN,
   GPIO_PULL_NONE,
   GPIO_PULL_MAX = 0x7FFFFFFF
} GPIO_PULL_MODE;


typedef enum
{
    BOOT_GPIO_INPUT,
    BOOT_GPIO_OUTPUT
} bootGpioMode_t;

/**
 * GPIO configuration
 */
typedef struct
{
    bootGpioMode_t mode; ///< GPIO mode
    bool out_level;     ///< level to be set for GPIO output
    bool intr_enabled;  ///< interrupt enabled, only for GPIO input
    bool intr_level;    ///< true for level interrupt, false for edge interrupt
    bool debounce;      ///< debounce enabled
    bool rising;        ///< rising edge or level high interrupt enabled
    bool falling;       ///< falling edge or level low interrupt enabled
} bootGpioConfig_t;

/**
 * @brief GPIO module initialization
 *
 * It just initialize GPIO module, and won't touch any GPIO. It should be
 * called before any \a bootGpioOpen.
 */
void bootGpioInit(void);

/**
 * @brief open a GPIO
 *
 * IOMUX will be set to GPIO mode at open.
 *
 * Each GPIO sould be opened only once. When it is already opened, this
 * API will return NULL.
 *
 * If the specified GPIO can't support the specified mode, this API will
 * return NULL.
 *
 * If the specified GPIO can't support input interrupt, and interrupt is
 * enabled, this API will return NULL.
 *
 * The returned instance is dynamic allocated, caller should free it after
 * \a bootGpioClose is called.
 *
 * @param id    GPIO id. The range may be different among chips.
 * @param cfg   GPIO configuration
 * @return
 *      - GPIO instance pointer
 *      - NULL if parameter is invalid
 */
bootGpio_t *bootGpioOpen(uint32_t id, bootGpioConfig_t *cfg);

/**
 * @brief close a GPIO
 *
 * IOMUX will be kept, and the GPIO will be set to input mode.
 *
 * @param d     GPIO instance
 */
void bootGpioClose(bootGpio_t *d);

/**
 * @brief reconfigure the opened GPIO
 *
 * When the configuration is invalid, the current configuration will
 * be kept.
 *
 * @param d     GPIO instance
 * @param cfg   GPIO configuration
 * @return
 *      - true if GPIO configuration is valid
 *      - false if GPIO configuration is invalid
 */
bool bootGpioReconfig(bootGpio_t *d, bootGpioConfig_t *cfg);

/**
 * @brief read the level of GPIO
 *
 * It can be called for both GPIO input and output. For GPIO output,
 * it is the level set by software.
 *
 * @param d     GPIO instance
 * @return
 *      - true for level high
 *      - false for level low
 */
bool bootGpioRead(bootGpio_t *d);

/**
 * @brief write level for GPIO
 *
 * When it is called for GPIO input, it will do nothing.
 *
 * When it is called for GPIO output, set the output level.
 *
 * @param d     GPIO instance
 * @param level GPIO level to be set,
 *              - true for level high
 *              - false for level low
 */
void bootGpioWrite(bootGpio_t *d, bool level);


void bootIomuxPullCfg(uint8_t gpio, uint32_t pull_type);

#ifdef __cplusplus
}
#endif
#endif
