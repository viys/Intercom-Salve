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


#define LCD_DRV_ID_ST7789 0X858552
#define LCD_WIDTH 240
#define LCD_HEIGHT 320

static uint16_t st7789_line_mode;
static bool st7789_farmk;



#define LCM_WR_REG(Addr, Data)                             \
    {                                                      \
        while (halGoudaWriteReg(Addr, Data) != HAL_ERR_NO) \
            ;                                              \
    }
#define LCD_DataWrite_ST7789(Data)                    \
    {                                                 \
        while (halGoudaWriteData(Data) != HAL_ERR_NO) \
            ;                                         \
    }
#define LCD_CtrlWrite_ST7789(Cmd)                   \
    {                                               \
        while (halGoudaWriteCmd(Cmd) != HAL_ERR_NO) \
            ;                                       \
    }

static void _st7789Close(void);
static uint32_t _st7789ReadId(void);
static void _st7789SleepIn(bool is_sleep);
static void _st7789Init(void);
static void _st7789Invalidate(void);
static void _st7789InvalidateRect(
    uint16_t left,  //the left value of the rectangel
    uint16_t top,   //top of the rectangle
    uint16_t right, //right of the rectangle
    uint16_t bottom //bottom of the rectangle
);

static void _st7789SetDisplayWindow(
    uint16_t left,  // start Horizon address
    uint16_t right, // end Horizon address
    uint16_t top,   // start Vertical address
    uint16_t bottom // end Vertical address
);
static void _lcdDelayMs(int ms_delay)
{
    bootDelayUS(ms_delay * 1000);
}

__UNUSED static void _st7789SetDirection(lcdDirect_t direct_type)
{

    bootLog("lcd:    _st7789SetDirection = %d", direct_type);

    switch (direct_type)
    {
    case LCD_DIRECT_NORMAL:
        LCD_CtrlWrite_ST7789(0x36);
        LCD_DataWrite_ST7789(0x00);
        break;
    case LCD_DIRECT_ROT_90:
        LCD_CtrlWrite_ST7789(0x36);
        LCD_DataWrite_ST7789(0x60);
        break;
    }

    LCD_CtrlWrite_ST7789(0x2c);
}

/******************************************************************************/
//  Description:   Set the windows address to display, in this windows
//                 color is  refreshed.
/******************************************************************************/
static void _st7789SetDisplayWindow(
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
    bootLog("lcd:    _st7789SetDisplayWindow L = %d, top = %d, R = %d, bottom = %d", left, top, right, bottom);

    LCD_CtrlWrite_ST7789(0x2a);                   // set hori start , end (left, right)
    LCD_DataWrite_ST7789((newleft >> 8) & 0xff);  // left high 8 b
    LCD_DataWrite_ST7789(newleft & 0xff);         // left low 8 b
    LCD_DataWrite_ST7789((newright >> 8) & 0xff); // right high 8 b
    LCD_DataWrite_ST7789(newright & 0xff);        // right low 8 b

    LCD_CtrlWrite_ST7789(0x2b);                    // set vert start , end (top, bot)
    LCD_DataWrite_ST7789((newtop >> 8) & 0xff);    // top high 8 b
    LCD_DataWrite_ST7789(newtop & 0xff);           // top low 8 b
    LCD_DataWrite_ST7789((newbottom >> 8) & 0xff); // bot high 8 b
    LCD_DataWrite_ST7789(newbottom & 0xff);        // bot low 8 b
    LCD_CtrlWrite_ST7789(0x2c);                    // recover memory write mode
}

