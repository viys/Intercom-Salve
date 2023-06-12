#include "KingDef.h"
#include "kingnet.h"
#include "kingcstd.h"
#include "KingCSTD.h"
#include "KingPlat.h"
#include "kingCbData.h"
#include "KingRtos.h"
#include "Kinguart.h"
#include "kingsocket.h"
#include "Kingdef.h"
#include "KingPowerManage.h"

#include "KingFileSystem.h"
#include "KingAudio.h"
#include "app_main.h"
 #define LogPrintf(fmt, args...) while(0)
// #define LogPrintf(fmt, args...)    do { KING_SysLog("[AudioTest] "fmt, ##args); } while(0)
#define TEST_DUPLEX                     1
#define TEST_HALFDUPLEX                 0
#define AUDIO_HALFDUPLEX_RECORD         1
#define AUDIO_HALFDUPLEX_PLAY           2
#define RECORD_TIME                     5
#define AUDIO_TEST_PLAY_FILE            "/mnt/sd/8.pcm"
#define AUDIO_TEST_RECORD_FILE          "/record1.pcm"
#define FS_TEST_DIR1    "/app"
#define FS_CALL_LOG     "/app/call_log.txt"
#define AUDIO_DATA_MAX 8000*5
#define PLAY_DATA_MAX 8000*5
void Uart2_Init(void);
void Uart3_Init(void);
void var_reset(void);
void HMI_Aktion(void);
void file_test(void);
void audio_stop_v2(void);
void audio_start_v2(void);
void Timer_Start(uint8 s);
extern unsigned char linear2alaw(int pcm_val);
extern int alaw2linear(unsigned char a_val);

uint8 is_files_c=0;
CALENDAR_TIME sysTime;
extern MSG_HANDLE   MP3_msgHandle;      //MP3
MSG_HANDLE msgHandle_audio;
extern MSG_HANDLE socketMsgHandle;

extern uint32 my_strcmp(uint8* val1,uint8* val2,uint32 line);
static TIMER_HANDLE  s_rcdtimer_heartbeat = NULL;
extern int s_socketFd;
extern uint8 socket_en;      //区分第一次连接
extern uint8 mail_list_tim; //是否获取过imei
// uint8 uart3_write[800]={0};

/*   录音缓存   */
uint8 audio_data[AUDIO_DATA_MAX]={0};   //发送缓存
uint32 audio_data_num=0;    //发送缓存到第几个
uint32 audio_data_new=0;     //产生新数据个数
uint32 audio_data_en=0;     //产生了几包数据
#define AUDIO_DATA_EN_MAX 80       //80个字节发送一次 和 播放一次

uint32 audio_data_play=0;   //播放到了第几个

/*   用户操作   */
uint8 call_user_operation=0;     //用户操作 1发出呼叫 2等待接通 3对方接通 4语音数据 5被拨号
uint8 mail_list_id=0;       //当前连接的 IMEI ID
uint8 mail_list_road=0;
uint8 mail_list_road1=0;
uint8 mail_list_road2=0;
uint8 mail_list_road3=0;
uint8 mail_list_road4=0;
uint8 Call_direction=0;     //呼叫方向1:向外 2:向内
uint8 call_user_tim=0;      //用户操作响应计时
extern uint8 is_host_busy; //主机是否 忙

/*   接收语音数据缓存   */
uint8 play_data[PLAY_DATA_MAX]={0};//接收缓存
uint32 play_data_num=0;    //接收缓存到第几个
uint32 play_data_new=0;     //产生新数据个数
uint32 play_data_en=0;     //产生了几包数据
#define PLAY_DATA_EN_MAX 320       //500个字节发送一次 和 播放一次
extern uint8 is_mail_list_ALL; //是否广播
/* ��ֵ������� */
const uint16 StepSizeTable[89] = { 7,8,9,10,11,12,13,14,16,17,
							19,21,23,25,28,31,34,37,41,45,
							50,55,60,66,73,80,88,97,107,118,
							130,143,157,173,190,209,230,253,279,307,
							337,371,408,449,494,544,598,658,724,796,
							876,963,1060,1166,1282,1411,1552,1707,1878,2066,
							2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,
							5894,6484,7132,7845,8630,9493,10442,11487,12635,13899,
							15289,16818,18500,20350,22385,24623,27086,29794,32767 };
