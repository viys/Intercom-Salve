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
    uint16 transparent; //0:��͸; 1:ȫ͸
    uint16 len;     //�ַ�������
    wchar *pStr; //utf-16
} STR_PARA_S;

/**
 * LCD��ʼ��
 * 
 * @param[in]  lcdid   ��ʼ���ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[in]  config  ��Ļ��ص�������Ϣ
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdInit(uint32 lcdid, LCDCONFIG_T *config);

/**
 * LCD����ʼ��
 * 
 * @param[in]  lcdid   ȥ��ʼ���ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdDeinit(uint32 lcdid);

/**
 * ����LCD��Ļ
 * 
 * @param[in]  lcdid   ָ��������LCD����ֻ��1��LCD����Ϊ0
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdScreenOn(uint16 lcdid);

/**
 * �ر�LCD��Ļ
 * 
 * @param[in]  lcdid   ָ��������LCD����ֻ��1��LCD����Ϊ0
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdScreenOff(uint16 lcdid);


/**
 * ��ȡɫ��
 * 
 * @param[in]  lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[in]  bpp     ɫ��ֵ�������LCDBPP_T
 *                     RGB565һ������16bit,RGB888 һ������24bit
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdBppGet(uint32 lcdid, LCDBPP_T* bpp);

/**
 * LCD��������
 * 
 * @param[in] lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[in] value   LCD����ֵ,��Χ0-100
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdBrightnessSet(uint32 lcdid, uint32 value);

/**
 * LCD���Ȼ�ȡ
 * 
 * @param[in]  lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[out] *value  ��ȡLCD������ֵ������ʱĬ��Ϊ100
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdBrightnessGet(uint32 lcdid, uint32 *value);

/**
 * ����LCD��ת�Ƕ�
 * ����ڳ�ʼ���͵��ã�Ĭ������ת
 * 
 * @param[in]  lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[in]  Dir     LCD��ת�Ƕ�LCDBPP_T
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdDirectionSet(uint32 lcdid, LCDDIRECT_T Dir);

/**
 * ��ȡLCD��ת�Ƕ�
 * 
 * @param[in]  lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[out] Dir     ��ȡ����LCD��ת�Ƕ�
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdDirectionGet(uint32 lcdid, LCDDIRECT_T *Dir);

/**
 * LCDɫ������
 * 
 * @param[in] lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[in] value   LCD��ɫ��ֵ����λΪk
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdColortmpSet(uint32 lcdid, uint32 value);

/**
 * LCDɫ�»�ȡ
 * 
 * @param[in] lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[out] value  ��ȡLCD��ɫ��ֵ����λΪk
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdColortmpGet(uint32 lcdid, uint32 *value);


/**
 * LCD�Աȶ�����
 * 
 * @param[in] lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[in] value   LCD�ĶԱȶ�ֵ��ȡֵ��Χ0-100
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdContrastSet(uint32 lcdid, uint32 value);

/**
 * LCD�ԱȶȻ�ȡ
 * 
 * @param[in]  lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[out] *value  ��ȡLCD�ĶԱȶ�ֵ
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdContrastGet(uint32 lcdid, uint32 *value);


/**
 * LCDɫ������
 * 
 * @param[in] lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[in] value   LCD��ɫ��ֵ
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdHueSet(uint32 lcdid, uint32 value);

/**
 * LCDɫ��ֵ��ȡ
 * 
 * @param[in]  lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[out] *value  ��ȡLCD��ɫ��ֵ
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdHueGet(uint32 lcdid, uint32 *value);

/**
 * ��������ϵ�����
 * 
 * @param[in] lcdid   �޸��ĸ�LCD��Ӧ��canvas,LCDInitʱ����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdCanvasClear(uint32 lcdid);

/**
 * �ڻ��������ֱ��
 * 
 * @param[in] lcdid   �޸��ĸ�LCD��Ӧ��canvas,LCDInitʱ����
 * @param[in] rec �ṹ����xyΪ��ʼ�㣬����direction����ȷ��width��heightΪ����
 * @param[in] direction ֱ�ߵķ���
 * @param[in] color ֱ�ߵ���ɫ������KING_LcdBppGet��ȡ����Ϣ��ֵ
 * @param[in] transparence ͸���ȡ�0��͸�� 1͸��
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdDrawLine(LCDDISPLAY_T rec, LCDLAYOUT_T direction, uint32 color,uint32 lcdid,uint32 transparence);

/**
 * �ڻ��������ɫ��
 * 
 * @param[in] lcdid   �޸��ĸ�LCD��Ӧ��canvas,LCDInitʱ����
 * @param[in] rec ��ӵ�ɫ��ķ�Χ
 * @param[in] color ֱ�ߵ���ɫ������KING_LcdBppGet��ȡ����Ϣ��ֵ
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdFillRect(LCDDISPLAY_T rec, uint32 color,uint32 lcdid);

/**
 * �ѻ���������ʾ��LCD
 * 
 * @param[in] lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdShow(uint32 lcdid);

/**
 * LCD��ʾ
 * 
 * @param[in]  rec     ��ʾ��ʼλ�ü���ʾ�ĳ��ȣ����
 * @param[in]  data    ��������.����KING_LcdBppGet��ȡ����Ϣ��ֵ
 * @param[in]  clear   ���ǰ�Ƿ������Ļ
 * @param[in]  lcdid   ���ĸ�LCD������ʾ����ֻ��1��LCD����Ϊ0
 */
int KING_LcdDisplay(LCDDISPLAY_T rec, uint8* data, bool clear, uint32 lcdid);

/**
 * �ִ���ʾ
 * 
 * @param[in]  lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[in]  rec     �ִ���ʾ����widthΪ0��ʾ��ʾ����Ļ��Ե
 * @param[in]  para    �ִ�����ز���������ṹ��STR_PARA_S
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdDisplayStr(uint32 lcdid, LCDDISPLAY_T rec,  STR_PARA_S *para);


/**
 * ��������˳�sleep״̬
 * 
 * @param[in]  lcdid   �����ĸ�LCD����ֻ��1��LCD����Ϊ0
 * @param[in]  isSleep   0�˳�sleep״̬,1����sleep״̬
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_LcdSleep(uint32 lcdid, uint32 isSleep);

#endif

