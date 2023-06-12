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

#define LCD_DRV_ID_ST7735 0x7c89f0

#define LCD_DEF_WIDTH 128
#define LCD_DEF_HEIGHT 128//160
int LCD_WIDTH = 128;
int LCD_HEIGHT = 128;

static uint16_t st7735_line_mode;
static bool st7735_farmk;



#define LCM_WR_REG(Addr, Data)                             \
    {                                                      \
        while (halGoudaWriteReg(Addr, Data) != HAL_ERR_NO) \
            ;                                              \
    }
#define LCD_DataWrite_ST7735(Data)                    \
    {                                                 \
        while (halGoudaWriteData(Data) != HAL_ERR_NO) \
            ;                                         \
    }
#define LCD_CtrlWrite_ST7735(Cmd)                   \
    {                                               \
        while (halGoudaWriteCmd(Cmd) != HAL_ERR_NO) \
            ;                                       \
    }

static void _st7735Close(void);
static uint32_t _st7735ReadId(void);
static void _st7735SleepIn(bool is_sleep);
static void _st7735Init(void);
static void _st7735Invalidate(void);
static void _st7735InvalidateRect(
    uint16_t left,  //the left value of the rectangel
    uint16_t top,   //top of the rectangle
    uint16_t right, //right of the rectangle
    uint16_t bottom //bottom of the rectangle
);

static void _st7735SetDisplayWindow(
    uint16_t left,  // start Horizon address
    uint16_t right, // end Horizon address
    uint16_t top,   // start Vertical address
    uint16_t bottom // end Vertical address
);
static void _lcdDelayMs(int ms_delay)
{
    bootDelayUS(ms_delay * 1000);
}

__UNUSED static void _st7735SetDirection(lcdDirect_t direct_type)
{

    bootLog("lcd:    _st7735SetDirection = %d", direct_type);

    switch (direct_type)
    {
    case LCD_DIRECT_NORMAL:
        LCD_CtrlWrite_ST7735(0x36);
        LCD_DataWrite_ST7735(0x00);
        break;
    case LCD_DIRECT_ROT_90:
        LCD_CtrlWrite_ST7735(0x36);
        LCD_DataWrite_ST7735(0x60);
        break;
    }

    LCD_CtrlWrite_ST7735(0x2c);
}

/******************************************************************************/
//  Description:   Set the windows address to display, in this windows
//                 color is  refreshed.
/******************************************************************************/
static void _st7735SetDisplayWindow(
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
    bootLog("lcd:st7735SetDisplayWindow L = %d, top = %d, R = %d, bot = %d", left, top, right, bottom);

    LCD_CtrlWrite_ST7735(0x2a);                   // set hori start , end (left, right)
    LCD_DataWrite_ST7735((newleft >> 8) & 0xff);  // left high 8 b
    LCD_DataWrite_ST7735(newleft & 0xff);         // left low 8 b
    LCD_DataWrite_ST7735((newright >> 8) & 0xff); // right high 8 b
    LCD_DataWrite_ST7735(newright & 0xff);        // right low 8 b

    if (LCD_HEIGHT == 128)
    {
        LCD_CtrlWrite_ST7735(0x2b);                    // set vert start , end (top, bot)
        LCD_DataWrite_ST7735((newtop >> 8) & 0xff);    // top high 8 b
        LCD_DataWrite_ST7735(newtop & 0xff);           // top low 8 b
        LCD_DataWrite_ST7735(((newbottom+32) >> 8) & 0xff); // bot high 8 b
        LCD_DataWrite_ST7735((newbottom+32) & 0xff);        // bot low 8 b
        LCD_CtrlWrite_ST7735(0x2c);                    // recover memory write mode
    }
    else
    {
        LCD_CtrlWrite_ST7735(0x2b);                    // set vert start , end (top, bot)
        LCD_DataWrite_ST7735((newtop >> 8) & 0xff);    // top high 8 b
        LCD_DataWrite_ST7735(newtop & 0xff);           // top low 8 b
        LCD_DataWrite_ST7735((newbottom >> 8) & 0xff); // bot high 8 b
        LCD_DataWrite_ST7735(newbottom & 0xff);        // bot low 8 b
        LCD_CtrlWrite_ST7735(0x2c);                    // recover memory write mode
    }

}