/* ��ֵ�����±�仯����� */
const int8 IndexTable[16] = { 0xff,0xff,0xff,0xff,2,4,6,8,0xff,0xff,0xff,0xff,2,4,6,8 };


uint8 audio_en_v2=0;    //是否全双工

enum Msg_Evt
{
    MSG_EVT_BASE = 0,
    MSG_EVT_AUDIO_STOP,
    MSG_EVT_RECORD_STOP,
    MSG_EVT_RECORD,
    MSG_EVT_PLYRCD,
    MSG_EVT_PLYRCD_STOP,
    MSG_EVT_END,
    MSG_EVT_UART2_READ,
    MSG_EVT_UART3_READ,
    MSG_EVT_HMI_NEWS,
    MSG_EVT_UART3_SEND,
    MSG_EVT_HMI_NEWS2,
    MSG_EVT_TIME_5s,
    MSG_EVT_HMI_NEWS3,
    MSG_EVT_HMI_NEWS4,
};
enum  DIAL_REQ_REQ_CALL{     //主设备呼叫发送 & 被拨号请求
    DIAL_REQ_REQ_CALL_1  = 0x31,    //呼叫
    DIAL_REQ_REQ_CALL_2  ,      //取消
    DIAL_REQ_REQ_CALL_3  ,      //接通
    DIAL_REQ_REQ_CALL_4  ,      //挂断
};
enum Msg_Evt_MP3{
    Msg_Evt_MP3_NULL,
    Msg_Evt_MP3_AUDIO,
    Msg_Evt_MP3_STOP,
    Msg_Evt_MP3_STOP_AUDIO_START,
};
unsigned char ADPCM_Encode(int sample)
{
	// index �洢������һ��Ԥ��Ĳ�ֵ�����±�, ͨ��������Եõ�����
	static short  index = 0;
	// predsample �洢������һ������ֵ, ���뻹ԭʱ�����ľ������ֵ
	static int predsample = 0;
	// ��ǰ����ֵ, ���������, 4��bit
	uint8 code = 0;
	unsigned short tmpstep = 0;
	int diff = 0;
	int diffq = 0;
	unsigned short step = 0;

	// ���õ���ֵ����
	step = StepSizeTable[index];

	// ������ǰ����ֵ, ����һ�����ֵ�Ĳ�ֵ
	diff = sample - predsample;
	// ����Ǹ���, �͸�code �ĸ�4λ��1, ��ʾ��ֵ�Ǹ���
	if (diff < 0)
	{
		code = 8;
		diff = -diff;
	}

	tmpstep = step;

	// ���¸��ݲ�ֵ, ���㲽���ĳ���ϵ��(ͬʱ�ͻ���������±��ƫ����)
	// �����ǹ̶��� 1/8��step
	diffq = (step >> 3);

	// ������ǰ�λ���г���, ÿһλ�Ľ�������θ�ֵ��code��3,2,1λ, ͬʱpresample��ֵҲ�������
	if (diff >= tmpstep)
	{
		code |= 0x04;
		diff -= tmpstep;
		diffq += step;
	}

	tmpstep = tmpstep >> 1;

	if (diff >= tmpstep)
	{
		code |= 0x02;
		diff -= tmpstep;
		diffq += (step >> 1);
	}

	tmpstep = tmpstep >> 1;

	if (diff >= tmpstep)
	{
		code |= 0x01;
		diffq += (step >> 2);
	}
	// ����һ��, ���codeֵ���ڵ���4��С�ڵ���-4, ��˵����ֵ���ڵ�ǰ�Ĳ���(������1.125��), ����Ҫ����, ���򲽳�Ҫ����

	// ���¶��Ǳ���ֵԽ���һЩ����
	if (code & 8)
	{
		predsample -= diffq;
	}
	else
	{
		predsample += diffq;
	}

	if (predsample > 32767)
	{
		predsample = 32767;
	}
	else if (predsample < -32768)
	{
		predsample = -32768;
	}

	// ����õ���һ�����Ĳ�ֵ�����±�
	index += IndexTable[code];
	// ����Խ��
	if (index < 0)
	{
		index = 0;
	}
	else if (index > 88)
	{
		index = 88;
	}

	// ����Խ��
	return (code & 0x0f);
}
/**
  * @brief  ADPCM_Decode.
  * @param code: a byte containing a 4-bit ADPCM sample.
  * @retval : 16-bit ADPCM sample
  */
