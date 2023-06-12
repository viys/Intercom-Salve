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


#define LCD_DRV_ID_ILI9163 0X548066
#define LCD_DEF_WIDTH 128
#define LCD_DEF_HEIGHT 128//160
static int LCD_WIDTH = 128;
static int LCD_HEIGHT = 128;


static uint16_t ili9163_line_mode;
static bool ili9163_farmk;



#define LCM_WR_REG(Addr, Data)                             \
    {                                                      \
        while (halGoudaWriteReg(Addr, Data) != HAL_ERR_NO) \
            ;                                              \
    }
#define LCD_DataWrite_ILI9163(Data)                    \
    {                                                 \
        while (halGoudaWriteData(Data) != HAL_ERR_NO) \
            ;                                         \
    }
#define LCD_CtrlWrite_ILI9163(Cmd)                   \
    {                                               \
        while (halGoudaWriteCmd(Cmd) != HAL_ERR_NO) \
            ;                                       \
    }

static void _ili9163Close(void);
static uint32_t _ili9163ReadId(void);
static void _ili9163SleepIn(bool is_sleep);
static void _ili9163Init(void);
static void _ili9163Invalidate(void);
static void _ili9163InvalidateRect(
    uint16_t left,  //the left value of the rectangel
    uint16_t top,   //top of the rectangle
    uint16_t right, //right of the rectangle
    uint16_t bottom //bottom of the rectangle
);

static void _ili9163SetDisplayWindow(
    uint16_t left,  // start Horizon address
    uint16_t right, // end Horizon address
    uint16_t top,   // start Vertical address
    uint16_t bottom // end Vertical address
);
static void _lcdDelayMs(int ms_delay)
{
    bootDelayUS(ms_delay * 1000);
}

__UNUSED static void _ili9163SetDirection(lcdDirect_t direct_type)
{

    bootLog("lcd:    _ili9163SetDirection = %d", direct_type);

    switch (direct_type)
    {
    case LCD_DIRECT_NORMAL:
        LCD_CtrlWrite_ILI9163(0x36);
        LCD_DataWrite_ILI9163(0x00);
        break;
    case LCD_DIRECT_ROT_90:
        LCD_CtrlWrite_ILI9163(0x36);
        LCD_DataWrite_ILI9163(0x60);
        break;
    }

    LCD_CtrlWrite_ILI9163(0x2c);
}

/******************************************************************************/
//  Description:   Set the windows address to display, in this windows
//                 color is  refreshed.
/******************************************************************************/
static void _ili9163SetDisplayWindow(
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
    bootLog("lcd:ili9163SetDisplayWindow L = %d, top = %d, R = %d, bot = %d", left, top, right, bottom);

    LCD_CtrlWrite_ILI9163(0x2a);                   // set hori start , end (left, right)
    LCD_DataWrite_ILI9163((newleft >> 8) & 0xff);  // left high 8 b
    LCD_DataWrite_ILI9163(newleft & 0xff);         // left low 8 b
    LCD_DataWrite_ILI9163((newright >> 8) & 0xff); // right high 8 b
    LCD_DataWrite_ILI9163(newright & 0xff);        // right low 8 b

    if (LCD_HEIGHT == 128)
    {
        LCD_CtrlWrite_ILI9163(0x2b);                    // set vert start , end (top, bot)
        LCD_DataWrite_ILI9163((newtop >> 8) & 0xff);    // top high 8 b
        LCD_DataWrite_ILI9163(newtop & 0xff);           // top low 8 b
        LCD_DataWrite_ILI9163(((newbottom+32) >> 8) & 0xff); // bot high 8 b
        LCD_DataWrite_ILI9163((newbottom+32) & 0xff);        // bot low 8 b
        LCD_CtrlWrite_ILI9163(0x2c);                    // recover memory write mode
    }
    else
    {
        LCD_CtrlWrite_ILI9163(0x2b);                    // set vert start , end (top, bot)
        LCD_DataWrite_ILI9163((newtop >> 8) & 0xff);    // top high 8 b
        LCD_DataWrite_ILI9163(newtop & 0xff);           // top low 8 b
        LCD_DataWrite_ILI9163((newbottom >> 8) & 0xff); // bot high 8 b
        LCD_DataWrite_ILI9163(newbottom & 0xff);        // bot low 8 b
        LCD_CtrlWrite_ILI9163(0x2c);                    // recover memory write mode
    }

}

