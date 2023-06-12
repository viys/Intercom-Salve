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

#include "hwregs.h"
#include "hal_gouda.h"
#include "hal_chip.h"
#include "boot_lcd.h"
#include "boot_platform.h"

#define LCD_DRV_ID_GC9306 0x009306
#define LCD_WIDTH 240
#define LCD_HEIGHT 320

static uint16_t gc9306_line_mode;
static bool gc9306_farmk;



#define LCM_WR_REG(Addr, Data)                             \
    {                                                      \
        while (halGoudaWriteReg(Addr, Data) != HAL_ERR_NO) \
            ;                                              \
    }
#define LCD_DataWrite_GC9306(Data)                    \
    {                                                 \
        while (halGoudaWriteData(Data) != HAL_ERR_NO) \
            ;                                         \
    }
#define LCD_CtrlWrite_GC9306(Cmd)                   \
    {                                               \
        while (halGoudaWriteCmd(Cmd) != HAL_ERR_NO) \
            ;                                       \
    }

static void _gc9306Close(void);
static uint32_t _gc9306ReadId(void);
static void _gc9306SleepIn(bool is_sleep);
static void _gc9306Init(void);
static void _gc9306Invalidate(void);
static void _gc9306InvalidateRect(
    uint16_t left,  //the left value of the rectangel
    uint16_t top,   //top of the rectangle
    uint16_t right, //right of the rectangle
    uint16_t bottom //bottom of the rectangle
);

static void _gc9306SetDisplayWindow(
    uint16_t left,  // start Horizon address
    uint16_t right, // end Horizon address
    uint16_t top,   // start Vertical address
    uint16_t bottom // end Vertical address
);
static void _lcdDelayMs(int ms_delay)
{
    bootDelayUS(ms_delay * 1000);
}

__UNUSED static void _gc9306SetDirection(lcdDirect_t direct_type)
{

    bootLog("lcd:    _gc9306SetDirection = %d", direct_type);

    switch (direct_type)
    {
    case LCD_DIRECT_NORMAL:
        LCD_CtrlWrite_GC9306(0x36);
        LCD_DataWrite_GC9306(0x00);
        break;
    case LCD_DIRECT_ROT_90:
        LCD_CtrlWrite_GC9306(0x36);
        LCD_DataWrite_GC9306(0x60);
        break;
    }

    LCD_CtrlWrite_GC9306(0x2c);
}

/******************************************************************************/
//  Description:   Set the windows address to display, in this windows
//                 color is  refreshed.
/******************************************************************************/
static void _gc9306SetDisplayWindow(
    uint16_t left,  // start Horizon address
    uint16_t top,   // start Vertical address
    uint16_t right, // end Horizon address
    uint16_t bottom // end Vertical address
)
{
    uint16_t newleft = left;
    uint16_t newtop = top;
    uint16_t newright = right;
    uint16_t newbottom = bottom;
    bootLog("lcd:    _gc9306SetDisplayWindow L = %d, top = %d, R = %d, bottom = %d", left, top, right, bottom);

    LCD_CtrlWrite_GC9306(0x2a);                   // set hori start , end (left, right)
    LCD_DataWrite_GC9306((newleft >> 8) & 0xff);  // left high 8 b
    LCD_DataWrite_GC9306(newleft & 0xff);         // left low 8 b
    LCD_DataWrite_GC9306((newright >> 8) & 0xff); // right high 8 b
    LCD_DataWrite_GC9306(newright & 0xff);        // right low 8 b

    LCD_CtrlWrite_GC9306(0x2b);                    // set vert start , end (top, bot)
    LCD_DataWrite_GC9306((newtop >> 8) & 0xff);    // top high 8 b
    LCD_DataWrite_GC9306(newtop & 0xff);           // top low 8 b
    LCD_DataWrite_GC9306((newbottom >> 8) & 0xff); // bot high 8 b
    LCD_DataWrite_GC9306(newbottom & 0xff);        // bot low 8 b
    LCD_CtrlWrite_GC9306(0x2c);                    // recover memory write mode
}

