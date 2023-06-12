#ifndef __KING_TTS_H__
#define __KING_TTS_H__

#include "KingDef.h"

typedef enum
{
    TTS_PLAY_START = 1,
    TTS_PLAY_STOP,
    TTS_PLAY_MAX = 0x7FFFFFFF
}TTS_PLAY_E;

typedef void (*TtsCallback)(TTS_PLAY_E event, uint8 *buff, uint32 len);

typedef enum
{
    CODE_UCS2 = 0,
    CODE_GBK,
    CODE_TYPE_MAX = 0x7FFFFFFF
}TTS_CODE_TYPE_E;

typedef struct
{
    int16 volume; //������tts��������(-32768-32767) -32768����С������32767���������
    int16 role; // 0��Ů����1������
    int16 pitch; //���(-32768-32767) -32768����С�����32767��������
    int16 speed;  //����Χ(-32768-32767) -32768����С���٣�32767���������
    int16 readDigit; //��0-2��0���Զ���1�����������2������ֵ��
    int16 readalp; //��0-2��0���Զ���1������ĸ����2�������ʶ�
    int16 language; //��0-2��0���Զ���1�����ģ�2��Ӣ��
}TTS_PARAM_S;

/**
 * TTS��ʼ��
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_TtsInit(void);

/**
 * TTS����ʼ��
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_TtsDeinit(void);

/**
 * TTS��������
 * 
 * @param[in]  pParam      ���١������
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_TtsParamsSet(TTS_PARAM_S* pParam);

/**
 * TTS��ǰ������ȡ
 * 
 * @param[out]  pParam      ���١������
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_TtsParamsGet(TTS_PARAM_S* pParam);

/**
 * �����ı�
 * 
 * @param[in]  codeType    ��������
 * @param[in]  text        �ı�����(codeType����)
 * @param[in]  len         �ı�����
 * @param[in]  cb          �ص�����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_TtsPlay(TTS_CODE_TYPE_E codeType , uint8* text, uint32 len, TtsCallback cb);

/**
 * ֹͣ����
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_TtsStop(void);

/**
 * �Ƿ���ֹͣ����״̬
 * 
 * @return 1 ����ֹͣ״̬  0 ���ڲ���״̬
 */
int IsTtsStop(void);
#endif

