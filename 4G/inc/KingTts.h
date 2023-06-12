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
    int16 volume; //仅调节tts引擎音量(-32768-32767) -32768：最小音量；32767：最大音量
    int16 role; // 0：女声；1：男声
    int16 pitch; //语调(-32768-32767) -32768：最小语调；32767：最大语调
    int16 speed;  //语速围(-32768-32767) -32768：最小语速；32767：最大语速
    int16 readDigit; //（0-2）0：自动；1：按号码读；2：按数值读
    int16 readalp; //（0-2）0：自动；1：按字母读；2：按单词读
    int16 language; //（0-2）0：自动；1：中文；2：英文
}TTS_PARAM_S;

/**
 * TTS初始化
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_TtsInit(void);

/**
 * TTS反初始化
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_TtsDeinit(void);

/**
 * TTS参数设置
 * 
 * @param[in]  pParam      语速、语调等
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_TtsParamsSet(TTS_PARAM_S* pParam);

/**
 * TTS当前参数获取
 * 
 * @param[out]  pParam      语速、语调等
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_TtsParamsGet(TTS_PARAM_S* pParam);

/**
 * 播放文本
 * 
 * @param[in]  codeType    编码类型
 * @param[in]  text        文本内容(codeType编码)
 * @param[in]  len         文本长度
 * @param[in]  cb          回调函数
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_TtsPlay(TTS_CODE_TYPE_E codeType , uint8* text, uint32 len, TtsCallback cb);

/**
 * 停止播放
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_TtsStop(void);

/**
 * 是否处于停止播放状态
 * 
 * @return 1 处于停止状态  0 处于播放状态
 */
int IsTtsStop(void);
#endif

