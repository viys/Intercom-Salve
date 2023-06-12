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

#define GPIO_COUNT (38)
#define GPIO1_GROUP_0_MAX_ID 7
#define GPIO2_GROUP_L_MAX_ID 31
#define GPIO2_GROUP_H_MAX_ID 38

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
    uint32_t oen_l;
    uint32_t val_l;
    uint32_t int_r_l;
    uint32_t int_f_l;
    uint32_t mode_l;
    uint32_t dbn_l;
    uint32_t oen_h;
    uint32_t val_h;
    uint32_t int_r_h;
    uint32_t int_f_h;
    uint32_t mode_h;
    uint32_t dbn_h;
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

    // set disable mask
    hwp_gpio1->gpio_clr = 0xff;
    hwp_gpio2->gpio_clr_l = 0xffffffff;
    hwp_gpio2->gpio_clr_h = 0xffffffff;
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

    bootGpio_t *d = ((p->gpios[id] != NULL)) ? p->gpios[id] : (bootGpio_t *)calloc(1, sizeof(bootGpio_t));
    if (d == NULL)
    {
        bootExitCritical(critical);
        return NULL;
    }

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

    HWP_GPIO1_T *hwp1 = hwp_gpio1;
    HWP_GPIO_T *hwp2 = hwp_gpio2;
    uint32_t lid = d->id;

    if (d->id <= GPIO1_GROUP_0_MAX_ID)
    {
        hwp1->gpint_ctrl_r = (0 << lid);
        hwp1->gpint_ctrl_f = (0 << lid);
        hwp1->gpint_ctrl_mode = (0 << lid);
        hwp1->db_ctrl[d->id].db_ctrl0 = 0;

        if (cfg->mode == BOOT_GPIO_INPUT)
        {
            hwp1->gpio_oen_set_in = (1 << lid);
        }
        else
        {
            hwp1->gpio_oen_set_out = (1 << lid);

            if (cfg->out_level == true)
            {
                hwp1->gpio_set = (1 << lid);
            }
            else
            {
                hwp1->gpio_clr = (1 << lid);
            }
        }

        if (cfg->intr_enabled)
        {
            if (cfg->debounce)
            {
                hwp1->db_ctrl[d->id].db_ctrl0 = ((lid & 0x7) << 1);
            }
            if (cfg->intr_level)
            {
                hwp1->gpint_ctrl_mode = (1 << lid);
            }
            if (cfg->rising)
            {
                hwp1->gpint_ctrl_r = (1 << lid);
            }
            if (cfg->falling)
            {
                hwp1->gpint_ctrl_f = (1 << lid);
            }
        }
    }
    else if ((d->id > GPIO1_GROUP_0_MAX_ID) & (d->id <= GPIO2_GROUP_L_MAX_ID))
    {
        hwp2->gpint_r_clr_l = (1 << lid);
        hwp2->gpint_f_clr_l = (1 << lid);
        hwp2->gpint_mode_clr_l = (1 << lid);
        hwp2->gpint_dbn_en_clr_l = (1 << lid);

        if (cfg->mode == BOOT_GPIO_INPUT)
        {
            hwp2->gpio_oen_set_in_l = (1 << lid);
        }
        else
        {
            hwp2->gpio_oen_set_out_l = (1 << lid);

            if (cfg->out_level == true)
            {
                hwp2->gpio_set_l = (1 << lid);
            }
            else
            {
                hwp2->gpio_clr_l = (1 << lid);
            }
        }

        if (cfg->intr_enabled)
        {
            if (cfg->debounce)
            {
                hwp2->gpint_dbn_en_set_l = (1 << lid);
            }
            if (cfg->intr_level)
            {
                hwp2->gpint_mode_set_l = (1 << lid);
            }
            if (cfg->rising)
            {
                hwp2->gpint_r_set_l = (1 << lid);
            }
            if (cfg->falling)
            {
                hwp2->gpint_f_set_l = (1 << lid);
            }
        }
    }
    else
    {
        lid = lid - GPIO2_GROUP_L_MAX_ID - 1;
        hwp2->gpint_r_clr_h = (1 << lid);
        hwp2->gpint_f_clr_h = (1 << lid);
        hwp2->gpint_mode_clr_h = (1 << lid);
        hwp2->gpint_dbn_en_clr_h = (1 << lid);

        if (cfg->mode == BOOT_GPIO_INPUT)
        {
            hwp2->gpio_oen_set_in_h = (1 << lid);
        }
        else
        {
            hwp2->gpio_oen_set_out_h = (1 << lid);

            if (cfg->out_level == true)
            {
                hwp2->gpio_set_h = (1 << lid);
            }
            else
            {
                hwp2->gpio_clr_h = (1 << lid);
            }
        }

        if (cfg->intr_enabled)
        {
            if (cfg->debounce)
                hwp2->gpint_dbn_en_set_h = (1 << lid);
            if (cfg->intr_level)
                hwp2->gpint_mode_set_h = (1 << lid);
            if (cfg->rising)
                hwp2->gpint_r_set_h = (1 << lid);
            if (cfg->falling)
                hwp2->gpint_f_set_h = (1 << lid);
        }
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

    HWP_GPIO1_T *hwp1 = hwp_gpio1;
    HWP_GPIO_T *hwp2 = hwp_gpio2;
    uint32_t lid = d->id;

    if (d->id <= GPIO1_GROUP_0_MAX_ID)
    {
        hwp1->gpio_oen_set_in = (1 << lid);
        hwp1->gpint_ctrl_r = (0 << lid);
        hwp1->gpint_ctrl_f = (0 << lid);
        hwp1->gpint_ctrl_mode = (0 << lid);
        hwp1->db_ctrl[d->id].db_ctrl0 = (0 << lid);
    }
    else if ((d->id > GPIO1_GROUP_0_MAX_ID) & (d->id <= GPIO2_GROUP_L_MAX_ID))
    {
        hwp2->gpio_oen_set_in_l = (1 << lid);
        hwp2->gpint_r_clr_l = (1 << lid);
        hwp2->gpint_f_clr_l = (1 << lid);
        hwp2->gpint_mode_clr_l = (1 << lid);
        hwp2->gpint_dbn_en_clr_l = (1 << lid);
    }
    else
    {
        lid = lid - GPIO2_GROUP_L_MAX_ID - 1;
        hwp2->gpio_oen_set_in_h = (1 << lid);
        hwp2->gpint_r_clr_h = (1 << lid);
        hwp2->gpint_f_clr_h = (1 << lid);
        hwp2->gpint_mode_clr_h = (1 << lid);
        hwp2->gpint_dbn_en_clr_h = (1 << lid);
    }

    free(d);
    p->gpios[lid] = NULL;
    bootExitCritical(critical);
}

