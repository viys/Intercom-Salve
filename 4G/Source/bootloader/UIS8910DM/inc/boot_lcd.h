/* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#ifndef _DRV_LCD_H_
#define _DRV_LCD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_gouda.h"
typedef struct
{
    uint8_t addr;
    uint8_t value;
}LCD_REG_T;

typedef struct
{
    uint8_t valid;
    uint8_t width;
    uint8_t heigh;
    uint8_t rotate;
    LCD_REG_T *reglist;
}LCDCONFIG_T;

typedef enum
{
    LCD_ANGLE_0,
    LCD_ANGLE_90,
    LCD_ANGLE_180,
    LCD_ANGLE_270
} lcdAngle_t;

typedef enum
{
    LCD_DIRECT_NORMAL = 0x00,
    LCD_DIRECT_ROT_90, // Rotation 90
} lcdDirect_t;

// LCD Contrller type
typedef enum
{
    LCD_CTRL_SPI = 0, //
    LCD_CTRL_CSI
} LCD_CTRL_MODULE_E;

//Block Configuration
typedef struct
{
    uint16_t start_x;
    uint16_t start_y;
    uint16_t end_x;
    uint16_t end_y;
    uint16_t colorkey;
    uint16_t width;     //real data width in SDRAM
    uint8_t resolution; //0:RGB888; 1:RGB666; 2:RGB565(RGB555 if OSD)
    uint8_t type;       //0:OSD; 1:image
    uint8_t alpha;
    uint8_t colorkey_en;
    uint32_t rotation;
} lcdBlockCfg_t;

typedef struct
{
    void (*init)(void);
    void (*enterSleep)(bool is_sleep);
    void (*checkEsd)(void);
    void (*setDisplayWindow)(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
    void (*invlaidateRect)(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
    void (*invalidate)(void);
    void (*close)(void);
    void (*rotationInvalidateRect)(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, lcdAngle_t angle);
    void (*setBrushDirection)(lcdDirect_t direct_type);
    uint32_t (*readId)(void);
} lcdOperations_t;

typedef struct
{
    uint32_t dev_id;
    uint32_t width;
    uint32_t height;
    HAL_GOUDA_SPI_LINE_T bus_mode;
    LCD_CTRL_MODULE_E controller;
    lcdOperations_t *operation;
    bool is_use_fmark;
    uint32_t fmark_delay;
    uint32_t freq;
} lcdSpec_t;

//=============================================================================
// lcdDisplay_t
//-----------------------------------------------------------------------------
/// Region of interest in a LCD screen or a buffer.
//=============================================================================
typedef struct
{
    /// x coordinate of the top left corner of the lcd.
    uint16_t x;
    /// y coordinate of the top left corner of the lcd.
    uint16_t y;
    uint16_t width;
    uint16_t height;

} lcdDisplay_t;

typedef struct
{
    /// Width of the region.
    uint16_t width;
    /// Height of the region.
    uint16_t height;
} lcdRoi_t;

//resolution of lcd data

#define LCD_RESOLUTION_RGB565 2
#define LCD_RESOLUTION_YUV422_UYVY 5
#define LCD_RESOLUTION_YUV420 6
#define LCD_RESOLUTION_YUV422_YUYV 9

//=============================================================================
// lcdFrameBuffer_t
//-----------------------------------------------------------------------------
/// Frame buffer type: this type describes a frame buffer through its dimensions
/// and a pointer to the data of the buffer. It is mandatory for the buffer to
/// be able to hold the data describing all the pixels at the color format
/// choosen.
///(ie sizeof(buffer) = width*height*NbOfBytesPerPixelAtTheFormat(format))
// colorFormat : support LCD_RESOLUTION_RGB565 ,LCD_RESOLUTION_YUV422 LCD_RESOLUTION_YUV420
//widthOrignal  : the width of the original image buffer,it will just be used to caculate the stribe
//width : the width will convert to. Normal is will equal with widthOrignal,unless zoom,rotation.
//height : the height will convert to.
//=============================================================================
typedef struct
{
    uint16_t *buffer;
    /// buffer x coordinate of the top left corner of the lcd.
    uint16_t region_x;
    ///buffer  y coordinate of the top left corner of the lcd.
    uint16_t region_y;
    ///the width of after convert. if don't need rotation and zoom ,set to  roginal width.
    uint16_t width;
    ///the height of after convert. if don't need rotation and zoom ,set to  roginal height.
    uint16_t height;
    ///image buffer width.
    uint16_t widthOriginal;
    uint16_t colorFormat;
    ///0 no rotation. 1 rotate 90 degree.
    uint16_t rotation;
    /// enable the color mask function.
    bool keyMaskEnable;
    /// mask color value
    uint16_t maskColor;
} lcdFrameBuffer_t;

/**
 * @brief Init the LCD moudle.
 * @return
 *      false  failure
 *      true  success.
 */
bool bootLcdInit(LCDCONFIG_T* config);

/**
 * @brief The function is used to close lcd when power off.
 */
void bootLcdClose(void);

/**
 * @brief This function provides the basic bit-block transfer capabilities.
 * copies the data (such as characters/bmp) on the LCD directly
 * as a (rectangular) block. The data is drawn in the active window.
 * The buffer has to be properly aligned
 * @param dataBufferWin  set the buffer address , height, width .
 * @param lcdRec   set the position of lcd region.
 * @return
 *      - true on success else false
 */
bool bootLcdBlockTransfer(const lcdFrameBuffer_t *dataBufferWin, const lcdDisplay_t *lcdRec);

/**
 * @brief This function to config the belower layer/vedio layer.
 * @param dataBufferWin  set the buffer address , height, width,etc .
 * @return
 *      - true on success else false
 */

bool bootLcdBlockSetOverLay(const lcdFrameBuffer_t *dataBufferWin);

/**
 * @brief This function get the basic information of lcd.
 * @param lcd_info_ptr  point to store lcd information
 */
void bootLcdGetLcdInfo(lcdSpec_t *lcd_info_ptr);

/**
 * @brief Set the lcd display window.
 * @param  left of the display window
 * @param  top of the display window
 * @param  right of the display window
 * @param  bottom of the display window
 * @return
 *      - true on success else false
 */
bool bootLcdSetDisplayWindow(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);

/**
 * @brief Set video layer parameter
 * @width  the orign pic buffer width, if rotation 90 degree, should set the height of origin pic .
 * @height  the orign pic buffer height, if rotation 90 degree, should set the width of origin pic .
 * @return
 *      - true on success else false
 */

bool bootLcdSetImageLayerSize(uint32_t width, uint32_t height, uint32_t imageBufWidth);




void bootLcdExtConfigSet(LCDCONFIG_T* config);
void bootLcdExtConfigGet(LCDCONFIG_T* config);

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/

int bootLcdShow(uint16 height,uint16 width,uint16* canvas);

void bootLcdBacklightCtrl(uint32_t onoff);


#ifdef __cplusplus
}
#endif

#endif //_DAL_LCD_H
