#ifndef __KING_AUDIO_H__
#define __KING_AUDIO_H__

#include "KingDef.h"


typedef enum
{
    AUDIO_MIC = 0,
    AUDIO_SPEAK,
    AUDIO_TYPE_MAX = 0x7FFFFFFF
}AUDIO_CHANNEL_TYPE_E;

typedef enum
{
    MIC_CHANNEL_0 = 0,
    MIC_CHANNEL_1,
    MIC_CHANNEL_MAX = 0x7FFFFFFF
}MIC_CHANNEL_INDEX_E;

typedef enum
{
    SPEAK_HANDHOLD = 0,
    SPEAK_HANDFREE,
    SPEAK_EAR,
    SPEAK_CHANNEL_MAX = 0x7FFFFFFF
}SPEAK_CHANNEL_INDEX_E;


typedef enum
{
    AUDIO_STATE_IDLE = 0,
    AUDIO_STATE_BUSY,
    AUDIO_STATE_MAX = 0x7FFFFFFF
}AUDIO_STATE_E;


typedef enum
{
    AUDIO_PLAY_FILL_BUFF = 1,
    AUDIO_PLAY_STOP,
    AUDIO_RECORD_ONE_FRAME,
    AUDIO_HEADSET_PLUGIN,
    AUDIO_HEADSET_PLUGOUT,
    AUDIO_EVENT_MAX    = 0x7FFFFFFF
}AUDIO_EVENT_E;


typedef enum
{
    SAMPLING_RATE_8KHZ = 8000,
    SAMPLING_RATE_16KHZ = 16000,
    SAMPLING_RATE_32KHZ = 32000,
    SAMPLING_RATE_44DOT1KHZ = 44100,
    SAMPLING_RATE_MAX = 0x7FFFFFFF
}PCM_SAMPL_RATE_E;

typedef enum
{
    SAMPLING_FORMAT_S8 = 0,
    SAMPLING_FORMAT_S16_LE,
    SAMPLING_FORMAT_MAX = 0x7FFFFFFF
}PCM_SAMPL_FORMAT_E;

typedef enum
{
    CHANNEL_NUM_SINGLE = 1,
    CHANNEL_NUM_STERE0,
    CHANNEL_NUM_MAX = 0x7FFFFFFF
}PCM_CHANNEL_NUM_E;

typedef enum
{
    AUD_SPKPA_TYPE_CLASSAB,
    AUD_INPUT_TYPE_CLASSD,
    AUD_INPUT_TYPE_CLASSK,
    AUD_SPKPA_INPUT_TYPE_QTY = 0xFF000000
} AUD_SPKPA_TYPE_T;

typedef enum
{
    AUD_FORMAT_WAV,
    AUD_FORMAT_MP3,
    AUD_FORMAT_AMRNB,
    AUD_FORMAT_AMRWB,
    AUD_FORMAT_SBC,
    AUD_FORMAT_MAX = 0x7FFFFFFF
} AUD_STREAM_FORMAT_E;

typedef enum
{
    AUDEV_PLAY_TYPE_NONE = 0,
    AUDEV_PLAY_TYPE_LOCAL,
    AUDEV_PLAY_TYPE_VOICE,
    AUDEV_PLAY_TYPE_POC
} AUDEV_PLAY_TYPE_E;

typedef enum
{
    AUDEV_RECORD_TYPE_NONE = 0,
    AUDEV_RECORD_TYPE_MIC,
    AUDEV_RECORD_TYPE_VOICE,
    AUDEV_RECORD_TYPE_VOICE_DUAL,
    AUDEV_RECORD_TYPE_DEBUG_DUMP,
    AUDEV_RECORD_TYPE_POC,
} AUDEV_RECORD_TYPE_E;

typedef void (*AudioCallback)(AUDIO_EVENT_E event, uint8 *buff, uint32 *len);