/**************************************************************************************/
// Description: initialize all LCD with LCDC MCU MODE and LCDC mcu mode
/**************************************************************************************/
static void _ili9163Init(void)
{
    LCDCONFIG_T config={0};
    bootLcdExtConfigGet(&config);
    if (config.valid)
    {
        LCD_WIDTH = config.width;
        LCD_HEIGHT = config.heigh;
        
        if (config.reglist != NULL)
        {
            int i = 0;
            uint8_t last_addr = 0XFF;
            _lcdDelayMs(60);
            while (1)
            {
                LCD_REG_T regInfo = config.reglist[i];
                if (regInfo.addr == 0X00)
                {
                    //bootLog(0, "lcdtest count is %d", i);
                    break;
                }
                else if (regInfo.addr == 0XFF)
                {
                    _lcdDelayMs(regInfo.value);
                    //bootLog(0, "lcdtest delay %d", regInfo.value);
                }
                else if (regInfo.addr == last_addr)
                {
                    LCD_DataWrite_ILI9163(regInfo.value);
                    //bootLog(0, "lcdtest datawrite %X", regInfo.value);
                }
                else
                {
                    LCD_CtrlWrite_ILI9163(regInfo.addr);
                    //bootLog(0, "lcdtest ctrlwrite %X", regInfo.addr);
                    if (regInfo.value != 0XFF)
                    {
                        LCD_DataWrite_ILI9163(regInfo.value);
                        //bootLog(0, "lcdtest datawrite %X", regInfo.value);
                    }
                    last_addr = regInfo.addr;
                }

                i++;
            }
            return;
        }
    }

    bootLog("lcd:    _ili9163Init ");
    // VCI=2.8V
    _lcdDelayMs(60); // Delay 60 ms
    //************* Start Initial Sequence **********//
    LCD_CtrlWrite_ILI9163(0x11); //Exit Sleep
    _lcdDelayMs(20);
    LCD_CtrlWrite_ILI9163(0x26); //Set Default Gamma
    LCD_DataWrite_ILI9163(0x04);

    LCD_CtrlWrite_ILI9163(0xB1);
    LCD_DataWrite_ILI9163(0x0E);
    LCD_DataWrite_ILI9163(0x14);

    LCD_CtrlWrite_ILI9163(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
    LCD_DataWrite_ILI9163(0x08); //GVDD:4.40
    LCD_DataWrite_ILI9163(0x00); //VCI1:2.75

    LCD_CtrlWrite_ILI9163(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
    LCD_DataWrite_ILI9163(0x05); //101 5 2xVCI -1xVCI1 6xVCI -6xVCI

    LCD_CtrlWrite_ILI9163(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
    LCD_DataWrite_ILI9163(0x50); //VCOMH:4.5
    LCD_DataWrite_ILI9163(0x40); //VCOML:-0.9

    LCD_CtrlWrite_ILI9163(0xC7);
    LCD_DataWrite_ILI9163(0x40);//0x40

    LCD_CtrlWrite_ILI9163(0x3a); //Set Color Format
    LCD_DataWrite_ILI9163(0x05);

    LCD_CtrlWrite_ILI9163(0x2A); //Set Column Address
    LCD_DataWrite_ILI9163(0x00);
    LCD_DataWrite_ILI9163(0x00);
    LCD_DataWrite_ILI9163(0x00);
    LCD_DataWrite_ILI9163(0x7F);

    LCD_CtrlWrite_ILI9163(0x2B); //Set Page Address
    LCD_DataWrite_ILI9163(0x00);
    LCD_DataWrite_ILI9163(0x00);
    LCD_DataWrite_ILI9163(0x00);
    LCD_DataWrite_ILI9163(0x9F);

    LCD_CtrlWrite_ILI9163(0x36); //Set Scanning Direction
    LCD_DataWrite_ILI9163(0x68);

    LCD_CtrlWrite_ILI9163(0xB4); //Display Inversion Control
    LCD_DataWrite_ILI9163(0x00);

    LCD_CtrlWrite_ILI9163(0xB7); //Set qSource Output Direction
    LCD_DataWrite_ILI9163(0x00);

    LCD_CtrlWrite_ILI9163(0xf2); //Enable Gamma bit
    LCD_DataWrite_ILI9163(0x01);

    LCD_CtrlWrite_ILI9163(0xE0); 
    LCD_DataWrite_ILI9163(0x3F);//p1 
    LCD_DataWrite_ILI9163(0x26);//p2 
    LCD_DataWrite_ILI9163(0x23);//p3 
    LCD_DataWrite_ILI9163(0x30);//p4 
    LCD_DataWrite_ILI9163(0x28);//p5 
    LCD_DataWrite_ILI9163(0x10);//p6 
    LCD_DataWrite_ILI9163(0x55);//p7 
    LCD_DataWrite_ILI9163(0xB7);//p8 
    LCD_DataWrite_ILI9163(0x40);//p9 
    LCD_DataWrite_ILI9163(0x19);//p10 
    LCD_DataWrite_ILI9163(0x10);//p11 
    LCD_DataWrite_ILI9163(0x1E);//p12 
    LCD_DataWrite_ILI9163(0x02);//p13 
    LCD_DataWrite_ILI9163(0x01);//p14 
    LCD_DataWrite_ILI9163(0x00);//p15 
    
    LCD_CtrlWrite_ILI9163(0xE1); 
    LCD_DataWrite_ILI9163(0x00);//p1 
    LCD_DataWrite_ILI9163(0x19);//p2 
    LCD_DataWrite_ILI9163(0x1C);//p3 
    LCD_DataWrite_ILI9163(0x0F);//p4 
    LCD_DataWrite_ILI9163(0x14);//p5 
    LCD_DataWrite_ILI9163(0x0F);//p6 
    LCD_DataWrite_ILI9163(0x2A);//p7 
    LCD_DataWrite_ILI9163(0x48);//p8 
    LCD_DataWrite_ILI9163(0x3F);//p9 
    LCD_DataWrite_ILI9163(0x06);//p10 
    LCD_DataWrite_ILI9163(0x1D);//p11 
    LCD_DataWrite_ILI9163(0x21);//p12 
    LCD_DataWrite_ILI9163(0x3d);//p13 
    LCD_DataWrite_ILI9163(0x3e);//p14 
    LCD_DataWrite_ILI9163(0x3f);//p15 

    LCD_CtrlWrite_ILI9163(0x29); // Display On
    LCD_CtrlWrite_ILI9163(0x2C);

}

static void _ili9163SleepIn(bool is_sleep)
{
    bootLog("lcd:   _ili9163SleepIn, is_sleep = %d", is_sleep);

    if (is_sleep)
    {
        LCD_CtrlWrite_ILI9163(0x28); //display off
        _lcdDelayMs(120);
        LCD_CtrlWrite_ILI9163(0x10); // enter sleep mode
    }
    else
    {
#if 0
        LCD_CtrlWrite_ILI9163(0x11); // sleep out mode
        _lcdDelayMs(120);
        LCD_CtrlWrite_ILI9163(0x29); // display on
#endif
        _ili9163Init();
    }
}

static void _ili9163Close(void)
{
    bootLog("lcd:     in _ili9163Close");

    _ili9163SleepIn(true);
}
static void _ili9163Invalidate(void)
{
    bootLog("lcd:     in _ili9163Invalidate");

    _ili9163SetDisplayWindow(0x0, 0x0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
}

void _ili9163InvalidateRect(
    uint16_t left,  //the left value of the rectangel
    uint16_t top,   //top of the rectangle
    uint16_t right, //right of the rectangle
    uint16_t bottom //bottom of the rectangle
)
{

    bootLog("lcd:  _ili9163InvalidateRect");
    left = (left >= LCD_WIDTH) ? LCD_WIDTH - 1 : left;
    right = (right >= LCD_WIDTH) ? LCD_WIDTH - 1 : right;
    top = (top >= LCD_HEIGHT) ? LCD_HEIGHT - 1 : top;
    bottom = (bottom >= LCD_HEIGHT) ? LCD_HEIGHT - 1 : bottom;

    if ((right < left) || (bottom < top))
    {
        BOOT_ASSERT(false, "lcd:  _ili9163InvalidateRect error"); /*assert verified*/
    }

    _ili9163SetDisplayWindow(left, top, right, bottom);
}

static void _ili9163RotationInvalidateRect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, lcdAngle_t angle)
{

    bootLog("lcd: ili9163 st7735RotationInvalidateRect");
    switch (angle)
    {
    case LCD_ANGLE_0:
        _ili9163SetDisplayWindow(left, top, right, bottom);
        break;
    case LCD_ANGLE_90:
        _ili9163SetDisplayWindow(left, top, bottom, right);
        break;
    case LCD_ANGLE_180:
        _ili9163SetDisplayWindow(left, top, right, bottom);
        break;
    case LCD_ANGLE_270:
        _ili9163SetDisplayWindow(left, top, bottom, right);
        break;
    default:
        _ili9163SetDisplayWindow(left, top, right, bottom);
        break;
    }
}

static void _ili9163EsdCheck(void)
{
    bootLog("lcd:  ili9163sEsdCheck");
}

static const lcdOperations_t ili9163vOperations =
    {
        _ili9163Init,
        _ili9163SleepIn,
        _ili9163EsdCheck,
        _ili9163SetDisplayWindow,
        _ili9163InvalidateRect,
        _ili9163Invalidate,
        _ili9163Close,
        _ili9163RotationInvalidateRect,
        NULL,
        _ili9163ReadId,
};

lcdSpec_t g_lcd_ili9163v =
    {
        LCD_DRV_ID_ILI9163,
        LCD_DEF_WIDTH,
        LCD_DEF_HEIGHT,
        HAL_GOUDA_SPI_LINE_4,
        LCD_CTRL_SPI,
        (lcdOperations_t *)&ili9163vOperations,
        false,
        0x2a000,
       10000000,// 500000, //10000000, 10M
};

static uint32_t _ili9163ReadId(void)
{
    uint32_t ret_id = 0;
    HAL_ERR_T r_err;
    uint8_t id[4] = {0};
    bootLog("lcd:ili9163ReadId \n");
    halPmuSwitchPower(HAL_POWER_LCD, true, true);
    _lcdDelayMs(10);
    ili9163_farmk = g_lcd_ili9163v.is_use_fmark;
    ili9163_line_mode = g_lcd_ili9163v.bus_mode;

    LCDCONFIG_T config={0};
    bootLcdExtConfigGet(&config);
    if (config.valid)
    {
        g_lcd_ili9163v.width = config.width;
        g_lcd_ili9163v.height = config.heigh;
        bootLog("lcd init change width %d height %d", g_lcd_ili9163v.width,g_lcd_ili9163v.height);
    }

    halGoudaReadConfig(ili9163_line_mode, g_lcd_ili9163v.is_use_fmark, g_lcd_ili9163v.fmark_delay);
    _lcdDelayMs(140);
    //hwp_analogReg->pad_spi_lcd_cfg2 |= ANALOG_REG_PAD_SPI_LCD_SIO_SPU;
    _lcdDelayMs(10);
    r_err = halGoudaReadData(0x04, id, 4);
    //hwp_analogReg->pad_spi_lcd_cfg2 =  hwp_analogReg->pad_spi_lcd_cfg2&(~ANALOG_REG_PAD_SPI_LCD_SIO_SPU);
    bootLog(0, "lcd:ili9163ReadId LCM: 0x%0x, 0x%x, 0x%0x (read return : %d)\n", id[2], id[1], id[3], r_err);
    ret_id = ((id[3] << 16) | (id[2] << 8) | id[1]);

    if (LCD_DRV_ID_ILI9163 == ret_id)
    {
        bootLog(0, "lcd:   is using !");
    }
    halGoudaClose();
    halPmuSwitchPower(HAL_POWER_LCD, false, false);
    bootDebugEvent(0x666661d);bootDebugEvent(ret_id);
    return ret_id;
}


