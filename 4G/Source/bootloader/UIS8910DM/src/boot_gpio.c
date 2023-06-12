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

#include "boot_gpio.h"
#include "boot_platform.h"
#include "hwregs.h"
#include <string.h>
#include <stdlib.h>
#include "iomux.h"
#define GPIO_COUNT (32)

struct bootGpio
{
    uint32_t id;

    bootGpioMode_t mode;
    bool intr_enabled;
    bool intr_level;
    bool debounce;
    bool rising;
    bool falling;
};

typedef struct
{
    uint32_t oen;
    uint32_t val;
    uint32_t int_ctrl_r;
    uint32_t int_ctrl_f;
    uint32_t int_mode;
    uint32_t dbn;
} bootGpioSuspend_t;

typedef struct
{
    bootGpio_t *gpios[GPIO_COUNT];
    bootGpioSuspend_t suspend;
} bootGpioContext_t;

static bootGpioContext_t gbootGpioCtx;


static bool _isConfigValid(uint32_t id, bootGpioConfig_t *cfg)
{
    if (id >= GPIO_COUNT || cfg == NULL)
        return false;

    return true;
}


void bootGpioInit(void)
{
    bootGpioContext_t *p = &gbootGpioCtx;
    memset(p->gpios, 0, sizeof(p->gpios));

    hwp_gpio1->gpio_clr_reg = 0xffffffff;
}

bootGpio_t *bootGpioOpen(uint32_t id, bootGpioConfig_t *cfg)
{
    bootGpioContext_t *p = &gbootGpioCtx;

    uint32_t critical = bootEnterCritical();
    if (!_isConfigValid(id, cfg))
    {
        bootExitCritical(critical);
        return NULL;
    }
    if (p->gpios[id] != NULL)
        return p->gpios[id];

    bootGpio_t *d = (bootGpio_t *)calloc(1, sizeof(bootGpio_t));
    d->id = id;
    p->gpios[id] = d;
    bootGpioReconfig(d, cfg);

    bootExitCritical(critical);
    return d;
}

bool bootGpioReconfig(bootGpio_t *d, bootGpioConfig_t *cfg)
{
    if (d == NULL || !_isConfigValid(d->id, cfg))
        return false;

    uint32_t critical = bootEnterCritical();

    d->mode = cfg->mode;
    d->intr_enabled = cfg->intr_enabled;
    d->intr_level = cfg->intr_level;
    d->debounce = cfg->debounce;
    d->rising = cfg->rising;
    d->falling = cfg->falling;

    HWP_GPIO_T *hwp = hwp_gpio1;
    uint32_t lid = d->id;

    hwp->gpint_ctrl_r_clr_reg = (1 << lid);
    hwp->gpint_ctrl_f_clr_reg = (1 << lid);
    hwp->dbn_en_clr_reg = (1 << lid);
    hwp->gpint_mode_clr_reg = (1 << lid);

    if (cfg->mode == BOOT_GPIO_INPUT)
    {
        hwp->gpio_oen_set_in = (1 << lid);
    }
    else
    {
        if (cfg->out_level)
            hwp->gpio_set_reg = (1 << lid);
        else
            hwp->gpio_clr_reg = (1 << lid);
        hwp->gpio_oen_set_out = (1 << lid);
    }

    if (cfg->intr_enabled)
    {
        if (cfg->debounce)
            hwp->dbn_en_set_reg = (1 << lid);
        if (cfg->intr_level)
            hwp->gpint_mode_set_reg = (1 << lid);
        if (cfg->rising)
            hwp->gpint_ctrl_r_set_reg = (1 << lid);
        if (cfg->falling)
            hwp->gpint_ctrl_f_set_reg = (1 << lid);
    }

    bootExitCritical(critical);
    return true;
}