short ADPCM_Decode(unsigned char code)
{
	// ��һ��Ԥ��Ĳ�ֵ�����±�
	static short  index = 0;
	// ��һ���Ľ���ֵ
	static int predsample = 0;
	unsigned short step = 0;
	int diffq = 0;

	// �õ���ǰ����
	step = StepSizeTable[index];

	// ���ݲ�����4bit����ֵ, ���㵱ǰ��ʵ�ʲ�ֵ

	// ����1/8�Ĺ̶���ֵ
	diffq = step >> 3;

	// ��3λ, 1������
	if (code & 4)
	{
		diffq += step;
	}
	// ��2λ, 0.5������
	if (code & 2)
	{
		diffq += step >> 1;
	}
	// ��1λ, 0.25������
	if (code & 1)
	{
		diffq += step >> 2;
	}

	// ������������, �ӻ��߼�, ���������
	if (code & 8)
	{
		predsample -= diffq;
	}
	else
	{
		predsample += diffq;
	}

	// ��ֹԽ��
	if (predsample > 32767)
	{
		predsample = 32767;
	}
	else if (predsample < -32768)
	{
		predsample = -32768;
	}

	// ���, ����õ���һ����ֵ�������±�
	index += IndexTable[code];
	// ��ֹԽ��
	if (index < 0)
	{
		index = 0;
	}
	if (index > 88)
	{
		index = 88;
	}

	// ���ؽ�����
	return ((short)predsample);
}

