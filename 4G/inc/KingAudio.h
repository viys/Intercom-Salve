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
 * ������Ƶͨ��
 * 
 * @param[in]  type        ��Ƶͨ������
 * @param[in]  channel     ����typeֵ��ѡ��MIC_CHANNEL_INDEX_E or SPEAK_CHANNEL_INDEX_E
 *                         ����channel��Ϣ��鿴Ӳ�������
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioChannelSet(AUDIO_CHANNEL_TYPE_E type, uint32 channel);

/**
 * ��ȡ��ǰ��Ƶͨ��
 * 
 * @param[in]  type        ��Ƶͨ������
 * @param[out]  pChannel   ����typeֵ����MIC_CHANNEL_INDEX_E or SPEAK_CHANNEL_INDEX_E
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioChannelGet(AUDIO_CHANNEL_TYPE_E type, uint32* pChannel);

/**
 * ��������
 * 
 * @param[in]  type        ��Ƶͨ������
 * @param[in]  volume      �����ٷֱȣ�ȡֵ��Χ0-100����Ӧ0%-100%
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioVolumeSet(AUDIO_CHANNEL_TYPE_E type, uint32 volume);

/**
 * ��ȡ��ǰ����
 * 
 * @param[in]  type        ��Ƶͨ������
 * @param[out]  pVolume    �����ٷֱȣ�ȡֵ��Χ0-100����Ӧ0%-100%
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioVolumeGet(AUDIO_CHANNEL_TYPE_E type, uint32* pVolume);

/**
 * ��ȡ��ǰAUDIO״̬
 * 
 * @param[out]  pStatus  ��ǰaudio״̬
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioStatusGet(AUDIO_STATE_E* pStatus);

/**
 * ��Ƶ���ų�ʼ��
 * 
 * @return 0 SUCCESS,  -1 FAIL��ʾƽ̨��֧��
 */
int KING_AudioPlayInit(void);


/**
 * ��Ƶ���ŷ���ʼ��
 * 
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioPlayDeinit(void);


/**
 * pcm����������
 * 
 * @param[in]  sampleRate     ����PCM ����ʱ���ļ�������
 * @param[in]  sampleFormat   pcm���ݲ���λ����ͨ��Ϊ16bit
 *                            UIS8910DMֻ֧��SAMPLING_FORMAT_S16_LE
 * @param[in]  chnum          ��������������
 * @param[in]  cb             �ص�����,����buff��ַ��buff���ȣ��û�ֱ����buff copy����
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioPcmStreamPlay(PCM_SAMPL_RATE_E sampleRate, PCM_SAMPL_FORMAT_E sampleFormat, PCM_CHANNEL_NUM_E chnum, AudioCallback cb);


/**
 * ����ֹͣ��Ƶ����
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioStop(void);

/**
 * ֹͣ��Ƶ���ţ��ȴ��װ�buffer�е����ݲ������
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioAbort(void);


/**
 * ¼����ʼ��
 * 
 * @return 0 SUCCESS,  -1 FAIL��ʾƽ̨��֧��
 */
int KING_AudioRecordInit(void);


/**
 * ¼������ʼ��
 * 
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioRecordDeinit(void);

/**
 * ¼������
 * 
 * @param[in]  sampleRate     ����Ƶ��
 * @param[in]  sampleFormat   ����λ����ͨ��Ϊ16-bit
 *                            UIS8910DMֻ֧��SAMPLING_FORMAT_S16_LE
 * @param[in]  chnum          ��������������
 * @note UIS8910DMƽ̨��֧��¼���������ã�Ĭ��¼����ʽΪPCM��sampleRate=8k��sampleFormat=16bit��chnum=1
 * @param[in]  cb  �ص�����ÿ¼��¼��һ֡�����ݴ��ظ�app
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioRecordStart(PCM_SAMPL_RATE_E sampleRate, PCM_SAMPL_FORMAT_E sampleFormat, PCM_CHANNEL_NUM_E chnum, AudioCallback cb);


/**
 * audio ¼����ͣ
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioRecordPause(void);

/**
 * audio ¼������
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioRecordResume(void);


/**
 * audio ¼��ֹͣ
 * 
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioRecordStop(void);


/**
 * ��ʼ��DTMF
 * 
 * @return 0 SUCCESS  -1 FAIL,����ʼ������SUCCESSʱ,����ǰƽ̨֧��DTMF����,����FAIL,����ƽ̨��֧��DTMF����
 */

int KING_DtmfInit(void);


