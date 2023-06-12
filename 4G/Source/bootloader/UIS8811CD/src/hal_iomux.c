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

#include "boot_trace.h"
#include "hal_iomux.h"
#include "hwregs.h"
#include <stdlib.h>

#define HWP_PM hwp_iomux2
#define IOMUX_REG_COUNT (sizeof(HWP_IOMUX2_T) / 4)

#define FUN_INDEX(fun_pad) ((fun_pad)&0xfff)
#define PAD_INDEX(fun_pad) (((fun_pad) >> 12) & 0xfff)
#define SEL_INDEX(fun_pad) (((fun_pad) >> 24) & 0xf)
#define FUN_INDEX_VALID(fun) ((unsigned)(fun - 1) < HAL_IOMUX_FUN_COUNT)
#define PAD_INDEX_VALID(pad) ((unsigned)(pad - 1) < HAL_IOMUX_PAD_COUNT)

#define FUN_INDEX_VAL(fun) ((fun)&0xfff)
#define PAD_INDEX_VAL(pad) (((pad)&0xfff) << 12)

#define REG_IOMUX_PAD_IO_CFG_T REG_IOMUX1_PAD_GPIO_0_CFG_T
#define pad_io_sel pad_gpio_0_sel
#define pad_io_oen_frc pad_gpio_0_oen_frc
#define pad_io_oen_reg pad_gpio_0_oen_reg
#define pad_io_out_frc pad_gpio_0_out_frc
#define pad_io_out_reg pad_gpio_0_out_reg
#define pad_io_pull_frc pad_gpio_0_pull_frc
#define pad_io_pull_dn pad_gpio_0_pull_dn
#define pad_io_pull_up pad_gpio_0_pull_up
#define pad_io_drv_strength pad_gpio_0_drv_strength

typedef struct
{
    volatile uint32_t *reg;
} halIomuxPadProp_t;

typedef struct
{
    uint16_t default_pad_index : 12;
    uint8_t sel : 4;
    uint8_t inout : 4;
    uint8_t pull : 4;
    uint8_t driving : 4;
} halIomuxFunProp_t;

typedef struct
{
    uint16_t pad_index;
    uint8_t sel : 4;
    uint8_t inout : 4;
    uint8_t pull : 4;
    uint8_t driving : 4;
} halIomuxPadInit_t;

typedef struct
{
    uint32_t suspend_regs[IOMUX_REG_COUNT];
} halIomuxContext_t;

#include "hal_iomux_prop.h"

static bool prvSetPadDriving(const halIomuxPadProp_t *pad_prop, uint8_t driving)
{
    if (driving > 3)
        return false;

    REG_IOMUX_PAD_IO_CFG_T cfg = {*pad_prop->reg};
    cfg.b.pad_io_drv_strength = driving;
    *pad_prop->reg = cfg.v;
    return true;
}

static bool prvSetPadPull(const halIomuxPadProp_t *pad_prop, uint8_t pull)
{
    if (pull != HAL_IOMUX_PULL_DEFAULT &&
        pull != HAL_IOMUX_FORCE_PULL_NONE &&
        pull != HAL_IOMUX_FORCE_PULL_DOWN &&
        pull != HAL_IOMUX_FORCE_PULL_UP_3 &&
        pull != HAL_IOMUX_FORCE_PULL_UP_2 &&
        pull != HAL_IOMUX_FORCE_PULL_UP_1)
        return false;

    REG_IOMUX_PAD_IO_CFG_T cfg = {*pad_prop->reg};
    switch (pull)
    {
    case HAL_IOMUX_PULL_DEFAULT:
    default:
        cfg.b.pad_io_pull_frc = 0;
        cfg.b.pad_io_pull_dn = 0;
        cfg.b.pad_io_pull_up = 0;
        break;

    case HAL_IOMUX_FORCE_PULL_NONE:
        cfg.b.pad_io_pull_frc = 1;
        cfg.b.pad_io_pull_dn = 0;
        cfg.b.pad_io_pull_up = 0;
        break;

    case HAL_IOMUX_FORCE_PULL_DOWN:
        cfg.b.pad_io_pull_frc = 1;
        cfg.b.pad_io_pull_dn = 1;
        cfg.b.pad_io_pull_up = 0;
        break;

    case HAL_IOMUX_FORCE_PULL_UP_1:
    case HAL_IOMUX_FORCE_PULL_UP_2:
    case HAL_IOMUX_FORCE_PULL_UP_3:
        cfg.b.pad_io_pull_frc = 1;
        cfg.b.pad_io_pull_dn = 0;
        cfg.b.pad_io_pull_up = (pull - HAL_IOMUX_FORCE_PULL_UP_1) + 1;
        break;
    }

    *pad_prop->reg = cfg.v;
    return true;
}