void AudioPlayCb(AUDIO_EVENT_E event, uint8 *buff, uint32 *len)
{
    short pcma[160]={0};
    switch(event)
    {
        case AUDIO_PLAY_FILL_BUFF:
        {
            if(play_data_en){
                uint16 i;
                play_data_en--;
                short alaw;
                *len = PLAY_DATA_EN_MAX;


                for (int i = 0; i < 80; i += 1)
                {
                    short a = ADPCM_Decode(play_data[i+audio_data_play] & 0x0F);
                    short b = ADPCM_Decode((play_data[i+audio_data_play]>>4) & 0x0F);
                    pcma[i*2] = a;
                    pcma[i*2+1] = b;
                }
                // 打印语音数据
                //  LogPrintf("fdfdgfdgdfg,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                // pcma[16*2+1],
                // pcma[16*2+2],
                // pcma[16*2+3],
                // pcma[16*2+4],
                // pcma[16*2+5],
                // pcma[16*2+6],
                // pcma[16*2+7],
                // pcma[16*2+8],
                // pcma[16*2+9],
                // pcma[16*2+10],
                // pcma[16*2+11],
                // pcma[16*2+12]
                //  );
                memcpy(buff, pcma, PLAY_DATA_EN_MAX);
                audio_data_play+=PLAY_DATA_EN_MAX/4;
                if(audio_data_play>=PLAY_DATA_MAX){
                    audio_data_play-=PLAY_DATA_MAX;
                }
                LogPrintf("conso_audio_play_en:%d,all:%d",play_data_en,audio_data_play);
            }
            break;
        }
        case AUDIO_PLAY_STOP:
        {
            break;
        }

        default:
        {
            break;
        }
    }     
}
void AudioRecordCb(AUDIO_EVENT_E event, uint8 *buff, uint32 *len)
{
    MSG_S msg_audio;
    uint8 pcmdata[640] = {0};
    if(is_mail_list_ALL!=0){  //广播
        return;
    }
    switch(event)
    {
        case AUDIO_RECORD_ONE_FRAME:
        {
            MSG_S msg;           
            short audio_copy=0;
                uint16 b=0;
                uint32 writeLen=0;
                // 串口2发送原始数据
                // KING_UartWrite(UART_2, msg_audio.pData, msg_audio.DataLen, &writeLen);

                if((s_socketFd == -1) ||((call_user_operation==0)&&( !is_mail_list_ALL) )){
                    LogPrintf("QWEASDvdabdfhspz");
                    audio_stop_v2();
                    break;
                }
                if(is_mail_list_ALL!=0){  //广播
                    break;
                }
                for(uint16 a=0; a<*len/2; a++){
                    memcpy(&audio_copy,buff+a*2,2);

                    if ((a % 2) == 0) {
                        audio_data[audio_data_num+b]= ADPCM_Encode(audio_copy) & 0x0f;
                    }
                    else {
                        audio_data[audio_data_num+b] |= ((ADPCM_Encode(audio_copy) & 0x0f)<<4);
                        b++;
                    }
                }
                    // 串口2发送压缩后的原始数据
                    // KING_UartWrite(UART_2, audio_data+audio_data_num, 80, &writeLen);
                audio_data_num+=*len/4;
                if(audio_data_num>=AUDIO_DATA_MAX){ //循环计数
                    audio_data_num-=AUDIO_DATA_MAX;
                }

                audio_data_en++;
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = 125;
                KING_MsgSend(&msg, socketMsgHandle);

            break;
        }
        default:
        {
            break;
        }
    }
}
void Timer_heartbeat2(uint32 tmrId){
    MSG_S msg_audio;
    memset(&msg_audio, 0x00, sizeof(MSG_S));
    msg_audio.messageID = MSG_EVT_TIME_5s;
    // msg_audio.pData = pcmdata;
    // msg_audio.DataLen = *len;
    KING_MsgSend(&msg_audio, msgHandle_audio);
}