/**************************************************************************************/
// Description: initialize all LCD with LCDC MCU MODE and LCDC mcu mode
/**************************************************************************************/

static void _st7735Init(void)
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

            while (1)
            {
                LCD_REG_T regInfo = config.reglist[i];
                if (regInfo.addr == 0X00)
                {
                    //OSI_LOGI(0, "lcdtest count is %d", i);
                    break;
                }
                else if (regInfo.addr == 0XFF)
                {
                    _lcdDelayMs(regInfo.value);
                    //OSI_LOGI(0, "lcdtest delay %d", regInfo.value);
                }
                else if (regInfo.addr == last_addr)
                {
                    LCD_DataWrite_ST7735(regInfo.value);
                    //OSI_LOGI(0, "lcdtest datawrite %X", regInfo.value);
                }
                else
                {
                    LCD_CtrlWrite_ST7735(regInfo.addr);
                    //OSI_LOGI(0, "lcdtest ctrlwrite %X", regInfo.addr);
                    if (regInfo.value != 0XFF)
                    {
                        LCD_DataWrite_ST7735(regInfo.value);
                        //OSI_LOGI(0, "lcdtest datawrite %X", regInfo.value);
                    }
                    last_addr = regInfo.addr;
                }

                i++;
            }
            
            return;
        }
    }

    bootLog(0, "lcd:    _st7735Init ");
    //LCD_CtrlWrite_ST7735(0x11); //Sleep out
    _lcdDelayMs(100); //Delay 120ms
    //------------------------------------ST7735S Frame Rate-----------------------------------------//
    LCD_CtrlWrite_ST7735(0x11); 


    _lcdDelayMs(120);       //ms 

    LCD_CtrlWrite_ST7735(0xF0);
    LCD_DataWrite_ST7735(0x11);

    LCD_CtrlWrite_ST7735(0xF4);
    LCD_DataWrite_ST7735(0xCA);
    LCD_DataWrite_ST7735(0x78);
    LCD_DataWrite_ST7735(0x64);

    LCD_CtrlWrite_ST7735(0xB1);
    LCD_DataWrite_ST7735(0x01);//(0x05);
    LCD_DataWrite_ST7735(0x08);//(0x3C);
    LCD_DataWrite_ST7735(0x05);//(0x3C);

    LCD_CtrlWrite_ST7735(0xB2);
    LCD_DataWrite_ST7735(0x05);//(0x05);
    LCD_DataWrite_ST7735(0x3A);//(0x3C);
    LCD_DataWrite_ST7735(0x3A);//(0x3C);

    LCD_CtrlWrite_ST7735(0xB3);
    LCD_DataWrite_ST7735(0x05);//(0x05);
    LCD_DataWrite_ST7735(0x3A);//(0x3C);
    LCD_DataWrite_ST7735(0x3A);//(0x3C);
    LCD_DataWrite_ST7735(0x05);//(0x05);
    LCD_DataWrite_ST7735(0x3A);//(0x3C);
    LCD_DataWrite_ST7735(0x3A);//(0x3C);
    //------------------------------------End ST7735S Frame Rate-----------------------------------------//
    LCD_CtrlWrite_ST7735(0xB4); //Dot inversion
    LCD_DataWrite_ST7735(0x03);

    LCD_CtrlWrite_ST7735(0xC0);
    LCD_DataWrite_ST7735(0x62);//((0x28);
    LCD_DataWrite_ST7735(0x02);//((0x08);
    LCD_DataWrite_ST7735(0x04);//((0x04);

    LCD_CtrlWrite_ST7735(0xC1);
    LCD_DataWrite_ST7735(0XC0);

    LCD_CtrlWrite_ST7735(0xC2);
    LCD_DataWrite_ST7735(0x0D);
    LCD_DataWrite_ST7735(0x00);

    LCD_CtrlWrite_ST7735(0xC3);
    LCD_DataWrite_ST7735(0x8D);
    LCD_DataWrite_ST7735(0x6A);//((0x2A);

    LCD_CtrlWrite_ST7735(0xC4);
    LCD_DataWrite_ST7735(0x8D);
    LCD_DataWrite_ST7735(0xEE);
    //---------------------------------End ST7735S Power Sequence-------------------------------------//
    LCD_CtrlWrite_ST7735(0xC5); //VCOM
    LCD_DataWrite_ST7735(0x0F);//((0x1A);

    LCD_CtrlWrite_ST7735(0x36); //MX, MY, RGB mode
    LCD_DataWrite_ST7735(0x68);
    //------------------------------------ST7735S Gamma Sequence-----------------------------------------//
    LCD_CtrlWrite_ST7735(0xE0);
    LCD_DataWrite_ST7735(0x08);//((0x04);
    LCD_DataWrite_ST7735(0x1C);//((0x22);
    LCD_DataWrite_ST7735(0x17);//((0x07);
    LCD_DataWrite_ST7735(0x1C);//((0x0A);
    LCD_DataWrite_ST7735(0x3C);//((0x2E);
    LCD_DataWrite_ST7735(0x36);//((0x30);
    LCD_DataWrite_ST7735(0x2E);//((0x25);
    LCD_DataWrite_ST7735(0x30);//((0x2A);
    LCD_DataWrite_ST7735(0x2D);//((0x28);
    LCD_DataWrite_ST7735(0x29);//((0x26);
    LCD_DataWrite_ST7735(0x2E);//((0x2E);
    LCD_DataWrite_ST7735(0x37);//((0x3A);
    LCD_DataWrite_ST7735(0x00);//((0x00);
    LCD_DataWrite_ST7735(0x04);//((0x01);
    LCD_DataWrite_ST7735(0x01);//((0x03);
    LCD_DataWrite_ST7735(0x10);//((0x13);

    LCD_CtrlWrite_ST7735(0xE1);
    LCD_DataWrite_ST7735(0x05);//((0x04);
    LCD_DataWrite_ST7735(0x18);//((0x16);
    LCD_DataWrite_ST7735(0x14);//((0x06);
    LCD_DataWrite_ST7735(0x16);//((0x0D);
    LCD_DataWrite_ST7735(0x34);//((0x2D);
    LCD_DataWrite_ST7735(0x2E);//((0x26);
    LCD_DataWrite_ST7735(0x28);//((0x23);
    LCD_DataWrite_ST7735(0x2B);//((0x27);
    LCD_DataWrite_ST7735(0x2A);//((0x27);
    LCD_DataWrite_ST7735(0x28);//((0x25);
    LCD_DataWrite_ST7735(0x30);//((0x2D);
    LCD_DataWrite_ST7735(0x3B);//((0x3B);
    LCD_DataWrite_ST7735(0x00);//((0x00);
    LCD_DataWrite_ST7735(0x03);//((0x01);
    LCD_DataWrite_ST7735(0x00);//((0x04);
    LCD_DataWrite_ST7735(0x10);//((0x13);
    //------------------------------------End ST7735S Gamma Sequence-----------------------------------------//
    LCD_CtrlWrite_ST7735(0x3A); //65k mode
    LCD_DataWrite_ST7735(0x05);

    LCD_CtrlWrite_ST7735(0x26);   //ADD  
    LCD_DataWrite_ST7735(0x04);   //01  

    LCD_CtrlWrite_ST7735(0x35);     
    LCD_DataWrite_ST7735(0x00);  

    LCD_CtrlWrite_ST7735(0x29); //Display on

    _lcdDelayMs(20);

    //LCD_CtrlWrite_ST7735(0x2c);

}