/**************************************************************************************/
// Description: initialize all LCD with LCDC MCU MODE and LCDC mcu mode
/**************************************************************************************/
static void _st7789Init(void)
{
    bootLog("lcd:    st7789Init ");
#if 1

    LCD_CtrlWrite_ST7789(0x11);
    _lcdDelayMs(120);

    LCD_CtrlWrite_ST7789(0x36);
    LCD_DataWrite_ST7789(0x00);

    LCD_CtrlWrite_ST7789(0xb2);
    LCD_DataWrite_ST7789(0x0c);
    LCD_DataWrite_ST7789(0x0c);
    LCD_DataWrite_ST7789(0x00);
    LCD_DataWrite_ST7789(0x33);
    LCD_DataWrite_ST7789(0x33);

    LCD_CtrlWrite_ST7789(0xb7);
    LCD_DataWrite_ST7789(0x35);

    LCD_CtrlWrite_ST7789(0xbb);
    LCD_DataWrite_ST7789(0x37);
      
    LCD_CtrlWrite_ST7789(0xc0);
    LCD_DataWrite_ST7789(0x2c);

      
    LCD_CtrlWrite_ST7789(0xc2);
    LCD_DataWrite_ST7789(0x01);
 
    LCD_CtrlWrite_ST7789(0xc3);
    LCD_DataWrite_ST7789(0x09);

    LCD_CtrlWrite_ST7789(0xc4);
    LCD_DataWrite_ST7789(0x20);

    LCD_CtrlWrite_ST7789(0xc6);
    LCD_DataWrite_ST7789(0x03);

    LCD_CtrlWrite_ST7789(0xd0);
    LCD_DataWrite_ST7789(0xa4);
    LCD_DataWrite_ST7789(0xa1);
    
    LCD_CtrlWrite_ST7789(0xb0);
    LCD_DataWrite_ST7789(0x00);
    LCD_DataWrite_ST7789(0xd0);
    
    LCD_CtrlWrite_ST7789(0xe0);
    LCD_DataWrite_ST7789(0xd0);
    LCD_DataWrite_ST7789(0x00);
    LCD_DataWrite_ST7789(0x05);
    LCD_DataWrite_ST7789(0x0e);
    LCD_DataWrite_ST7789(0x15);
    LCD_DataWrite_ST7789(0x0d);
    LCD_DataWrite_ST7789(0x37);
    LCD_DataWrite_ST7789(0x43);
    LCD_DataWrite_ST7789(0x47);
    LCD_DataWrite_ST7789(0x09);
    LCD_DataWrite_ST7789(0x15);
    LCD_DataWrite_ST7789(0x12);
    LCD_DataWrite_ST7789(0x16);
    LCD_DataWrite_ST7789(0x19);

    LCD_CtrlWrite_ST7789(0xe1);
    LCD_DataWrite_ST7789(0xd0);
    LCD_DataWrite_ST7789(0x00);
    LCD_DataWrite_ST7789(0x05);
    LCD_DataWrite_ST7789(0x0d);
    LCD_DataWrite_ST7789(0x0c);
    LCD_DataWrite_ST7789(0x06);
    LCD_DataWrite_ST7789(0x2d);
    LCD_DataWrite_ST7789(0x44);
    LCD_DataWrite_ST7789(0x40);
    LCD_DataWrite_ST7789(0x0e);
    LCD_DataWrite_ST7789(0x1c);
    LCD_DataWrite_ST7789(0x18);
    LCD_DataWrite_ST7789(0x16);
    LCD_DataWrite_ST7789(0x19);

    LCD_CtrlWrite_ST7789(0x35);
    LCD_DataWrite_ST7789(0x00);

    LCD_CtrlWrite_ST7789(0x3A);
    LCD_DataWrite_ST7789(0x55);

    _lcdDelayMs(20);
    
    LCD_CtrlWrite_ST7789(0x29);
#else
    
    LCD_CtrlWrite_ST7789(0x11);
    _lcdDelayMs(0x80);

    LCD_CtrlWrite_ST7789(0x36);
    LCD_DataWrite_ST7789(0x00);

    LCD_CtrlWrite_ST7789(0xb2);
    LCD_DataWrite_ST7789(0x0c);
    LCD_DataWrite_ST7789(0x0c);
    LCD_DataWrite_ST7789(0x00);
    LCD_DataWrite_ST7789(0x33);
    LCD_DataWrite_ST7789(0x33);

    LCD_CtrlWrite_ST7789(0xb7);
    LCD_DataWrite_ST7789(0x35);

    LCD_CtrlWrite_ST7789(0xbb);
    LCD_DataWrite_ST7789(0x14);

    LCD_CtrlWrite_ST7789(0xc0);
    LCD_DataWrite_ST7789(0x2c);

    LCD_CtrlWrite_ST7789(0xc2);
    LCD_DataWrite_ST7789(0x01);

    LCD_CtrlWrite_ST7789(0xc3);
    LCD_DataWrite_ST7789(0x1A);

    LCD_CtrlWrite_ST7789(0xc4);
    LCD_DataWrite_ST7789(0x20);

    LCD_CtrlWrite_ST7789(0xc6);
    LCD_DataWrite_ST7789(0x08);

    LCD_CtrlWrite_ST7789(0xd0);
    LCD_DataWrite_ST7789(0xa4);
    LCD_DataWrite_ST7789(0xa1);

    LCD_CtrlWrite_ST7789(0xe0);
    LCD_DataWrite_ST7789(0xd0);
    LCD_DataWrite_ST7789(0x04);
    LCD_DataWrite_ST7789(0x10);
    LCD_DataWrite_ST7789(0x13);
    LCD_DataWrite_ST7789(0x14);
    LCD_DataWrite_ST7789(0x2A);
    LCD_DataWrite_ST7789(0x41);
    LCD_DataWrite_ST7789(0x54);
    LCD_DataWrite_ST7789(0x4E);
    LCD_DataWrite_ST7789(0x18);
    LCD_DataWrite_ST7789(0x0C);
    LCD_DataWrite_ST7789(0x07);
    LCD_DataWrite_ST7789(0x1E);
    LCD_DataWrite_ST7789(0x25);

    LCD_CtrlWrite_ST7789(0xe1);
    LCD_DataWrite_ST7789(0xd0);
    LCD_DataWrite_ST7789(0x04);
    LCD_DataWrite_ST7789(0x0F);
    LCD_DataWrite_ST7789(0x13);
    LCD_DataWrite_ST7789(0x15);
    LCD_DataWrite_ST7789(0x2A);
    LCD_DataWrite_ST7789(0x40);
    LCD_DataWrite_ST7789(0x44);
    LCD_DataWrite_ST7789(0x52);
    LCD_DataWrite_ST7789(0x2D);
    LCD_DataWrite_ST7789(0x1D);
    LCD_DataWrite_ST7789(0x1E);
    LCD_DataWrite_ST7789(0x21);
    LCD_DataWrite_ST7789(0x24);

    LCD_CtrlWrite_ST7789(0x35);
    LCD_DataWrite_ST7789(0x00);

    LCD_CtrlWrite_ST7789(0x3A);
    LCD_DataWrite_ST7789(0x55);

    LCD_CtrlWrite_ST7789(0x29);
#endif

}