/**
 * 设置音频通道
 * 
 * @param[in]  type        音频通道类型
 * @param[in]  channel     根据type值来选择MIC_CHANNEL_INDEX_E or SPEAK_CHANNEL_INDEX_E
 *                         具体channel信息请查看硬件规格书
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioChannelSet(AUDIO_CHANNEL_TYPE_E type, uint32 channel);

/**
 * 获取当前音频通道
 * 
 * @param[in]  type        音频通道类型
 * @param[out]  pChannel   根据type值返回MIC_CHANNEL_INDEX_E or SPEAK_CHANNEL_INDEX_E
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioChannelGet(AUDIO_CHANNEL_TYPE_E type, uint32* pChannel);

/**
 * 音量调节
 * 
 * @param[in]  type        音频通道类型
 * @param[in]  volume      音量百分比，取值范围0-100，对应0%-100%
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioVolumeSet(AUDIO_CHANNEL_TYPE_E type, uint32 volume);

/**
 * 获取当前音量
 * 
 * @param[in]  type        音频通道类型
 * @param[out]  pVolume    音量百分比，取值范围0-100，对应0%-100%
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioVolumeGet(AUDIO_CHANNEL_TYPE_E type, uint32* pVolume);

/**
 * 获取当前AUDIO状态
 * 
 * @param[out]  pStatus  当前audio状态
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioStatusGet(AUDIO_STATE_E* pStatus);

/**
 * 音频播放初始化
 * 
 * @return 0 SUCCESS,  -1 FAIL表示平台不支持
 */
int KING_AudioPlayInit(void);


/**
 * 音频播放反初始化
 * 
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioPlayDeinit(void);


/**
 * pcm数据流播放
 * 
 * @param[in]  sampleRate     播放PCM 数据时传文件采样率
 * @param[in]  sampleFormat   pcm数据采样位数，通常为16bit
 *                            UIS8910DM只支持SAMPLING_FORMAT_S16_LE
 * @param[in]  chnum          单声道或立体声
 * @param[in]  cb             回调函数,返回buff地址和buff长度，用户直接往buff copy数据
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioPcmStreamPlay(PCM_SAMPL_RATE_E sampleRate, PCM_SAMPL_FORMAT_E sampleFormat, PCM_CHANNEL_NUM_E chnum, AudioCallback cb);


/**
 * 立刻停止音频播放
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioStop(void);

/**
 * 停止音频播放，等待底包buffer中的数据播放完成
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioAbort(void);


/**
 * 录音初始化
 * 
 * @return 0 SUCCESS,  -1 FAIL表示平台不支持
 */
int KING_AudioRecordInit(void);


/**
 * 录音反初始化
 * 
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioRecordDeinit(void);

/**
 * 录音启动
 * 
 * @param[in]  sampleRate     采样频率
 * @param[in]  sampleFormat   量化位数，通常为16-bit
 *                            UIS8910DM只支持SAMPLING_FORMAT_S16_LE
 * @param[in]  chnum          单声道或立体声
 * @note UIS8910DM平台不支持录音参数配置，默认录音格式为PCM，sampleRate=8k，sampleFormat=16bit，chnum=1
 * @param[in]  cb  回调函数每录音录完一帧后将数据传回给app
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioRecordStart(PCM_SAMPL_RATE_E sampleRate, PCM_SAMPL_FORMAT_E sampleFormat, PCM_CHANNEL_NUM_E chnum, AudioCallback cb);


/**
 * audio 录音暂停
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioRecordPause(void);

/**
 * audio 录音继续
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioRecordResume(void);


/**
 * audio 录音停止
 * 
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioRecordStop(void);


/**
 * 初始化DTMF
 * 
 * @return 0 SUCCESS  -1 FAIL,当初始化返回SUCCESS时,代表当前平台支持DTMF功能,返回FAIL,代表平台不支持DTMF功能
 */

int KING_DtmfInit(void);