static void _st7735SleepIn(bool is_sleep)
{
    bootLog("lcd:   _st7735SleepIn, is_sleep = %d", is_sleep);

    if (is_sleep)
    {
        LCD_CtrlWrite_ST7735(0x28); //display off
        _lcdDelayMs(120);
        LCD_CtrlWrite_ST7735(0x10); // enter sleep mode
    }
    else
    {
        _st7735Init();
    }
}

static void _st7735Close(void)
{
    bootLog("lcd:     in GC9304_Close");

    _st7735SleepIn(true);
}
static void _st7735Invalidate(void)
{
    bootLog("lcd:     in _st7735Invalidate");

    _st7735SetDisplayWindow(0x0, 0x0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
}

void _st7735InvalidateRect(
    uint16_t left,  //the left value of the rectangel
    uint16_t top,   //top of the rectangle
    uint16_t right, //right of the rectangle
    uint16_t bottom //bottom of the rectangle
)
{

    bootLog("lcd:  _st7735InvalidateRect");
    left = (left >= LCD_WIDTH) ? LCD_WIDTH - 1 : left;
    right = (right >= LCD_WIDTH) ? LCD_WIDTH - 1 : right;
    top = (top >= LCD_HEIGHT) ? LCD_HEIGHT - 1 : top;
    bottom = (bottom >= LCD_HEIGHT) ? LCD_HEIGHT - 1 : bottom;

    if ((right < left) || (bottom < top))
    {

        BOOT_ASSERT(false, "lcd:  _st7735InvalidateRect error"); /*assert verified*/
    }

    _st7735SetDisplayWindow(left, top, right, bottom);
}

static void _st7735RotationInvalidateRect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, lcdAngle_t angle)
{

    bootLog("lcd: gc9305 gc9305RotationInvalidateRect");
    switch (angle)
    {
    case LCD_ANGLE_0:
        _st7735SetDisplayWindow(left, top, right, bottom);
        break;
    case LCD_ANGLE_90:
        _st7735SetDisplayWindow(left, top, bottom, right);
        break;
    case LCD_ANGLE_180:
        _st7735SetDisplayWindow(left, top, right, bottom);
        break;
    case LCD_ANGLE_270:
        _st7735SetDisplayWindow(left, top, bottom, right);
        break;
    default:
        _st7735SetDisplayWindow(left, top, right, bottom);
        break;
    }
}