/**************************************************************************************/
// Description: initialize all LCD with LCDC MCU MODE and LCDC mcu mode
/**************************************************************************************/
static void _gc9306Init(void)
{
    bootLog("lcd:    gc9306Init ");

    LCD_CtrlWrite_GC9306(0xfe);
    LCD_CtrlWrite_GC9306(0xef);

    LCD_CtrlWrite_GC9306(0x36);
    LCD_DataWrite_GC9306(0x48);

    LCD_CtrlWrite_GC9306(0x3a);
    LCD_DataWrite_GC9306(0x05);

    LCD_CtrlWrite_GC9306(0xa4);
    LCD_DataWrite_GC9306(0x44);
    LCD_DataWrite_GC9306(0x44);

    LCD_CtrlWrite_GC9306(0xa5);
    LCD_DataWrite_GC9306(0x42);
    LCD_DataWrite_GC9306(0x42);

    LCD_CtrlWrite_GC9306(0xaa);
    LCD_DataWrite_GC9306(0x88);
    LCD_DataWrite_GC9306(0x88);

    if (gc9306_farmk == true)
    {
        // set output te at line 1
        LCD_CtrlWrite_GC9306(0x44);
        LCD_DataWrite_GC9306(0x0); //  1
        LCD_DataWrite_GC9306(0x1); //  3f

        LCD_CtrlWrite_GC9306(0x35);
        LCD_DataWrite_GC9306(0x0); //  vsync

        // blanking porch control
        //  LCD_CtrlWrite_GC9306(0xb5);
        //  LCD_DataWrite_GC9306(0x00);
        //  LCD_DataWrite_GC9306(0x00);
        //  LCD_DataWrite_GC9306(0x00);
        //  LCD_DataWrite_GC9306(0x14);

        // set frame rate
        LCD_CtrlWrite_GC9306(0xe8);
        LCD_DataWrite_GC9306(0x08); //4    // 4=41hz  7=30hz 8=28hz
        LCD_DataWrite_GC9306(0x40);
    }
    else
    {
        LCD_CtrlWrite_GC9306(0xe8);
        LCD_DataWrite_GC9306(0x11);
        LCD_DataWrite_GC9306(0x0b); //0x0b
    }

    LCD_CtrlWrite_GC9306(0xe3);
    LCD_DataWrite_GC9306(0x01);
    LCD_DataWrite_GC9306(0x10);

    LCD_CtrlWrite_GC9306(0xff);
    LCD_DataWrite_GC9306(0x61);

    LCD_CtrlWrite_GC9306(0xac);
    LCD_DataWrite_GC9306(0x00);

    LCD_CtrlWrite_GC9306(0xaf);
    LCD_DataWrite_GC9306(0x67);

    LCD_CtrlWrite_GC9306(0xa6);
    LCD_DataWrite_GC9306(0x16);
    LCD_DataWrite_GC9306(0x16);

    LCD_CtrlWrite_GC9306(0xa7);
    LCD_DataWrite_GC9306(0x08);
    LCD_DataWrite_GC9306(0x08);

    LCD_CtrlWrite_GC9306(0xa8);
    LCD_DataWrite_GC9306(0x27);
    LCD_DataWrite_GC9306(0x27);

    LCD_CtrlWrite_GC9306(0xa9);
    LCD_DataWrite_GC9306(0x08);
    LCD_DataWrite_GC9306(0x08);

    /*LCD_CtrlWrite_GC9306(0x2a);
    LCD_DataWrite_GC9306(0x00);
    LCD_DataWrite_GC9306(0x00);
    LCD_DataWrite_GC9306(0x00);
    LCD_DataWrite_GC9306(0xef);

    LCD_CtrlWrite_GC9306(0x2b);
    LCD_DataWrite_GC9306(0x00);
    LCD_DataWrite_GC9306(0x00);
    LCD_DataWrite_GC9306(0x01);
    LCD_DataWrite_GC9306(0x3f);
    LCD_CtrlWrite_GC9306(0x2c);*/

    LCD_CtrlWrite_GC9306(0xf0);
    LCD_DataWrite_GC9306(0x02);
    LCD_DataWrite_GC9306(0x01);
    LCD_DataWrite_GC9306(0x00);
    LCD_DataWrite_GC9306(0x08);
    LCD_DataWrite_GC9306(0x0d);
    LCD_DataWrite_GC9306(0x14);

    LCD_CtrlWrite_GC9306(0xf1);
    LCD_DataWrite_GC9306(0x01);
    LCD_DataWrite_GC9306(0x1);
    LCD_DataWrite_GC9306(0x00);
    LCD_DataWrite_GC9306(0xa);
    LCD_DataWrite_GC9306(0x15);
    LCD_DataWrite_GC9306(0xd);

    LCD_CtrlWrite_GC9306(0xf2);
    LCD_DataWrite_GC9306(0x14);
    LCD_DataWrite_GC9306(0xb);
    LCD_DataWrite_GC9306(0x42);
    LCD_DataWrite_GC9306(0x04);
    LCD_DataWrite_GC9306(0x04);
    LCD_DataWrite_GC9306(0x53);

    LCD_CtrlWrite_GC9306(0xf3);
    LCD_DataWrite_GC9306(0x10);
    LCD_DataWrite_GC9306(0xb);
    LCD_DataWrite_GC9306(0x3b);
    LCD_DataWrite_GC9306(0x4);
    LCD_DataWrite_GC9306(0x3);
    LCD_DataWrite_GC9306(0x4a);

    LCD_CtrlWrite_GC9306(0xf4);
    LCD_DataWrite_GC9306(0xd);
    LCD_DataWrite_GC9306(0x19);
    LCD_DataWrite_GC9306(0x19);
    LCD_DataWrite_GC9306(0x27);
    LCD_DataWrite_GC9306(0x28);
    LCD_DataWrite_GC9306(0x0F);

    LCD_CtrlWrite_GC9306(0xf5);
    LCD_DataWrite_GC9306(0x5);
    LCD_DataWrite_GC9306(0x14);
    LCD_DataWrite_GC9306(0x12);
    LCD_DataWrite_GC9306(0x16);
    LCD_DataWrite_GC9306(0x18);
    LCD_DataWrite_GC9306(0x0F);

    LCD_CtrlWrite_GC9306(0x11);
    _lcdDelayMs(120);
    LCD_CtrlWrite_GC9306(0x29);
    _lcdDelayMs(20);
    LCD_CtrlWrite_GC9306(0x2c);

    if (gc9306_line_mode == HAL_GOUDA_SPI_LINE_3_2_LANE)
    {
        //open two lane mode
        LCD_CtrlWrite_GC9306(0xe9);
        LCD_DataWrite_GC9306(0x08);
    }
}