/**
 * 开始播放DTMF
 * @param[in]  pDtmfStr   当前要播放的DTMF字符信息,支持的字符范围包括'0'-'9','a'-'d','*','#'
 * @param[in]  nDuration  每个DTMF音的播放的时间(ms),范围(0-0xFFFFFFFF),
 *                        当nDuration赋值为0xFFFFFFFF时代表持续播放DTMF音,只有调用KING_DtmfStop接口才能停止播放
 * @param[in]  nInterval  两个DTMF音之间的播放时间间隔(ms),范围(0-0xFFFFFFFF)
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_DtmfPlay(const char *pDtmfStr,uint32 nDuration, uint32 nInterval);


/**
 * 停止播放DTMF
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_DtmfStop(void);


/**
 * 反初始化DTMF
 * 
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_DtmfDeInit(void);

/**
 * 设置audio speak pa type
 * @param[in]  type   speaker pa type，支持的pa类型见AUD_SPKPA_TYPE_T
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpkPaTypeSet(AUD_SPKPA_TYPE_T type);

/**
 * 获取audio speak pa type
 * @param[out]  type   speaker pa type，支持的pa类型见AUD_SPKPA_TYPE_T
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpkPaTypeGet(AUD_SPKPA_TYPE_T *type);

/**
 * mp3,wav等格式数据流播放
 * 
 * @param[in]  format     支持播放的音频数据流格式，见AUD_STREAM_FORMAT_E
 * @param[in]  cb         回调函数,返回buff地址和buff长度，用户直接往buff copy数据
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioStreamPlayExt(AUD_STREAM_FORMAT_E format, AudioCallback cb);

/**
 * mp3,wav等格式录音
 * 
 * @param[in]  format         支持录音的音频数据流格式，UIS8910DM只支持AUD_FORMAT_AMRNB和AUD_FORMAT_AMRWB
 * @param[in]  sampleRate     采样率,UIS8910DM平台的AMRNB只支持8K采样率，AMRWB只支持16K采样率
 * @param[in]  sampleFormat   pcm数据采样位数，通常为16bit
 *                            UIS8910DM只支持SAMPLING_FORMAT_S16_LE
 * @param[in]  chnum          单声道或立体声
 * @param[in]  fname          录制成的文件名称
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 */
int KING_AudioStreamRecordExt(AUD_STREAM_FORMAT_E format, PCM_SAMPL_RATE_E sampleRate, PCM_SAMPL_FORMAT_E sampleFormat, PCM_CHANNEL_NUM_E chnum, const char *fname);

/**
 * pcm数据流播放
 * 
 * @param[in]  sampleRate     播放PCM 数据时传文件采样率
 * @param[in]  sampleFormat   pcm数据采样位数，通常为16bit
 *                            UIS8910DM只支持SAMPLING_FORMAT_S16_LE
 * @param[in]  chnum          单声道或立体声
 * @param[in]  type           双工选择
 * @param[in]  cb             回调函数,返回buff地址和buff长度，用户直接往buff copy数据
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioPcmStreamPlayV2(PCM_SAMPL_RATE_E sampleRate, PCM_SAMPL_FORMAT_E sampleFormat, PCM_CHANNEL_NUM_E chnum, AUDEV_PLAY_TYPE_E type, AudioCallback cb);

/**
 * 录音启动
 * @param[in]  sampleRate     采样率
 * @param[in]  sampleFormat   pcm数据采样位数，通常为16bit
 *                            UIS8910DM只支持SAMPLING_FORMAT_S16_LE
 * @param[in]  chnum          单声道或立体声
 * @param[in]  type           双工选择
 * @param[in]  cb  回调函数每录音录完一帧后将数据传回给app
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioRecordStartV2(PCM_SAMPL_RATE_E sampleRate, PCM_SAMPL_FORMAT_E sampleFormat, PCM_CHANNEL_NUM_E chnum, AUDEV_RECORD_TYPE_E type, AudioCallback cb);

/**
 * 启动Poc模式
 * @param[in]  duplexmode     是否全双工
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioDevStartPocMode(bool duplexmode);

/**
 * 停止Poc模式
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioDevStopPocMode(void);

/**
 * Poc模式半双工切换
 * @param[in]  mode    1 - 录音, 2 - 播放
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioDevPocModeSwitch(uint8 mode);


#endif