void bootGpioClose(bootGpio_t *d)
{
    bootGpioContext_t *p = &gbootGpioCtx;
    uint32_t critical = bootEnterCritical();

    if (d == NULL || d->id >= GPIO_COUNT || d != p->gpios[d->id])
    {
        bootExitCritical(critical);
        return;
    }

    HWP_GPIO_T *hwp = hwp_gpio1;
    uint32_t lid = d->id;

    hwp->gpint_ctrl_r_clr_reg = (1 << lid);
    hwp->gpint_ctrl_f_clr_reg = (1 << lid);
    hwp->dbn_en_clr_reg = (1 << lid);
    hwp->gpint_mode_clr_reg = (1 << lid);

    hwp->gpio_oen_set_in = (1 << lid);
    p->gpios[d->id] = NULL;
    bootExitCritical(critical);
}

bool bootGpioRead(bootGpio_t *d)
{
    bootGpioContext_t *p = &gbootGpioCtx;
    uint32_t critical = bootEnterCritical();

    if (d == NULL || d->id >= GPIO_COUNT || d != p->gpios[d->id])
    {
        bootExitCritical(critical);
        return false;
    }

    HWP_GPIO_T *hwp = hwp_gpio1;
    uint32_t lid = d->id;

    uint32_t res = hwp->gpio_val_reg & (1 << lid);
    bootExitCritical(critical);
    return res == 0 ? false : true;
}

void bootGpioWrite(bootGpio_t *d, bool level)
{
    bootGpioContext_t *p = &gbootGpioCtx;
    uint32_t critical = bootEnterCritical();

    if (d == NULL || d->id >= GPIO_COUNT || d != p->gpios[d->id])
    {
        bootExitCritical(critical);
        return;
    }

    HWP_GPIO_T *hwp = hwp_gpio1;
    uint32_t lid = d->id;

    if (level)
        hwp->gpio_set_reg = (1 << lid);
    else
        hwp->gpio_clr_reg = (1 << lid);
    bootExitCritical(critical);
}

void bootIomuxPullCfg(uint8_t gpio, uint32_t pull_type)
{
    HWP_IOMUX_T *hwp = hwp_iomux;
    switch(gpio)
    {
        case 7:
        {
            REG_IOMUX_PAD_GPIO_7_CFG_REG_T gpio_cfg = {0};
            if (GPIO_PULL_NONE == pull_type)
            {
                gpio_cfg.b.pad_gpio_7_pull_frc = 0;
                gpio_cfg.b.pad_gpio_7_pull_dn = 0;
                gpio_cfg.b.pad_gpio_7_pull_up = 0;
            }
            else
            {
                gpio_cfg.b.pad_gpio_7_pull_frc = 1;
                gpio_cfg.b.pad_gpio_7_pull_dn = (GPIO_PULL_DOWN == pull_type) ? 1 : 0;
                gpio_cfg.b.pad_gpio_7_pull_up = (GPIO_PULL_UP == pull_type) ? 1 : 0;
            }
            hwp->pad_gpio_7_cfg_reg = (hwp->pad_gpio_7_cfg_reg | gpio_cfg.v);
            break;
        }
        case 22:
        {
            REG_IOMUX_PAD_GPIO_22_CFG_REG_T gpio_cfg = {0};
            if (GPIO_PULL_NONE == pull_type)
            {
                gpio_cfg.b.pad_gpio_22_pull_frc = 0;
                gpio_cfg.b.pad_gpio_22_pull_dn = 0;
                gpio_cfg.b.pad_gpio_22_pull_up = 0;
            }
            else
            {
                
                gpio_cfg.b.pad_gpio_22_pull_frc = 1;
                gpio_cfg.b.pad_gpio_22_pull_dn = (GPIO_PULL_DOWN == pull_type) ? 1 : 0;
                gpio_cfg.b.pad_gpio_22_pull_up = (GPIO_PULL_UP == pull_type) ? 1 : 0;
            }
            hwp->pad_gpio_22_cfg_reg = (hwp->pad_gpio_22_cfg_reg | gpio_cfg.v);
            break;
        }
    }
}