static void _st7789SleepIn(bool is_sleep)
{
    bootLog("lcd:  st7789  _st7789SleepIn, is_sleep = %d", is_sleep);

    if (is_sleep)
    {
        LCD_CtrlWrite_ST7789(0x28); //display off
        _lcdDelayMs(120);
        LCD_CtrlWrite_ST7789(0x10); // enter sleep mode
    }
    else
    {

#if 0
        LCD_CtrlWrite_ST7789(0x11); // sleep out mode
        _lcdDelayMs(120);
        LCD_CtrlWrite_ST7789(0x29); // display on
#endif
         halGoudatResetLcdPin();
        _lcdDelayMs(100);

        _st7789Init();
    }
}

static void _st7789Close(void)
{
    bootLog(0, "lcd:  st7789   in _st7789Close");

    _st7789SleepIn(true);
}
static void _st7789Invalidate(void)
{
    bootLog(0, "lcd:  st7789   in _st7789Invalidate");

    _st7789SetDisplayWindow(0x0, 0x0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
}

void _st7789InvalidateRect(
    uint16_t left,  //the left value of the rectangel
    uint16_t top,   //top of the rectangle
    uint16_t right, //right of the rectangle
    uint16_t bottom //bottom of the rectangle
)
{

    bootLog(0, "lcd: st7789 _st7789InvalidateRect");
    left = (left >= LCD_WIDTH) ? LCD_WIDTH - 1 : left;
    right = (right >= LCD_WIDTH) ? LCD_WIDTH - 1 : right;
    top = (top >= LCD_HEIGHT) ? LCD_HEIGHT - 1 : top;
    bottom = (bottom >= LCD_HEIGHT) ? LCD_HEIGHT - 1 : bottom;

    if ((right < left) || (bottom < top))
    {

        BOOT_ASSERT(false, "lcd: st7789 _st7789InvalidateRect error"); /*assert verified*/
    }

    _st7789SetDisplayWindow(left, top, right, bottom);
}

static void _st7789RotationInvalidateRect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, lcdAngle_t angle)
{

    bootLog(0, "lcd: st7789 st7789RotationInvalidateRect");
    switch (angle)
    {
    case LCD_ANGLE_0:
        _st7789SetDisplayWindow(left, top, right, bottom);
        break;
    case LCD_ANGLE_90:
        _st7789SetDisplayWindow(left, top, bottom, right);
        break;
    case LCD_ANGLE_180:
        _st7789SetDisplayWindow(left, top, right, bottom);
        break;
    case LCD_ANGLE_270:
        _st7789SetDisplayWindow(left, top, bottom, right);
        break;
    default:
        _st7789SetDisplayWindow(left, top, right, bottom);
        break;
    }
}

