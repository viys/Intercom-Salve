#include "app_main.h"
#include "uart_init.h"
#include "at.h"

#define COLOR_RED       0XF800
#define COLOR_GREEN     0X07E0
#define COLOR_BLUE      0X001F
#define COLOR_BLACK     0X0000
#define COLOR_WHITE     0XFFFF

/**
 *    ���⿪���رգ�Ĭ�Ͽ����� 
 *    
 *    @param[in]  data  AT����֮��İ����Ⱥ��Լ��������ݣ�0/1��
 *    @param[in]  dataLen ���ݳ��� 
 *    @return 0 SUCCESS  -1 FAIL
 */

int AtBackCmd(char* data, uint32 dataLen)
{
    static int backflag = 1;
    int ret = SUCCESS;
    //�ж��Ƿ��в���
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    //ȥ�Ⱥ�
    data++;
    dataLen--;

    if (*data == '?')
    {
        ATPrintf("+BACKCMD: (0-1)\r\n");
        return SUCCESS;
    }

    //�жϲ���
    if (('0' != *data) && ('1' != *data))
    {
        LogPrintf("0/1 param error\r\n");
        return FAIL;
    }

    //�ж��Ƿ��ظ��������߹ر�
    if (('0'  == *data) && (1 == backflag))
    {
        
        ret =KING_LcdScreenOff(0);
        if (FAIL == ret)
        {

            LogPrintf("KING_LcdScreenOff() errcode=0x%x\r\n", KING_GetLastErrCode());
            return FAIL;
        }
    
        backflag = 0;
    }
    else if (('1'  == *data) && (0 == backflag))
    {
        ret =KING_LcdScreenOn(0);
        if(FAIL == ret)
        {

            LogPrintf("KING_LcdScreenOn() errcode=0x%x\r\n", KING_GetLastErrCode());
            return FAIL;
        }
        backflag = 1;
    }
    else
    {
        LogPrintf("repetition on-off error\r\n");
        return FAIL;
    }
    
    return SUCCESS;
}
/**
 *    Lcd �����ʾ
 *    
 *    @param[in]  rec   ��ʼ��������С
 *    @param[in]  pdata ������� 
 *    @param[in]  color ������� 
 *    @return 0 SUCCESS  -1 FAIL
 */

static int Lcd_Display(LCDDISPLAY_T rec,uint16 *pdata,uint16 color)
{
    int ret = SUCCESS;
    int i;
    i = 0;

    //ѭ�����
    while (i != (rec.width  * rec.height))
    {
        pdata[i] = color;
        i++;
    }
    //��ʾ
    ret = KING_LcdDisplay(rec,(uint8 *)pdata,1,0);    
    if(FAIL == ret)
    {

        LogPrintf("KING_LcdDisplay() Fail�� errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    KING_LcdShow(0);
    return ret;
}
/**
 *    lcd��ʾRGB+�ڰ�
 *    
 *    @param[in]  data  
 *    @param[in]  dataLen 
 *    @return 0 SUCCESS  -1 FAIL
 */

int AtDisplay(char* data, uint32 dataLen)
{
    int ret = SUCCESS;
    LCDDISPLAY_T rec;
    uint16 *pdata;

    if ((0 != dataLen) && (data[0] == '=') && (data[1] == '?'))
    {
        return SUCCESS;
    }
    
    if(0 != dataLen)
    {
        return FAIL;
    }
    rec.x = 0;
    rec.y = 0;
    rec.width = 240;
    rec.height = 320;
    ret = KING_MemAlloc((void **)&pdata, rec.width  * rec.height * sizeof(uint16));
    if (FAIL == ret)
    {
        LogPrintf("KING_MemAlloc() Fail�� errcode=0x%x\r\n", KING_GetLastErrCode());
        goto EXIT;
    }

    memset(pdata,0x00,rec.width  * rec.height * sizeof(uint16));

    //display
    ret = Lcd_Display(rec, pdata, COLOR_RED);
    if (FAIL == ret)
    {
        goto EXIT;
    }
    KING_Sleep(1000);

    Lcd_Display(rec, pdata, COLOR_GREEN);
    KING_Sleep(1000);

    Lcd_Display(rec, pdata, COLOR_BLUE);
    KING_Sleep(1000);

    Lcd_Display(rec, pdata, COLOR_BLACK);
    KING_Sleep(1000);

    Lcd_Display(rec, pdata, COLOR_WHITE);
    KING_Sleep(1000);
    ret = SUCCESS;
    
EXIT:
    KING_MemFree(pdata);
    pdata = NULL;
    
    return ret;
    
}

/**
 *    Lcd ������ڣ�Ĭ��100��
 *    
 *    @param[in]  data  ������ڷ�Χ0-100
 *    @param[in]  dataLen ���ݳ��� 
 *    @return 0 SUCCESS  -1 FAIL
 */
int AtLcdBack(char* data, uint32 dataLen)
{
    int ret = SUCCESS;
    int lcdBack_value; 
    
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;

    if (*data == '?')
    {
        ATPrintf("+LCDBACK: (0-100)\r\n");
        return SUCCESS;
    }

    //�����ж�
    lcdBack_value = atoi(data);
    if((lcdBack_value > 100) || (lcdBack_value < 0))
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }

    LogPrintf("Lcd Back Value : %d\r\n",lcdBack_value);

    //��������
    ret = KING_LcdBrightnessSet(0,lcdBack_value);
    if (FAIL == ret)
    {

        LogPrintf("KING_LcdScreenOff fail%x\r\n",KING_GetLastErrCode());
        return FAIL;
    }
    return SUCCESS;
}