static void _gc9306SleepIn(bool is_sleep)
{
    bootLog("lcd:  gc9306  _gc9306SleepIn, is_sleep = %d", is_sleep);

    if (is_sleep)
    {
        LCD_CtrlWrite_GC9306(0x28); //display off
        _lcdDelayMs(120);
        LCD_CtrlWrite_GC9306(0x10); // enter sleep mode
    }
    else
    {

#if 0
        LCD_CtrlWrite_GC9306(0x11); // sleep out mode
        _lcdDelayMs(120);
        LCD_CtrlWrite_GC9306(0x29); // display on
#endif
        _gc9306Init();
    }
}

static void _gc9306Close(void)
{
    bootLog("lcd:  gc9306   in GC9304_Close");

    _gc9306SleepIn(true);
}
static void _gc9306Invalidate(void)
{
    bootLog("lcd:  gc9306   in _gc9306Invalidate");

    _gc9306SetDisplayWindow(0x0, 0x0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
}

void _gc9306InvalidateRect(
    uint16_t left,  //the left value of the rectangel
    uint16_t top,   //top of the rectangle
    uint16_t right, //right of the rectangle
    uint16_t bottom //bottom of the rectangle
)
{
    bootLog("lcd: gc9306 _gc9306InvalidateRect");
    left = (left >= LCD_WIDTH) ? LCD_WIDTH - 1 : left;
    right = (right >= LCD_WIDTH) ? LCD_WIDTH - 1 : right;
    top = (top >= LCD_HEIGHT) ? LCD_HEIGHT - 1 : top;
    bottom = (bottom >= LCD_HEIGHT) ? LCD_HEIGHT - 1 : bottom;

    if ((right < left) || (bottom < top))
    {

        BOOT_ASSERT(false, "lcd: gc9306 _gc9306InvalidateRect error"); /*assert verified*/
    }

    _gc9306SetDisplayWindow(left, top, right, bottom);
}

static void _gc9306RotationInvalidateRect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, lcdAngle_t angle)
{

    bootLog("lcd: gc9306 gc9306RotationInvalidateRect");
    switch (angle)
    {
    case LCD_ANGLE_0:
        _gc9306SetDisplayWindow(left, top, right, bottom);
        break;
    case LCD_ANGLE_90:
        _gc9306SetDisplayWindow(left, top, bottom, right);
        break;
    case LCD_ANGLE_180:
        _gc9306SetDisplayWindow(left, top, right, bottom);
        break;
    case LCD_ANGLE_270:
        _gc9306SetDisplayWindow(left, top, bottom, right);
        break;
    default:
        _gc9306SetDisplayWindow(left, top, right, bottom);
        break;
    }
}