static void _st7789EsdCheck(void)
{

    bootLog("lcd:  st7789  st7789EsdCheck");
}

static const lcdOperations_t st7789vOperations =
    {
        _st7789Init,
        _st7789SleepIn,
        _st7789EsdCheck,
        _st7789SetDisplayWindow,
        _st7789InvalidateRect,
        _st7789Invalidate,
        _st7789Close,
        _st7789RotationInvalidateRect,
        NULL,
        _st7789ReadId,
};

const lcdSpec_t g_lcd_st7789v =
    {
        LCD_DRV_ID_ST7789,
        LCD_WIDTH,
        LCD_HEIGHT,
        HAL_GOUDA_SPI_LINE_4,
        LCD_CTRL_SPI,
        (lcdOperations_t *)&st7789vOperations,
        false,
        0x2a000,
        50000000,
};

static uint32_t _st7789ReadId(void)
{
    uint32_t ret_id = 0;
    HAL_ERR_T r_err;
    uint8_t id[4] = {0};
    bootLog("lcd: rst and read st7789 Id \n");
    halPmuSwitchPower(HAL_POWER_LCD, true, true);
    _lcdDelayMs(10);
    st7789_farmk = g_lcd_st7789v.is_use_fmark;
    st7789_line_mode = g_lcd_st7789v.bus_mode;
    halGoudatResetLcdPin();
    halGoudaReadConfig(st7789_line_mode, g_lcd_st7789v.is_use_fmark, g_lcd_st7789v.fmark_delay);
    _lcdDelayMs(140);
    //hwp_analogReg->pad_spi_lcd_cfg2 |= ANALOG_REG_PAD_SPI_LCD_SIO_SPU;
    r_err = halGoudaReadData(0x04, id, 4);
    //hwp_analogReg->pad_spi_lcd_cfg2 =  hwp_analogReg->pad_spi_lcd_cfg2&(~ANALOG_REG_PAD_SPI_LCD_SIO_SPU);
    _lcdDelayMs(10);
    bootLog("lcd:  st7789  _st7789ReadId LCM: 0x%0x, 0x%x, 0x%0x (read return : %d)\n", id[2], id[1], id[3], r_err);

    ret_id = ((id[3] << 16) | (id[2] << 8) | id[1]);

    if (LCD_DRV_ID_ST7789 == ret_id)
    {
        bootLog("lcd:  st7789 is using !");
    }
    halGoudaClose();
    halPmuSwitchPower(HAL_POWER_LCD, false, false);
    return ret_id;
}