/**
 * ��ʼ����DTMF
 * @param[in]  pDtmfStr   ��ǰҪ���ŵ�DTMF�ַ���Ϣ,֧�ֵ��ַ���Χ����'0'-'9','a'-'d','*','#'
 * @param[in]  nDuration  ÿ��DTMF���Ĳ��ŵ�ʱ��(ms),��Χ(0-0xFFFFFFFF),
 *                        ��nDuration��ֵΪ0xFFFFFFFFʱ�����������DTMF��,ֻ�е���KING_DtmfStop�ӿڲ���ֹͣ����
 * @param[in]  nInterval  ����DTMF��֮��Ĳ���ʱ����(ms),��Χ(0-0xFFFFFFFF)
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_DtmfPlay(const char *pDtmfStr,uint32 nDuration, uint32 nInterval);


/**
 * ֹͣ����DTMF
 * 
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_DtmfStop(void);


/**
 * ����ʼ��DTMF
 * 
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_DtmfDeInit(void);

/**
 * ����audio speak pa type
 * @param[in]  type   speaker pa type��֧�ֵ�pa���ͼ�AUD_SPKPA_TYPE_T
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpkPaTypeSet(AUD_SPKPA_TYPE_T type);

/**
 * ��ȡaudio speak pa type
 * @param[out]  type   speaker pa type��֧�ֵ�pa���ͼ�AUD_SPKPA_TYPE_T
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_SpkPaTypeGet(AUD_SPKPA_TYPE_T *type);

/**
 * mp3,wav�ȸ�ʽ����������
 * 
 * @param[in]  format     ֧�ֲ��ŵ���Ƶ��������ʽ����AUD_STREAM_FORMAT_E
 * @param[in]  cb         �ص�����,����buff��ַ��buff���ȣ��û�ֱ����buff copy����
 * @return 0 SUCCESS  -1 FAIL
 */
int KING_AudioStreamPlayExt(AUD_STREAM_FORMAT_E format, AudioCallback cb);

/**
 * mp3,wav�ȸ�ʽ¼��
 * 
 * @param[in]  format         ֧��¼������Ƶ��������ʽ��UIS8910DMֻ֧��AUD_FORMAT_AMRNB��AUD_FORMAT_AMRWB
 * @param[in]  sampleRate     ������,UIS8910DMƽ̨��AMRNBֻ֧��8K�����ʣ�AMRWBֻ֧��16K������
 * @param[in]  sampleFormat   pcm���ݲ���λ����ͨ��Ϊ16bit
 *                            UIS8910DMֻ֧��SAMPLING_FORMAT_S16_LE
 * @param[in]  chnum          ��������������
 * @param[in]  fname          ¼�Ƴɵ��ļ�����
 * @return 0 SUCCESS  -1 FAIL
 * @attention require API Level 2
 */
int KING_AudioStreamRecordExt(AUD_STREAM_FORMAT_E format, PCM_SAMPL_RATE_E sampleRate, PCM_SAMPL_FORMAT_E sampleFormat, PCM_CHANNEL_NUM_E chnum, const char *fname);

/**
 * pcm����������
 * 
 * @param[in]  sampleRate     ����PCM ����ʱ���ļ�������
 * @param[in]  sampleFormat   pcm���ݲ���λ����ͨ��Ϊ16bit
 *                            UIS8910DMֻ֧��SAMPLING_FORMAT_S16_LE
 * @param[in]  chnum          ��������������
 * @param[in]  type           ˫��ѡ��
 * @param[in]  cb             �ص�����,����buff��ַ��buff���ȣ��û�ֱ����buff copy����
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioPcmStreamPlayV2(PCM_SAMPL_RATE_E sampleRate, PCM_SAMPL_FORMAT_E sampleFormat, PCM_CHANNEL_NUM_E chnum, AUDEV_PLAY_TYPE_E type, AudioCallback cb);

/**
 * ¼������
 * @param[in]  sampleRate     ������
 * @param[in]  sampleFormat   pcm���ݲ���λ����ͨ��Ϊ16bit
 *                            UIS8910DMֻ֧��SAMPLING_FORMAT_S16_LE
 * @param[in]  chnum          ��������������
 * @param[in]  type           ˫��ѡ��
 * @param[in]  cb  �ص�����ÿ¼��¼��һ֡�����ݴ��ظ�app
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioRecordStartV2(PCM_SAMPL_RATE_E sampleRate, PCM_SAMPL_FORMAT_E sampleFormat, PCM_CHANNEL_NUM_E chnum, AUDEV_RECORD_TYPE_E type, AudioCallback cb);

/**
 * ����Pocģʽ
 * @param[in]  duplexmode     �Ƿ�ȫ˫��
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioDevStartPocMode(bool duplexmode);

/**
 * ֹͣPocģʽ
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioDevStopPocMode(void);

/**
 * Pocģʽ��˫���л�
 * @param[in]  mode    1 - ¼��, 2 - ����
 * @return 0 SUCCESS  -1 FAIL
 */

int KING_AudioDevPocModeSwitch(uint8 mode);


#endif