void AudioDuplexthread(void)
{
    // int ret = SUCCESS;
    MSG_S msg_audio;
    MSG_S msg_audio_mp3;
    MSG_S msg;
    LogPrintf("%s Enter", __FUNCTION__);
    Uart2_Init();
    Uart3_Init();
    KING_AudioPlayInit();
    KING_AudioRecordInit();
    KING_MsgCreate(&msgHandle_audio);
    KING_TimerCreate(&s_rcdtimer_heartbeat);
    
    KING_AudioChannelSet(AUDIO_MIC, 0);
    KING_AudioChannelSet(AUDIO_SPEAK, 1);
    KING_AudioVolumeSet(AUDIO_MIC, 100);
    KING_AudioVolumeSet(AUDIO_SPEAK, 100);

    // LogPrintf("KING_AudioDevStartPocMode ret=%d", ret);

	  //上电断开继电器
    memset(&msg_audio, 0x00, sizeof(MSG_S));
    msg_audio.messageID = MSG_EVT_UART3_SEND;
    msg_audio.pData = (uint8*)"cmd\x02\x00\x00\x00\x00\x00\x00";
    msg_audio.DataLen = 10;
    KING_MsgSend(&msg_audio, msgHandle_audio);
    while(1)
    {
    while(1)
    {
        memset(&msg_audio, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg_audio, msgHandle_audio, WAIT_OPT_INFINITE);
        switch (msg_audio.messageID)
        {
            case MSG_EVT_RECORD:      
            {
                short audio_copy=0;
                uint16 audio_copy2=0;
                uint16 b=0;
                uint32 writeLen=0;
                // 串口2发送原始数据
                // KING_UartWrite(UART_2, msg_audio.pData, msg_audio.DataLen, &writeLen);

                if((s_socketFd == -1) ||((call_user_operation==0)&&( !is_mail_list_ALL) )){
                    LogPrintf("QWEASDvdabdfhspz");
                    audio_stop_v2();
                    break;
                }
                if(is_mail_list_ALL!=0){  //广播
                    break;
                }
                for(uint16 a=0; a<msg_audio.DataLen/2; a++){
                    memcpy(&audio_copy,msg_audio.pData+a*2,2);

                    if ((a % 2) == 0) {
                        audio_data[audio_data_num+b]= ADPCM_Encode(audio_copy) & 0x0f;
                    }
                    else {
                        audio_data[audio_data_num+b] |= ((ADPCM_Encode(audio_copy) & 0x0f)<<4);
                        b++;
                    }
                }
                    // 串口2发送压缩后的原始数据
                    // KING_UartWrite(UART_2, audio_data+audio_data_num, 80, &writeLen);
                audio_data_num+=msg_audio.DataLen/4;
                if(audio_data_num>=AUDIO_DATA_MAX){ //循环计数
                    audio_data_num-=AUDIO_DATA_MAX;
                }

                audio_data_en++;
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = 125;
                KING_MsgSend(&msg, socketMsgHandle);
                break;
            }
            case MSG_EVT_UART3_READ:{
                if(mail_list_tim) //IMEI未获取
                    break;
                uint32 availableLen;
                uint32 readLen;
                uint8 temp[100];

                KING_UartGetAvailableBytes(UART_3,&availableLen);
                if(availableLen<2)
                    break;
                KING_UartRead(UART_3,temp, availableLen, &readLen);
                LogPrintf("MY_APP_UART3_%d:%d,%d,%d,%d,%d,%d,%d,%d,%d",readLen,temp[4],temp[5],temp[6],temp[7],temp[8],temp[9],temp[10],temp[11],temp[12]);

                if((temp[0]==0x53)&&(temp[1]==0x59)&&(temp[2]==0)&&(temp[3]==0)){
                    if(temp[4]==1){
                        if(is_host_busy){
                            // LogPrintf("asgsdgbsdfgb");
                            continue;
                        }
                        if(!call_user_operation){
                            call_user_operation=1;
        
                            memset(&msg_audio_mp3, 0x00, sizeof(MSG_S));
                            msg_audio_mp3.messageID = Msg_Evt_MP3_AUDIO;
                            msg_audio_mp3.pData=(char*)"/app/system/audio/wait_conv.mp3";
                            KING_MsgSend(&msg_audio_mp3, MP3_msgHandle);
                            
                            mail_list_road=temp[7]+0x30;

                            memset(&msg, 0x00, sizeof(MSG_S));
                            msg.messageID = 123;
                            KING_MsgSend(&msg, socketMsgHandle);

                            Timer_Start(6);
                            temp[0]=0;
                            temp[4]=0;
                            mail_list_road1=0;
                            mail_list_road2=0;
                            mail_list_road3=0;
                            mail_list_road4=0;
                        }else if(mail_list_road==temp[7]+0x30)
                        {

                        }
                        else{
                            if(temp[7]+0x30 != mail_list_road){
                                LogPrintf("zvzvzcvzxcvzvb2:%d\r\n", temp[7]);
                                if(temp[7]+0x30=='1')
                                if(mail_list_road1 == 0){
                                    mail_list_road1 = temp[7]+0x30 ;
                                    memset(&msg, 0x00, sizeof(MSG_S));
                                    msg.messageID = 127;
                                    KING_MsgSend(&msg, socketMsgHandle);                                
                                }
                                if(temp[7]+0x30=='2')
                                if(mail_list_road2 == 0){
                                    mail_list_road2 = temp[7]+0x30 ;
                                    memset(&msg, 0x00, sizeof(MSG_S));
                                    msg.messageID = 127;
                                    KING_MsgSend(&msg, socketMsgHandle);                                
                                }
                                if(temp[7]+0x30=='3')
                                if(mail_list_road3 == 0){
                                    mail_list_road3 = temp[7]+0x30 ;
                                    memset(&msg, 0x00, sizeof(MSG_S));
                                    msg.messageID = 127;
                                    KING_MsgSend(&msg, socketMsgHandle);                                
                                }
                                if(temp[7]+0x30=='4')
                                if(mail_list_road4 == 0){
                                    mail_list_road4 = temp[7]+0x30 ;
                                    memset(&msg, 0x00, sizeof(MSG_S));
                                    msg.messageID = 127;
                                    KING_MsgSend(&msg, socketMsgHandle);                                
                                }
                            }
                        }
                    }
                    else if((temp[4]==2)){	//报警信息上传
                        memset(&msg, 0x00, sizeof(MSG_S));
                        msg.messageID = 129;
                        msg.pData = temp+5;
                        KING_MsgSend(&msg, socketMsgHandle);
                    }
                    else if((temp[4]==3)){	//计数
                        memset(&msg, 0x00, sizeof(MSG_S));
                        msg.messageID = 131;
                        msg.pData = temp+5;
                        KING_MsgSend(&msg, socketMsgHandle);
                    }
                    else if((temp[4]==4)){	//继电器状态上传
                        memset(&msg, 0x00, sizeof(MSG_S));
                        msg.messageID = 132;
                        msg.pData = temp+5;
                        KING_MsgSend(&msg, socketMsgHandle);
                    }
                    }
                break;
            }
            case MSG_EVT_UART3_SEND:{   //发送串口数据·
                uint8 dat[2];
				memcpy(dat,msg_audio.pData+3,1);  // 2:接通挂断 4:登陆成功 5:新的一天 6：控制继电器
                LogPrintf("MY_APP_SOAO_UART3:%d,%d",msg_audio.DataLen,dat[0]);
                
                uint32 writeLen;
                
                KING_UartWrite(UART_3,msg_audio.pData, msg_audio.DataLen, &writeLen);

                break;
            }
            case MSG_EVT_HMI_NEWS:{
                Timer_Start(0);
                audio_data_num=0;    //发送缓存到第几个
                audio_data_new=0;     //产生新数据个数
                audio_data_en=0;     //产生了几包数据
                play_data_num=0;    //接收缓存到第几个
                play_data_new=0;     //产生新数据个数
                play_data_en=0;     //产生了几包数据
                audio_data_play=0;   //播放到了第几个
                audio_start_v2();
                break;
            }
            case MSG_EVT_HMI_NEWS2:{
                Timer_Start(0);
                audio_stop_v2();
                memset(&msg_audio_mp3, 0x00, sizeof(MSG_S));
                msg_audio_mp3.messageID = Msg_Evt_MP3_AUDIO;
                msg_audio_mp3.pData=(char*)"/app/system/audio/end_conv.mp3";
                KING_MsgSend(&msg_audio_mp3, MP3_msgHandle);
                break;
            }
            case MSG_EVT_TIME_5s:{
                memset(&msg_audio_mp3, 0x00, sizeof(MSG_S));
                msg_audio_mp3.messageID = Msg_Evt_MP3_AUDIO;
                msg_audio_mp3.pData=(char*)"/app/system/audio/wait_conv.mp3";
                KING_MsgSend(&msg_audio_mp3, MP3_msgHandle);
                break;
            }
            case MSG_EVT_HMI_NEWS3:{
                Timer_Start(0);
                audio_stop_v2();
                memset(&msg_audio_mp3, 0x00, sizeof(MSG_S));
                msg_audio_mp3.messageID = Msg_Evt_MP3_AUDIO;
                msg_audio_mp3.pData=(char*)"/app/system/audio/tim60_end.mp3";
                KING_MsgSend(&msg_audio_mp3, MP3_msgHandle);
                break;
            }
             case MSG_EVT_HMI_NEWS4:{
                Timer_Start(0);
                audio_stop_v2();
                memset(&msg_audio_mp3, 0x00, sizeof(MSG_S));
                msg_audio_mp3.messageID = Msg_Evt_MP3_AUDIO;
                msg_audio_mp3.pData=(char*)"/app/system/audio/busy.mp3";
                KING_MsgSend(&msg_audio_mp3, MP3_msgHandle);
                break;
             }
            default:
                break;
        }
    }
    LogPrintf("KING_Audioend");
    }
}
void Uart2EventCb(uint32 eventID, void* pData, uint32 len)
{
    MSG_S msg_audio;
    if (eventID != UART_EVT_DATA_TO_READ)return;
    memset(&msg_audio, 0x00, sizeof(MSG_S));
    msg_audio.messageID = MSG_EVT_UART2_READ;
    KING_MsgSend(&msg_audio, msgHandle_audio);
}
void Uart3EventCb(uint32 eventID, void* pData, uint32 len)
{
    MSG_S msg_audio;
    if (eventID != UART_EVT_DATA_TO_READ)return;
    memset(&msg_audio, 0x00, sizeof(MSG_S));
    msg_audio.messageID = MSG_EVT_UART3_READ;
    KING_MsgSend(&msg_audio, msgHandle_audio);
}
void Uart2_Init(void)
{
    int ret = SUCCESS;
    UART_CONFIG_S cfg;
    cfg.baudrate = UART_BAUD_230400;
    cfg.byte_size = UART_BYTE_LEN_8;
    cfg.flow_ctrl = UART_NO_FLOW_CONTROL;
    cfg.parity = UART_NO_PARITY;
    cfg.stop_bits = UART_ONE_STOP_BIT; 
    ret = KING_UartInit(UART_2, &cfg);
    if (FAIL == ret)
    {
        //LogPrintf("KING_UartInit() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_RegNotifyFun(DEV_CLASS_UART, UART_2, Uart2EventCb);
    if (FAIL == ret)
    {
        //LogPrintf("KING_RegNotifyFun() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());

    }
    //LogPrintf("MY_app_uart2_init");
    // KING_UartWrite(UART_2, pdata, readLen, &writeLen);
    // KING_UartRead(UART_2, pdata, availableLen, &readLen);
    // KING_UartGetAvailableBytes(UART_2,&availableLen);
}
void Uart3_Init(void)
{
    int ret = SUCCESS;
    UART_CONFIG_S cfg;
    cfg.baudrate = UART_BAUD_230400;
    cfg.byte_size = UART_BYTE_LEN_8;
    cfg.flow_ctrl = UART_NO_FLOW_CONTROL;
    cfg.parity = UART_NO_PARITY;
    cfg.stop_bits = UART_ONE_STOP_BIT; 
    ret = KING_UartInit(UART_3, &cfg);
    if (FAIL == ret)
    {
        //LogPrintf("KING_UartInit() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    ret = KING_RegNotifyFun(DEV_CLASS_UART, UART_3, Uart3EventCb);
    if (FAIL == ret)
    {
        //LogPrintf("KING_RegNotifyFun() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());

    }
}
void audio_start_v2(void){

    audio_en_v2=1;
    KING_AudioPcmStreamPlayV2(SAMPLING_RATE_8KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, AUDEV_PLAY_TYPE_POC, AudioPlayCb);
    KING_AudioRecordStartV2(SAMPLING_RATE_8KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, AUDEV_RECORD_TYPE_POC, AudioRecordCb);
    KING_AudioDevStartPocMode(TEST_DUPLEX);
}
void audio_stop_v2(void){
    if((audio_en_v2==1)||(is_mail_list_ALL==1)){
        KING_AudioDevStopPocMode();
        KING_AudioStop();
        audio_en_v2=0;       
    }

}
void Timer_Start(uint8 s){

    if(!s){
        KING_TimerDeactive(s_rcdtimer_heartbeat);
        return;
    }
    TIMER_ATTR_S timerattr;   
    memset(&timerattr, 0x00, sizeof(timerattr));
    timerattr.isPeriod= TRUE;
    timerattr.timeout = s*1000;
    timerattr.timer_fun = Timer_heartbeat2;
    KING_TimerActive(s_rcdtimer_heartbeat, &timerattr);

}