static bool prvSetPadInout(const halIomuxPadProp_t *pad_prop, uint8_t inout)
{
    if (inout != HAL_IOMUX_INOUT_DEFAULT &&
        inout != HAL_IOMUX_FORCE_INPUT &&
        inout != HAL_IOMUX_FORCE_OUTPUT &&
        inout != HAL_IOMUX_FORCE_OUTPUT_HI &&
        inout != HAL_IOMUX_FORCE_OUTPUT_LO)
        return false;

    REG_IOMUX_PAD_IO_CFG_T cfg = {*pad_prop->reg};
    switch (inout)
    {
    case HAL_IOMUX_INOUT_DEFAULT:
        cfg.b.pad_io_oen_frc = 0;
        cfg.b.pad_io_oen_reg = 0;
        cfg.b.pad_io_out_frc = 0;
        cfg.b.pad_io_out_reg = 0;
        break;

    case HAL_IOMUX_FORCE_INPUT:
        cfg.b.pad_io_oen_frc = 1;
        cfg.b.pad_io_oen_reg = 1;
        cfg.b.pad_io_out_frc = 0;
        cfg.b.pad_io_out_reg = 0;
        break;

    case HAL_IOMUX_FORCE_OUTPUT:
        cfg.b.pad_io_oen_frc = 1;
        cfg.b.pad_io_oen_reg = 0;
        cfg.b.pad_io_out_frc = 0;
        cfg.b.pad_io_out_reg = 0;
        break;

    case HAL_IOMUX_FORCE_OUTPUT_HI:
        cfg.b.pad_io_oen_frc = 1;
        cfg.b.pad_io_oen_reg = 0;
        cfg.b.pad_io_out_frc = 1;
        cfg.b.pad_io_out_reg = 1;
        break;

    case HAL_IOMUX_FORCE_OUTPUT_LO:
        cfg.b.pad_io_oen_frc = 1;
        cfg.b.pad_io_oen_reg = 0;
        cfg.b.pad_io_out_frc = 1;
        cfg.b.pad_io_out_reg = 0;
        break;
    }

    *pad_prop->reg = cfg.v;
    return true;
}

/**
 * Set pad configuration, change to closest property if not supported
 */
static void prvSetPad(const halIomuxPadProp_t *pad_prop, uint8_t sel, uint8_t inout, uint8_t pull, uint8_t driving)
{
    if (pad_prop->reg == NULL)
        return;

    REG_IOMUX_PAD_IO_CFG_T cfg = {*pad_prop->reg};
    cfg.b.pad_io_sel = sel;
    *pad_prop->reg = cfg.v;

    prvSetPadInout(pad_prop, inout);
    prvSetPadPull(pad_prop, pull);
    prvSetPadDriving(pad_prop, driving);
}

/**
 * Get pad selection, -1 on no register for the pad
 */
static int prvGetPadSel(const halIomuxPadProp_t *pad_prop)
{
    if (pad_prop->reg == NULL)
        return -1;

    REG_IOMUX_PAD_IO_CFG_T cfg = {*pad_prop->reg};
    return cfg.b.pad_io_sel;
}

bool halIomuxSetInitConfig(void)
{
    for (unsigned n = 0; n < ARRAY_SIZE(gInitPad); n++)
    {
        const halIomuxPadInit_t *pad_init = &gInitPad[n];
        unsigned pad_index = pad_init->pad_index;
        if (!PAD_INDEX_VALID(pad_index))
        {
            bootLog("iomux init invalid pad/0x%x", PAD_INDEX_VAL(pad_index));
            return false;
        }

        const halIomuxPadProp_t *pad_prop = &gPadProp[pad_index - 1];
        prvSetPad(pad_prop, pad_init->sel, pad_init->inout, pad_init->pull, pad_init->driving);
    }
    return true;
}

bool halIomuxSetFunction(unsigned fun_pad)
{
    unsigned fun_index = FUN_INDEX(fun_pad);
    unsigned pad_index = PAD_INDEX(fun_pad);
    unsigned sel = SEL_INDEX(fun_pad);
    if (!FUN_INDEX_VALID(fun_index))
        goto failed;

    const halIomuxFunProp_t *fun_prop = &gFunProp[fun_index - 1];
    if (!PAD_INDEX_VALID(pad_index))
    {
        pad_index = fun_prop->default_pad_index;
        sel = fun_prop->sel;
    }

    if (!PAD_INDEX_VALID(pad_index))
        goto failed;

    const halIomuxPadProp_t *pad_prop = &gPadProp[pad_index - 1];
    prvSetPad(pad_prop, sel, fun_prop->inout, fun_prop->pull, fun_prop->driving);
    return true;

failed:
    bootLog("iomux failed to set function 0x%x", fun_pad);
    return false;
}

bool halIomuxSetPadDriving(unsigned fun_pad, unsigned driving)
{
    unsigned pad_index = PAD_INDEX(fun_pad);
    if (!PAD_INDEX_VALID(pad_index))
    {
        unsigned fun_index = FUN_INDEX(fun_pad);
        if (!FUN_INDEX_VALID(fun_index))
            goto failed;

        const halIomuxFunProp_t *fun_prop = &gFunProp[fun_index - 1];
        pad_index = fun_prop->default_pad_index;
    }

    if (!PAD_INDEX_VALID(pad_index))
        goto failed;

    const halIomuxPadProp_t *pad_prop = &gPadProp[pad_index - 1];
    if (!prvSetPadDriving(pad_prop, driving))
        goto failed;

    return true;

failed:
    bootLog("iomux failed to set driving 0x%x %d", fun_pad, driving);
    return false;
}

