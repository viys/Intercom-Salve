#ifndef __KING_DISPLAY_H__
#define __KING_DISPLAY_H__

#include "KingDef.h"


typedef struct
{
    /// x coordinate of the top left corner of the lcd.
    uint16 x;
    /// y coordinate of the top left corner of the lcd.
    uint16 y;
    uint16 width;
    uint16 height;
} LCDDISPLAY_T;

typedef enum
{
    LCD_DIRECT_NORMAL = 0x00,
    LCD_DIRECT_ROT_90, // Rotation 90 degree clockwise
    LCD_DIRECT_ROT_180, // Rotation 180 degree clockwise
    LCD_DIRECT_ROT_270, // Rotation 270 degree clockwise
    LCD_DIRECT_MAX = 0x7FFFFFFF
}LCDDIRECT_T;

typedef enum
{
    LCD_BPP_RGB888 = 0x00,
    LCD_BPP_RGB565,
    LCD_BPP_MAX = 0x7FFFFFFF
}LCDBPP_T;

typedef enum
{
    LCD_LAYOUT_HORIZONTAL,
    LCD_LAYOUT_VERTICAL,
    LCD_LAYOUT_MAX = 0x7FFFFFFF
}LCDLAYOUT_T;

typedef struct
{
    uint8 addr;
    uint8 value;
}LCDREG_T;

typedef struct
{
    uint8 valid;
    uint8 width;
    uint8 heigh;
    uint8 rotate;
    LCDREG_T *reglist;
}LCDCONFIG_T;

typedef struct{
    uint16 left;
    uint16 top;
} POINT;

typedef struct{
    uint16 fontsize; //font size
    uint16 bgcolor; //background color
    uint16 fgcolor; //foreground color
    uint16 transparent; //0:不透; 1:全透
    uint16 len;     //字符串长度
    wchar *pStr; //utf-16
} STR_PARA_S;

/**
 * LCD初始化
 * 
 * @param[in]  lcdid   初始化哪个LCD，如只有1个LCD，则为0
 * @param[in]  config  屏幕相关的配置信息
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdInit(uint32 lcdid, LCDCONFIG_T *config);

/**
 * LCD反初始化
 * 
 * @param[in]  lcdid   去初始化哪个LCD，如只有1个LCD，则为0
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdDeinit(uint32 lcdid);

/**
 * 点亮LCD屏幕
 * 
 * @param[in]  lcdid   指定操作的LCD，如只有1个LCD，则为0
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdScreenOn(uint16 lcdid);

/**
 * 关闭LCD屏幕
 * 
 * @param[in]  lcdid   指定操作的LCD，如只有1个LCD，则为0
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdScreenOff(uint16 lcdid);


/**
 * 获取色深
 * 
 * @param[in]  lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @param[in]  bpp     色深值定义详见LCDBPP_T
 *                     RGB565一个像素16bit,RGB888 一个像素24bit
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdBppGet(uint32 lcdid, LCDBPP_T* bpp);

/**
 * LCD亮度设置
 * 
 * @param[in] lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @param[in] value   LCD亮度值,范围0-100
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdBrightnessSet(uint32 lcdid, uint32 value);

/**
 * LCD亮度获取
 * 
 * @param[in]  lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @param[out] *value  获取LCD的亮度值。开机时默认为100
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdBrightnessGet(uint32 lcdid, uint32 *value);

/**
 * 设置LCD旋转角度
 * 最好在初始化就调用，默认无旋转
 * 
 * @param[in]  lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @param[in]  Dir     LCD旋转角度LCDBPP_T
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdDirectionSet(uint32 lcdid, LCDDIRECT_T Dir);

/**
 * 获取LCD旋转角度
 * 
 * @param[in]  lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @param[out] Dir     获取到的LCD旋转角度
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdDirectionGet(uint32 lcdid, LCDDIRECT_T *Dir);

/**
 * LCD色温设置
 * 
 * @param[in] lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @param[in] value   LCD的色温值，单位为k
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdColortmpSet(uint32 lcdid, uint32 value);

/**
 * LCD色温获取
 * 
 * @param[in] lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @param[out] value  获取LCD的色温值，单位为k
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdColortmpGet(uint32 lcdid, uint32 *value);


/**
 * LCD对比度设置
 * 
 * @param[in] lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @param[in] value   LCD的对比度值，取值范围0-100
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdContrastSet(uint32 lcdid, uint32 value);

/**
 * LCD对比度获取
 * 
 * @param[in]  lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @param[out] *value  获取LCD的对比度值
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdContrastGet(uint32 lcdid, uint32 *value);


/**
 * LCD色调设置
 * 
 * @param[in] lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @param[in] value   LCD的色调值
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdHueSet(uint32 lcdid, uint32 value);

/**
 * LCD色调值获取
 * 
 * @param[in]  lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @param[out] *value  获取LCD的色调值
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdHueGet(uint32 lcdid, uint32 *value);

/**
 * 清除画布上的内容
 * 
 * @param[in] lcdid   修改哪个LCD对应的canvas,LCDInit时创建
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdCanvasClear(uint32 lcdid);

/**
 * 在画布上添加直线
 * 
 * @param[in] lcdid   修改哪个LCD对应的canvas,LCDInit时创建
 * @param[in] rec 结构体中xy为起始点，根据direction参数确认width或height为长度
 * @param[in] direction 直线的方向
 * @param[in] color 直线的颜色。根据KING_LcdBppGet获取的信息赋值
 * @param[in] transparence 透明度。0不透明 1透明
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdDrawLine(LCDDISPLAY_T rec, LCDLAYOUT_T direction, uint32 color,uint32 lcdid,uint32 transparence);

/**
 * 在画布上添加色块
 * 
 * @param[in] lcdid   修改哪个LCD对应的canvas,LCDInit时创建
 * @param[in] rec 添加的色块的范围
 * @param[in] color 直线的颜色。根据KING_LcdBppGet获取的信息赋值
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdFillRect(LCDDISPLAY_T rec, uint32 color,uint32 lcdid);

/**
 * 把画布内容显示到LCD
 * 
 * @param[in] lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdShow(uint32 lcdid);

/**
 * LCD显示
 * 
 * @param[in]  rec     显示起始位置及显示的长度，宽度
 * @param[in]  data    填充的数据.根据KING_LcdBppGet获取的信息赋值
 * @param[in]  clear   填充前是否清空屏幕
 * @param[in]  lcdid   在哪个LCD上面显示，如只有1个LCD，则为0
 */
int KING_LcdDisplay(LCDDISPLAY_T rec, uint8* data, bool clear, uint32 lcdid);

/**
 * 字串显示
 * 
 * @param[in]  lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @param[in]  rec     字串显示区域，width为0表示显示到屏幕边缘
 * @param[in]  para    字串的相关参数，详见结构体STR_PARA_S
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdDisplayStr(uint32 lcdid, LCDDISPLAY_T rec,  STR_PARA_S *para);


/**
 * 进入或者退出sleep状态
 * 
 * @param[in]  lcdid   设置哪个LCD，如只有1个LCD，则为0
 * @param[in]  isSleep   0退出sleep状态,1进入sleep状态
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdSleep(uint32 lcdid, uint32 isSleep);

#endif