static void _st7735EsdCheck(void)
{

    bootLog("lcd:  st7735sEsdCheck");
}

static const lcdOperations_t st7735sOperations =
    {
        _st7735Init,
        _st7735SleepIn,
        _st7735EsdCheck,
        _st7735SetDisplayWindow,
        _st7735InvalidateRect,
        _st7735Invalidate,
        _st7735Close,
        _st7735RotationInvalidateRect,
        NULL,
        _st7735ReadId,
};

lcdSpec_t g_lcd_st7735s =
    {
        LCD_DRV_ID_ST7735,
        LCD_DEF_WIDTH,
        LCD_DEF_HEIGHT,
        HAL_GOUDA_SPI_LINE_4,
        LCD_CTRL_SPI,
        (lcdOperations_t *)&st7735sOperations,
        false,
        0x2a000,
       10000000,// 500000, //10000000, 10M
};

static uint32_t _st7735ReadId(void)
{
    uint32_t ret_id = 0;
    HAL_ERR_T r_err;
    uint8_t id[4] = {0};
    bootLog("lcd:st7735ReadId \n");


    halPmuSwitchPower(HAL_POWER_LCD, true, true);
    _lcdDelayMs(10);
    st7735_farmk = g_lcd_st7735s.is_use_fmark;
    st7735_line_mode = g_lcd_st7735s.bus_mode;

    LCDCONFIG_T config={0};
    bootLcdExtConfigGet(&config);
    if (config.valid)
    {
        g_lcd_st7735s.width = config.width;
        g_lcd_st7735s.height = config.heigh;
        bootLog(0, "lcd init change width %d height %d", g_lcd_st7735s.width,g_lcd_st7735s.height);
    }


    halGoudaReadConfig(st7735_line_mode, g_lcd_st7735s.is_use_fmark, g_lcd_st7735s.fmark_delay);
    _lcdDelayMs(140);
    //hwp_analogReg->pad_spi_lcd_cfg2 |= ANALOG_REG_PAD_SPI_LCD_SIO_SPU;
    _lcdDelayMs(10);
    r_err = halGoudaReadData(0x04, id, 4);
    //hwp_analogReg->pad_spi_lcd_cfg2 =  hwp_analogReg->pad_spi_lcd_cfg2&(~ANALOG_REG_PAD_SPI_LCD_SIO_SPU);
    bootLog("lcd:st7735ReadId LCM: 0x%0x, 0x%x, 0x%0x (read return : %d)\n", id[2], id[1], id[3], r_err);
    ret_id = ((id[3] << 16) | (id[2] << 8) | id[1]);

    if (LCD_DRV_ID_ST7735 == ret_id)
    {
        bootLog("lcd:   is using !");
    }
    halGoudaClose();
    halPmuSwitchPower(HAL_POWER_LCD, false, false);
    bootDebugEvent(0x666661d);bootDebugEvent(ret_id);
    return ret_id;
}