bool bootGpioRead(bootGpio_t *d)
{
    bool res = false;
    bootGpioContext_t *p = &gbootGpioCtx;
    uint32_t critical = bootEnterCritical();

    if (d == NULL || d->id >= GPIO_COUNT || d != p->gpios[d->id])
    {
        bootExitCritical(critical);
        return false;
    }

    HWP_GPIO1_T *hwp1 = hwp_gpio1;
    HWP_GPIO_T *hwp2 = hwp_gpio2;
    uint32_t lid = d->id;

    if (lid <= GPIO1_GROUP_0_MAX_ID)
    {
        res = hwp1->gpio_val & (1 << lid);
    }
    else if ((lid > GPIO1_GROUP_0_MAX_ID) & (lid <= GPIO2_GROUP_L_MAX_ID))
    {
        res = hwp2->gpio_val_l & (1 << lid);
    }
    else
    {
        lid = lid - GPIO2_GROUP_L_MAX_ID - 1;
        res = hwp2->gpio_val_h & (1 << lid);
    }
    bootExitCritical(critical);
    return res;
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

    HWP_GPIO1_T *hwp1 = hwp_gpio1;
    HWP_GPIO_T *hwp2 = hwp_gpio2;
    uint32_t lid = d->id;

    if (lid <= GPIO1_GROUP_0_MAX_ID)
    {
        if (level)
            hwp1->gpio_set = (1 << lid);
        else
            hwp1->gpio_clr = (1 << lid);
    }
    else if ((lid > GPIO1_GROUP_0_MAX_ID) & (lid <= GPIO2_GROUP_L_MAX_ID))
    {
        if (level)
            hwp2->gpio_set_l = (1 << lid);
        else
            hwp2->gpio_clr_l = (1 << lid);
    }
    else
    {
        lid = lid - GPIO2_GROUP_L_MAX_ID - 1;
        if (level)
            hwp2->gpio_set_h = (1 << lid);
        else
            hwp2->gpio_clr_h = (1 << lid);
    }
    bootExitCritical(critical);
}