static void _gc9306EsdCheck(void)
{

    bootLog("lcd:  gc9306  gc9306EsdCheck");
}

static const lcdOperations_t gc9306Operations =
    {
        _gc9306Init,
        _gc9306SleepIn,
        _gc9306EsdCheck,
        _gc9306SetDisplayWindow,
        _gc9306InvalidateRect,
        _gc9306Invalidate,
        _gc9306Close,
        _gc9306RotationInvalidateRect,
        NULL,
        _gc9306ReadId,
};

const lcdSpec_t g_lcd_gc9306 =
    {
        LCD_DRV_ID_GC9306,
        LCD_WIDTH,
        LCD_HEIGHT,
        HAL_GOUDA_SPI_LINE_4,
        LCD_CTRL_SPI,
        (lcdOperations_t *)&gc9306Operations,
        false,
        0x2a000,
        50000000,
};

static uint32_t _gc9306ReadId(void)
{
    uint32_t ret_id = 0;
    HAL_ERR_T r_err;
    uint8_t id[4] = {0};
    bootLog("lcd:  gc9306   _gc9306ReadId \n");
    halPmuSetPowerLevel(HAL_POWER_LCD,SENSOR_VDD_1800MV);
    halPmuSwitchPower(HAL_POWER_LCD, true, true);
    _lcdDelayMs(10);
    gc9306_farmk = g_lcd_gc9306.is_use_fmark;
    gc9306_line_mode = g_lcd_gc9306.bus_mode;

    halGoudaReadConfig(gc9306_line_mode, g_lcd_gc9306.is_use_fmark, g_lcd_gc9306.fmark_delay);
    _lcdDelayMs(140);
    //hwp_analogReg->pad_spi_lcd_cfg2 |= ANALOG_REG_PAD_SPI_LCD_SIO_SPU;
    r_err = halGoudaReadData(0x04, id, 4);
    //hwp_analogReg->pad_spi_lcd_cfg2 =  hwp_analogReg->pad_spi_lcd_cfg2&(~ANALOG_REG_PAD_SPI_LCD_SIO_SPU);
    _lcdDelayMs(10);
    bootLog("lcd:  gc9306  _gc9306ReadId LCM: 0x%0x, 0x%x, 0x%0x (read return : %d)\n", id[2], id[1], id[3], r_err);

    ret_id = ((id[3] << 16) | (id[2] << 8) | id[1]);

    if (LCD_DRV_ID_GC9306 == ret_id)
    {
        bootLog("lcd:  gc9306 is using !");
    }

    halGoudaClose();
    halPmuSwitchPower(HAL_POWER_LCD, false, false);
    return LCD_DRV_ID_GC9306;
}
