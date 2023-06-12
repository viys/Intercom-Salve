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

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

#include "hal_chip.h"
#include "hwregs.h"
#include "boot_lcd.h"
#include "boot_trace.h"

extern const lcdSpec_t g_lcd_gc9305;
extern const lcdSpec_t g_lcd_st7735s;
extern const lcdSpec_t g_lcd_st7789v;
extern const lcdSpec_t g_lcd_ili9163v;
extern const lcdSpec_t g_lcd_gc9306;

static uint8_t g_lcd_index = 0xff;

static lcdSpec_t *lcd_cfg_tab[] =
    {
        (lcdSpec_t *)&g_lcd_gc9305,
        (lcdSpec_t *)&g_lcd_st7735s,
        (lcdSpec_t *)&g_lcd_st7789v,
        (lcdSpec_t *)&g_lcd_ili9163v,
        (lcdSpec_t *)&g_lcd_gc9306,
};

static uint32_t _bootLcdGetCfgCount(void)
{
    uint32_t count;
    count = sizeof(lcd_cfg_tab) / sizeof(lcd_cfg_tab[0]);
    bootLog("lcd:  _bootLcdGetCfgCount count=%d", count);
    return count;
}

/*****************************************************************************/
//  Description:    This function is used to get LCD spec information
/*****************************************************************************/
lcdSpec_t *bootLcdGetSpecInfo(void)
{
    uint32_t count = 0;
    uint32_t i = 0;
    uint32_t lcm_dev_id = 0;
    bootLog("lcd:  bootLcdGetSpecInfo \n");
    count = _bootLcdGetCfgCount();
    if ((count != 0) && (g_lcd_index == 0xff))
    {
        for (i = 0; i < count; i++)
        {
            if (lcd_cfg_tab[i]->operation->readId)
            {
                lcm_dev_id = lcd_cfg_tab[i]->operation->readId();
                bootLog("lcd:  bootLcdGetSpecInfo id=0x%x \n", lcm_dev_id);
            }
            if (lcm_dev_id == lcd_cfg_tab[i]->dev_id)
            {
                g_lcd_index = i;
                bootLog("lcd:  bootLcdGetSpecInfo g_lcd_index =0x%x \n", g_lcd_index);
                return lcd_cfg_tab[g_lcd_index];
            }
        }
    }
    else if (g_lcd_index != 0xff)
    {
        return lcd_cfg_tab[g_lcd_index];
    }
    bootLog("lcd:  bootLcdGetSpecInfo null ! \n");
    return NULL;
}

#ifdef __cplusplus
}

#endif // End of lcm_cfg_info.c