bool halIomuxSetPadPull(unsigned fun_pad, halIomuxPullType_t pull)
{
    unsigned pad_index = PAD_INDEX(fun_pad);
    if (!PAD_INDEX_VALID(pad_index))
    {
        unsigned fun_index = FUN_INDEX(fun_pad);
        if (!FUN_INDEX_VALID(fun_index))
            goto failed;

        const halIomuxFunProp_t *fun_prop = &gFunProp[fun_index - 1];
        pad_index = fun_prop->default_pad_index;
    }

    if (!PAD_INDEX_VALID(pad_index))
        goto failed;

    const halIomuxPadProp_t *pad_prop = &gPadProp[pad_index - 1];
    if (!prvSetPadPull(pad_prop, pull))
        goto failed;

    return true;

failed:
    bootLog("iomux failed to set pull 0x%x %d", fun_pad, pull);
    return false;
}

bool halIomuxSetPadInout(unsigned fun_pad, halIomuxInoutType_t inout)
{
    unsigned pad_index = PAD_INDEX(fun_pad);
    if (!PAD_INDEX_VALID(pad_index))
    {
        unsigned fun_index = FUN_INDEX(fun_pad);
        if (!FUN_INDEX_VALID(fun_index))
            goto failed;

        const halIomuxFunProp_t *fun_prop = &gFunProp[fun_index - 1];
        pad_index = fun_prop->default_pad_index;
    }

    if (!PAD_INDEX_VALID(pad_index))
        goto failed;

    const halIomuxPadProp_t *pad_prop = &gPadProp[pad_index - 1];
    if (!prvSetPadInout(pad_prop, inout))
        goto failed;

    return true;

failed:
    bootLog("iomux failed to set inout 0x%x %d", fun_pad, inout);
    return false;
}

unsigned halIomuxGetFunPads(unsigned fun, unsigned *pads)
{
    unsigned count = 0;
    unsigned fun_index = FUN_INDEX(fun);
    if (!FUN_INDEX_VALID(fun_index))
        return count;

    for (unsigned n = 0; n < ARRAY_SIZE(gFunPadSel); n++)
    {
        unsigned nfun_index = FUN_INDEX(gFunPadSel[n]);
        unsigned npad_index = PAD_INDEX(gFunPadSel[n]);
        unsigned nsel = SEL_INDEX(gFunPadSel[n]);
        if (nfun_index != fun_index)
            continue;

        const halIomuxPadProp_t *pad_prop = &gPadProp[npad_index - 1];
        int psel = prvGetPadSel(pad_prop);
        if (nsel == psel || psel < 0) // no register means force select
        {
            count++;
            if (pads != NULL)
                *pads++ = gFunPadSel[n];
        }
    }
    return count;
}

unsigned halIomuxGetPadFun(unsigned pad)
{
    unsigned pad_index = PAD_INDEX(pad);
    if (!PAD_INDEX_VALID(pad_index))
        return 0;

    for (unsigned n = 0; n < ARRAY_SIZE(gFunPadSel); n++)
    {
        unsigned npad_index = PAD_INDEX(gFunPadSel[n]);
        unsigned nsel = SEL_INDEX(gFunPadSel[n]);
        if (npad_index != pad_index)
            continue;

        const halIomuxPadProp_t *pad_prop = &gPadProp[npad_index - 1];
        int psel = prvGetPadSel(pad_prop);
        if (nsel == psel || psel < 0) // no register means force select
            return gFunPadSel[n];
    }
    return 0;
}

const char *halIomuxFunName(unsigned fun)
{
    unsigned fun_index = FUN_INDEX(fun);
    if (!FUN_INDEX_VALID(fun_index))
        return "";
    return gFunNames[fun_index - 1];
}

const char *halIomuxPadName(unsigned pad)
{
    unsigned pad_index = PAD_INDEX(pad);
    if (!PAD_INDEX_VALID(pad_index))
        return "";
    return gPadNames[pad_index - 1];
}

void halIomuxShowInfo(void)
{
    for (unsigned fun = 1; fun <= HAL_IOMUX_FUN_COUNT; fun++)
    {
        unsigned count = halIomuxGetFunPads(fun, NULL);
        bootLog("IOMUX FUN/0x%x/%s PAD count/%d",fun, halIomuxFunName(fun), count);
    }
    for (int pad = PAD_INDEX_VAL(1); pad <= PAD_INDEX_VAL(HAL_IOMUX_PAD_COUNT); pad += PAD_INDEX_VAL(1))
    {
        unsigned fun = halIomuxGetPadFun(pad);
        bootLog("IOMUX PAD/0x%x/%s FUN/0x%x/%s ",pad, halIomuxPadName(pad), fun, halIomuxFunName(fun));
    }
}

void halIomuxInit(void)
{
    halIomuxSetInitConfig();
}
