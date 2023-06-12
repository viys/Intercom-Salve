#include "app_main.h"
#include "uart_init.h"
#include "at.h"


static FS_HANDLE gFsHandle;
static uint8 audio_flag = 0;
static uint32 recordTime = 0;
FS_HANDLE AudioFileHandle = NULL;

int AudioChannelGet(char* data, uint32 dataLen)
{
    int ret;
    uint32 audio_type;
    uint32 audio_channel;

    char *paudio_type;
    char *pend;

    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;

    if (*data == '?')
    {
        ATPrintf("+AUDIOCHANNELGET: (0-1)\r\n");
        return SUCCESS;
    }

    paudio_type = strtok(data,",");
   
    pend = strtok(NULL, ",");
    //�����ж�
    if ((NULL == paudio_type) || (NULL != pend))
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }

    //ͨ������
    audio_type = atoi(paudio_type);
    if ((AUDIO_MIC != audio_type) && (AUDIO_SPEAK  != audio_type))
    {
        LogPrintf("audio type param error\r\n");
        return FAIL;
    }

    ret = KING_AudioChannelGet(audio_type, &audio_channel);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioChannelGet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    ATPrintf("AUDIO CHANNEL GET %d\r\n", audio_channel);
    return SUCCESS;
}

int AudioChannelSet(char* data, uint32 dataLen)
{
    int ret;
    uint32 audio_type;
    uint32 audio_channel;

    char *paudio_type;
    char *paudio_channel;
    char *pend;

    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;

    if (*data == '?')
    {
        ATPrintf("+AUDIOCHANNELSET: (0-1),(0-1)\r\n");
        return SUCCESS;
    }

    paudio_type = strtok(data,",");
    paudio_channel = strtok(NULL, ",");
    pend = strtok(NULL, ",");
    //�����ж�
    if((NULL == paudio_type) || (NULL == paudio_channel) || (NULL != pend))
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }

    //ͨ������
    audio_type = atoi(paudio_type);
    if((AUDIO_MIC != audio_type) && (AUDIO_SPEAK  != audio_type))
    {
        LogPrintf("audio type param error\r\n");
        return FAIL;
    }

    //ͨ��
    audio_channel = atoi(paudio_channel);
    if((0 != audio_channel) && (1 != audio_channel))
    {
        LogPrintf("audio channel param error\r\n");
        return FAIL;
    }
    
    ret = KING_AudioChannelSet(audio_type,audio_channel);
    if (FAIL == ret)
    {
        LogPrintf("KING_GpioModeSet() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    return SUCCESS;
}
/**
 *    ������С������Ĭ�ϣ� 
 *    
 *    @param[in]  data  
 *    @param[in]  dataLen  
 *    @return 0 SUCCESS  -1 FAIL
 */
int AtAudioVolume(char* data, uint32 dataLen)
{
    
    int ret = SUCCESS;
    uint32 audio_mode;
    uint32 volume_value; 

    char *paudio_mode;
    char *pvolume_value;
    char *pend;
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;

    if (*data == '?')
    {
        ATPrintf("+AUDIOVOLUME: (0-1),(0-100)\r\n");
        return SUCCESS;
    }

    paudio_mode = strtok(data,",");
    pvolume_value = strtok(NULL, ",");
    pend = strtok(NULL, ",");
    //�����ж�
    if((NULL == paudio_mode) || (NULL == pvolume_value) || (NULL != pend))
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }

    //ͨ���ж�
    audio_mode = atoi(paudio_mode);
    if((AUDIO_MIC != audio_mode) && (AUDIO_SPEAK != audio_mode))
    {
        LogPrintf("mode param error\r\n");
        return FAIL;
    }

    //������С�ж�
    volume_value = atoi(pvolume_value);
    if((volume_value > 100) || (volume_value < 0))
    {
        LogPrintf("volume param error\r\n");
        return FAIL;
    }

    //��������
    ret = KING_AudioVolumeSet(audio_mode,volume_value);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioVolumeSet() errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    
    volume_value = 0;
    ret = KING_AudioVolumeGet(audio_mode,&volume_value);
    if (FAIL == ret)
    {

        LogPrintf("KING_AudioVolumeGet() errcode=0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    LogPrintf("audio volume : %d\r\n",volume_value);
    return SUCCESS;
}
/**
 *    ���ź�¼���ص�����
 *    
 *    @param[in]  event   �ص��¼�  
 *    @param[in]  buff  ���Ż���¼��
 *    @param[in]  len   ���Ż���¼������
 *    @return 0 SUCCESS  -1 FAIL
 */
static void AudioCb(AUDIO_EVENT_E event, uint8 *buff, uint32 *len)
{
    int ret = SUCCESS;
    MSG_S msg;
    uint8 pcmdata[640] = {0};
    uint32 bytesRead = 0;
    static uint32 val = 0;
    switch(event)
    {
        case AUDIO_PLAY_FILL_BUFF:
        {
            ret = KING_FsFileRead(gFsHandle, pcmdata, 320, &bytesRead);
            if (FAIL == ret)
            {
                LogPrintf("KING_FsFileRead fail errorcode: 0x%X\r\n", KING_GetLastErrCode());
                KING_FsCloseHandle(gFsHandle);
                gFsHandle = 0;
                audio_flag = 0;
                KING_AudioStop();
                return;
            }
            memcpy(buff, pcmdata, bytesRead);
            *len = bytesRead;
            if(bytesRead <= 0)
            {
                KING_AudioStop();
                LogPrintf("stop\r\n");
                KING_FsCloseHandle(gFsHandle);
                gFsHandle = 0;
                audio_flag = 0;
                
                return;
            }
            break;
        }
        case AUDIO_PLAY_STOP:
        {
            audio_flag = 0;
            //LogPrintf("stop!\r\n");
            break;
        }
        case AUDIO_RECORD_ONE_FRAME:
        {
            val++;
            if(val <= recordTime)
            {
                memset(pcmdata, 0x00, 640);
                memcpy(pcmdata, buff, *len);
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = MSG_EVT_RECORD;
                msg.pData = pcmdata;
                msg.DataLen = *len;
                KING_MsgSend(&msg, msgHandle);
            }
            else
            {
                val = 0;
                *len = 0;
                KING_AudioRecordStop();
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = MSG_EVT_RECORD_STOP;
                KING_MsgSend(&msg, msgHandle);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

/**
 *    ¼��
 *    
 *    @param[in]  data  
 *    @param[in]  dataLen ���ݳ��� 
 *    @return 0 SUCCESS  -1 FAIL
 */

int AtRecord(char* data, uint32 dataLen)
{
    int ret = SUCCESS;
    char filename[50];
    char *pfilename;
    char *ptime;
    char *pend;
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;

    if (*data == '?')
    {
        ATPrintf("+RECORD: (file name),(1-n)\r\n");
        return SUCCESS;
    }
    
    pfilename = strtok(data,",");
    ptime = strtok(NULL,",");
    pend = strtok(NULL,",");
    if (NULL == pfilename || NULL == ptime || NULL != pend)
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }

    //�ļ���ȡ
    memset(filename,0x00,sizeof(filename));
    memcpy(filename,&data[1],strlen(data)-2); 
    //�ļ������ڴ��������ļ����ļ���������ļ�
    ret = KING_FsFileCreate(filename, FS_MODE_CREATE_ALWAYS | FS_MODE_READ | FS_MODE_WRITE, 0, 0, &AudioFileHandle);
    if (FAIL == ret)
    {
        LogPrintf("KING_FsFileCreate fail errorcode: 0x%X\r\n", KING_GetLastErrCode());
        return FAIL;
    }

    //¼��ʱ��
    recordTime = atoi(ptime);
    if (recordTime == 0)
    {
        LogPrintf("time param error\r\n");
        return FAIL;
    }
    LogPrintf("record file: %s time: %d\r\n",filename, recordTime);
    recordTime = recordTime * 50;
    
    ret = KING_AudioRecordStart(SAMPLING_RATE_8KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE,AudioCb);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioRecordStart fail errorcode: 0x%x\r\n", KING_GetLastErrCode());
        return FAIL;
    }
    
    return SUCCESS;
}


/**
 *  ����
 *    
 *    @param[in]  data  
 *    @param[in]  dataLen ���ݳ��� 
 *    @return 0 SUCCESS  -1 FAIL
 */

int AtAudioPlay(char* data, uint32 dataLen)
{
    int ret = SUCCESS;
    char filename[50];
    uint32 sampling_rate;

    char *pfilename;
    char *psampling;
    char *pend;

    //�ж��Ƿ񲥷�
    if(audio_flag)
    {
        return FAIL;
    }
    if ((0 == dataLen) || ('=' != *data))
    {
        return FAIL;
    }
    data++;

    if (*data == '?')
    {
        ATPrintf("+AUDIOPLAY: (file name),(8-16)\r\n");
        return SUCCESS;
    }

    //�����ж�
    pfilename = strtok(data,",");
    psampling = strtok(NULL,",");
    pend = strtok(NULL,",");
    if((NULL == pfilename) || (NULL == psampling) || (NULL != pend))
    {
        LogPrintf("param error\r\n");
        return FAIL;
    }

    sampling_rate = atoi(psampling);
    switch(sampling_rate)
    {
        case 8:sampling_rate = SAMPLING_RATE_8KHZ;break;
        case 16:sampling_rate = SAMPLING_RATE_16KHZ;break;
        default:LogPrintf("sampling param error\r\n");return FAIL;
    }
    
    //ȥ��ͷβ����
    memset(filename,0x00,sizeof(filename));
    memcpy(filename,&data[1],strlen(data)-2); 
    //�����ж�
    //�ļ�����
    ret = KING_FsFileCreate(filename, FS_MODE_OPEN_EXISTING | FS_MODE_READ | FS_MODE_WRITE, 0, 0, &gFsHandle);
    if (FAIL == ret)
    {
        LogPrintf("no %s file\r\n", data);
        return FAIL;
    }
    LogPrintf("play: %s sampling: %d \r\n",filename, sampling_rate);

    //����
    ret = KING_AudioPcmStreamPlay(sampling_rate, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, AudioCb);
    if (FAIL == ret)
    {
        LogPrintf("KING_AudioPcmStreamPlay fail errorcode: 0x%X\r\n", KING_GetLastErrCode());
        KING_FsCloseHandle(gFsHandle);
        gFsHandle = 0;
        return FAIL;
    }
    audio_flag = 1;
    
    return SUCCESS;
}
