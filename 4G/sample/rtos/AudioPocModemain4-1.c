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

#include "KingFileSystem.h"
#include "KingAudio.h"
#include "app_main.h"

#define LogPrintf(fmt, args...)    do { KING_SysLog("[AudioTest] "fmt, ##args); } while(0)
#define TEST_DUPLEX                     1
#define TEST_HALFDUPLEX                 0
#define AUDIO_HALFDUPLEX_RECORD         1
#define AUDIO_HALFDUPLEX_PLAY           2
#define RECORD_TIME                     5
#define AUDIO_TEST_PLAY_FILE            "/mnt/sd/8.pcm"
#define AUDIO_TEST_RECORD_FILE          "/record1.pcm"
#define FS_TEST_DIR1    "/app"
#define FS_CALL_LOG     "/app/call_log.txt"
void Uart2_Init(void);
void Uart3_Init(void);
void var_reset(void);
void HMI_Aktion(void);
void file_test(void);
// #define TEST_COUNT_MAX                  3
// static uint8 s_test_count = 0;
// static TIMER_HANDLE  s_rcdtimer_handle = NULL;
CALENDAR_TIME sysTime;
extern uint32 my_strcmp(uint8* val1,uint8* val2,uint32 line);

static uint8 uart_data_handle[256]={0x53,0x59,0x00,0x00,0x01};  //
static uint8 uart3_write[256];
#define PLAY_DATA_GROUP 10  //每十组数据播放一次
#define PLAY_DATA_MEMBER 500    //每组数据320字节
#define PLAY_DATA_MAX 8000*40
#define AUDIO_DATA_MAX 8000*40
#define AUDIO_MIC_TIME 10        //录音时间
extern uint32 Current_data_length;        //   帧数据长度
extern uint8 DangQianTongHua;   //开始呼叫 接听 挂断...
extern uint8 DangQianTongHua_back;
extern uint8 mail_list[256][16];    //记录的IMIEI
uint8 Call_direction=0;     //呼叫方向1:向外 2:向内
uint8 mail_list_uese_num=10;     //从机个数
uint8 mail_list_id=0;
uint8 mail_list_road=0;
uint8 play_data[AUDIO_DATA_MAX+4000]={0};//接收缓存
uint8 audio_data[PLAY_DATA_MAX+4000]={0};   //发送缓存
uint8 play_data_back[PLAY_DATA_MEMBER];

uint32 play_data_num=0;     //已经接收
uint32 play_data_num_back=0;     //已经播放

uint32 play_data_en=0;   //需要播放
uint32 new_data_num=0;    //接收新数据个数

uint32 audio_data_num=0;    //录音数据大小
uint32 audio_data_num_back=0;   //录音发送数据个数

uint32 new_audio_num=0;    //新数据个数
uint32 audio_data_en=0;   //数据标志

uint32 keyCodeGpio=0;
uint8 audio_start=0;  //0,关闭录音
uint8 HMI_Aktion_Index=0;   //屏幕操作索引
uint8 HMI_Aktion_Index_en=0;   //屏幕操作更新

enum CURRENT_CALL_STATUS{   //当前通话状态
    CURRENT_CALL_STATUS_0,  //
    CURRENT_CALL_STATUS_1,  //呼叫中
    CURRENT_CALL_STATUS_1_2,  //呼叫中：呼入
    CURRENT_CALL_STATUS_2,  //已接通
    CURRENT_CALL_STATUS_3,  //已接通
    CURRENT_CALL_STATUS_4,  //忙
    CURRENT_CALL_STATUS_5,  //拒绝//
    CURRENT_CALL_STATUS_6,  //挂断
    /* DangQianTongHua */
};
enum HMI_Aktion_Enum{   //屏幕操作
    HMI_Aktion_Enum_0 = 1,  //主拨号_呼叫       
    HMI_Aktion_Enum_1,  //主拨号_取消
    HMI_Aktion_Enum_2,  //主拨号_接通
    HMI_Aktion_Enum_3,  //主拨号_挂断
    HMI_Aktion_Enum_4,  //
    HMI_Aktion_Enum_5,   //主拨号返回_忙
    HMI_Aktion_Enum_6,   //主拨号返回_呼叫中
    HMI_Aktion_Enum_7,   //主拨号返回_被同意
    HMI_Aktion_Enum_8,   //主拨号返回_被拒绝
    HMI_Aktion_Enum_9,   //主拨号返回_已接通
    HMI_Aktion_Enum_10,   //主拨号返回_被挂断
    HMI_Aktion_Enum_11,  //
    HMI_Aktion_Enum_12,   //被拨号请求_呼叫
    HMI_Aktion_Enum_13,   //被拨号请求_取消
    HMI_Aktion_Enum_14,   //被拨号请求_已接通
    HMI_Aktion_Enum_15,   //被拨号请求_挂断
    HMI_Aktion_Enum_16,   //
    HMI_Aktion_Enum_17,   //被拨号返回_忙
    HMI_Aktion_Enum_18,   //被拨号返回_呼叫中
    HMI_Aktion_Enum_19,   //被拨号返回_统一
    HMI_Aktion_Enum_20,   //被拨号返回_拒绝
    HMI_Aktion_Enum_21,   //被拨号返回_拒绝
    HMI_Aktion_Enum_22,   //被拨号返回_接通
    HMI_Aktion_Enum_23,   //被拨号返回_挂断
    HMI_Aktion_Enum_24,   //
    HMI_Aktion_Enum_25,   //主拨号:无网络
};
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
};
static MSG_HANDLE msgHandle;
// static FS_HANDLE PlayFileHandle;
// static FS_HANDLE RecordFileHandle;

void AudioPlayCb(AUDIO_EVENT_E event, uint8 *buff, uint32 *len)
{
    // int ret = SUCCESS;
    MSG_S msg;
    uint8 i;
    uint8 pcmdata[640] = {0};
    // uint32 bytesRead = 0;
    //LogPrintf("%s:evnet:%d", __FUNCTION__, event);
    switch(event)
    {
        case AUDIO_PLAY_FILL_BUFF:
        {
            if(play_data_en){
                play_data_en--;
                *len = PLAY_DATA_MEMBER;
                for(i=0;i<PLAY_DATA_MEMBER/2;i++){
                    pcmdata[i*2+1]=play_data[play_data_num_back+i];
                }
                memcpy(buff, pcmdata, PLAY_DATA_MEMBER);

                play_data_num_back+=PLAY_DATA_MEMBER/2;
                if(play_data_num_back>=AUDIO_DATA_MAX){
                    play_data_num_back-=AUDIO_DATA_MAX;
                }
                LogPrintf("MY_APP_audio_package_%d",play_data_en);
            }else{
                play_data_num_back=0;
            }
            break;
        }
        case AUDIO_PLAY_STOP:
        {
            //LogPrintf("AUDIO_PLAY_STOP!\r\n");
            memset(&msg, 0x00, sizeof(MSG_S));
            msg.messageID = MSG_EVT_BASE;
            KING_MsgSend(&msg, msgHandle);
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
    MSG_S msg;
    uint8 pcmdata[640] = {0};
    // static uint32 val = 0;
    //LogPrintf("%s:evnet:%d", __FUNCTION__, event);
    switch(event)
    {
        case AUDIO_RECORD_ONE_FRAME:
        {
            // val++;
            // if(val <= (RECORD_TIME * 50))
            // {
                memset(pcmdata, 0x00, 640);
                memcpy(pcmdata, buff, *len);
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = MSG_EVT_RECORD;
                msg.pData = pcmdata;
                msg.DataLen = *len;
                KING_MsgSend(&msg, msgHandle);

            // }
            // else
            // {
            //     val = 0;
            //     *len = 0;
            //     KING_AudioRecordStop();
            //     memset(&msg, 0x00, sizeof(MSG_S));
            //     msg.messageID = MSG_EVT_RECORD_STOP;
            //     KING_MsgSend(&msg, msgHandle);
            // }
            break;
        }

        default:
        {
            break;
        }
    }
}

void AudioDuplexTest(void)
{
    int ret = SUCCESS;
    MSG_S msg;
    //LogPrintf("%s Enter", __FUNCTION__);
    Uart2_Init();
    Uart3_Init();
    ret = KING_AudioPlayInit();
    //LogPrintf("KING_AudioPlayInit ret=%d",ret);
    ret = KING_AudioRecordInit();
    //LogPrintf("KING_AudioRecordInit ret=%d",ret);

    // file_test();
    ret = KING_MsgCreate(&msgHandle);

    KING_AudioChannelSet(AUDIO_MIC, 0);
    KING_AudioChannelSet(AUDIO_SPEAK, 1);
    KING_AudioVolumeSet(AUDIO_MIC, 10);
    KING_AudioVolumeSet(AUDIO_SPEAK, 70);
    //
    KING_AudioPcmStreamPlayV2(SAMPLING_RATE_8KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, AUDEV_PLAY_TYPE_POC, AudioPlayCb);
    KING_AudioRecordStartV2(SAMPLING_RATE_8KHZ, SAMPLING_FORMAT_S16_LE, CHANNEL_NUM_SINGLE, AUDEV_RECORD_TYPE_POC, AudioRecordCb);

    KING_AudioDevStartPocMode(TEST_DUPLEX);
    LogPrintf("KING_AudioDevStartPocMode ret=%d", ret);

    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        KING_MsgRcv(&msg, msgHandle, WAIT_OPT_NO_WAIT);
        KING_Sleep(1);
        
        switch (msg.messageID)
        {
            case MSG_EVT_AUDIO_STOP:
            {
                //LogPrintf("stop!\r\n");
                // KING_FsCloseHandle(PlayFileHandle);
                
                break;
            }
            case MSG_EVT_RECORD:      
            {
                // uint32 bytesWrite = 1;
                uint32 i;
                uint32 j;
                uint32 k;
                if(audio_start){
                    j=msg.DataLen/2;
                    if( audio_data_num+j > PLAY_DATA_MAX){
                        j=PLAY_DATA_MAX-audio_data_num;
                        for(i=0; i<j ;i++){
                            memcpy(audio_data+audio_data_num+i ,msg.pData+i*2+1, 1);
                        }
                        j=msg.DataLen/2-j;
                        for(k=0; k<j ;k++){
                            memcpy(audio_data+k ,msg.pData+i*2+1, 1);
                            i++;
                        }
                    }else{
                       for(i=0; i< msg.DataLen/2;i++){
                         memcpy(audio_data+audio_data_num+i ,msg.pData+i*2+1, 1);
                        } 
                    }

                    // for(i=0; i< msg.DataLen/2;i++){
                    //     memcpy(audio_data+audio_data_num+i ,msg.pData+i*2+1, 1);
                    // }
                    audio_data_num+=msg.DataLen/2;
                    if(audio_data_num>=PLAY_DATA_MAX){
                        audio_data_num-=PLAY_DATA_MAX;
                    }
                    // if(audio_data_num>=20*8000){
                    //     // KING_UartWrite(UART_2,audio_data,audio_data_num, &bytesWrite);
                    //     KING_AudioRecordStop();
                    // }

                    new_audio_num+=msg.DataLen/2;

                    if(new_audio_num>=4000){    //一秒
                        audio_data_en+=new_audio_num/4000;
                        new_audio_num=new_audio_num%4000;
                    }
                    LogPrintf("MY_APP_audio__%d_%d",audio_data_num,audio_data_en);                    
                }
                break;
            }
            case MSG_EVT_RECORD_STOP:
            {
                break;
            }
            case MSG_EVT_PLYRCD://²¥·ÅÂ¼ÖÆµÄPCMÎÄ¼þ
            {
               break;
            }
            case MSG_EVT_PLYRCD_STOP:
            {

                break;
            }
            case MSG_EVT_UART2_READ:{
                break;
            }
            case MSG_EVT_UART3_READ:{
                uint32 availableLen;
                uint32 readLen;
                uint8 temp1[100];
                LogPrintf("uart3_dat");
                KING_UartGetAvailableBytes(UART_3,&availableLen);
                if(availableLen < 100){
                    KING_UartRead(UART_3,temp1, availableLen, &readLen);
                    LogPrintf("case_");
                    if((temp1[0]==0x53)&&(temp1[1]==0x59)&&(temp1[2]==0)&&(temp1[3]==0)&&(temp1[4]==1)){
                        uint32 writeLen;
                        uint8 i;
                        LogPrintf("case___");
                        switch (temp1[5]){
                            case 0x01:{    //获取时间
                                ret = KING_LocalTimeGet(&sysTime);
                                if (ret == SUCCESS)
                                {
                                    LogPrintf("rtc_localTimeGet %02x/%02x/%02x %02x:%02x:%x%x %d%d\r\n",
                                    sysTime.yy, sysTime.MM, sysTime.dd,
                                    sysTime.hh, sysTime.mm, sysTime.ss_h,
                                    sysTime.ss_l, sysTime.ew, sysTime.tz);
                                }
                                uart3_write[0]= 'r';
                                uart3_write[1]= 't';
                                uart3_write[2]= 'c';
                                uart3_write[3]= '0';
                                uart3_write[4]= '=';
                                uart3_write[5]= '2';
                                uart3_write[6]= '0';
                                uart3_write[7]= ((sysTime.yy>>4)+0x30);
                                uart3_write[8]= ((sysTime.yy&0x0f)+0x30);
                                uart3_write[9]= 0xff;
                                uart3_write[10]= 0xff;
                                uart3_write[11]= 0xff;
                                for(i=0;i<5;i++){
                                    uart3_write[i*10+12]='r';
                                    uart3_write[i*10+12+1]='t';
                                    uart3_write[i*10+12+2]='c';
                                    uart3_write[i*10+12+3]=i+1+0x30;
                                    uart3_write[i*10+12+4]='=';
                                    uart3_write[i*10+12+7]=0xff;
                                    uart3_write[i*10+12+8]=0xff;
                                    uart3_write[i*10+12+9]=0xff;
                                }
                                uart3_write[0*10+12+5]= ((sysTime.MM>>4)+0x30);
                                uart3_write[0*10+12+6]= ((sysTime.MM&0x0f)+0x30);
                                uart3_write[1*10+12+5]= ((sysTime.dd>>4)+0x30);
                                uart3_write[1*10+12+6]= ((sysTime.dd&0x0f)+0x30);
                                uart3_write[2*10+12+5]= ((sysTime.hh>>4)+0x30);
                                uart3_write[2*10+12+6]= ((sysTime.hh&0x0f)+0x30);
                                uart3_write[3*10+12+5]= ((sysTime.mm>>4)+0x30);
                                uart3_write[3*10+12+6]= ((sysTime.mm&0x0f)+0x30);
                                uart3_write[4*10+12+5]= ((sysTime.ss_h)+0x30);
                                uart3_write[4*10+12+6]= ((sysTime.ss_l)+0x30);
                                
                                KING_UartWrite(UART_3, uart3_write, 84, &writeLen);
                                break;
                            }
                            case 0x02:{     //获取主机下所有设备
                                break;
                            }
                            case 3:{        //巡检

                                break;
                            }
                            case 5:{        //拨号
                                LogPrintf("MY_app_temp1[5]_5");
                                if(DangQianTongHua!=CURRENT_CALL_STATUS_0)
                                    break;
                                mail_list_id=temp1[6];
                                mail_list_road=temp1[7]+0x30;   
                                Call_direction=1;
                                DangQianTongHua=CURRENT_CALL_STATUS_1;  //呼叫中
                                DangQianTongHua_back++;
                                break;
                            }
                            case 6:{        //屏幕按下按钮挂断
                                LogPrintf("MY_app_temp1[5]_6");
                                if(Call_direction==0)       //呼叫方向
                                    break;
                                var_reset();
                                DangQianTongHua=CURRENT_CALL_STATUS_6;      //挂断
                                DangQianTongHua_back++;
                                break;
                            }
                            case 7:{        //来电信息

                                break;
                            }
                            case 9:{        //屏幕按下接通按钮
                                LogPrintf("MY_app_temp1[5]_9");
                                if(Call_direction!=2)  //
                                    break;
                                var_reset();
                                audio_start=AUDIO_MIC_TIME;
                                DangQianTongHua=CURRENT_CALL_STATUS_1_2;      //通话(之前)
                                DangQianTongHua_back++;
                                HMI_Aktion_Index=HMI_Aktion_Enum_14;        //被拨号请求_已接通
                                HMI_Aktion_Index_en++;
                                break;
                            }
                            case 10:{//通话记录
                                LogPrintf("MY_app_temp1[5]_10");

                            }
                            default:
                                break;
                        }
                    }
                }
                KING_UartPurge(UART_3);
                break;
            }
            default:
                break;
        }
        HMI_Aktion();
    }
}
void Uart2EventCb(uint32 eventID, void* pData, uint32 len)
{
    MSG_S msg;
    if (eventID != UART_EVT_DATA_TO_READ)return;
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = MSG_EVT_UART2_READ;
    KING_MsgSend(&msg, msgHandle);
}
void Uart3EventCb(uint32 eventID, void* pData, uint32 len)
{
    MSG_S msg;
    if (eventID != UART_EVT_DATA_TO_READ)return;
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = MSG_EVT_UART3_READ;
    KING_MsgSend(&msg, msgHandle);
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
void var_reset(void)
{
 play_data_num=0;     //已经接收
 play_data_num_back=0;     //已经播放

 play_data_en=0;   //需要播放
 new_data_num=0;    //接收新数据个数

 audio_data_num=0;    //录音数据大小
 audio_data_num_back=0;   //录音发送数据个数

 new_audio_num=0;    //新数据个数
 audio_data_en=0;   //数据标志

 keyCodeGpio=0;
 audio_start=0;  //0,关闭录音
}
void HMI_Aktion(void){      //处理屏幕数据
    if(HMI_Aktion_Index_en){
        uint32 writeLen;
        uint32 len;
        HMI_Aktion_Index_en--;
        LogPrintf("MY_APP_HMI_Aktion_Index_%d",HMI_Aktion_Index-1);
        switch(HMI_Aktion_Index){
            case 0: break;
            case HMI_Aktion_Enum_0:{    //主拨号_呼叫
                len= strlen("b.t4.txt=\"正在呼叫\"");
                memcpy(uart3_write,"b.t4.txt=\"正在呼叫\"",len);
                uart3_write[len]= 0xff;
                uart3_write[len+1]= 0Xff;
                uart3_write[len+2]= 0Xff;
                KING_UartWrite(UART_3, uart3_write, len+3, &writeLen);
                break;
            }
            case HMI_Aktion_Enum_1:{    //主拨号_取消
                break;
            }
            case HMI_Aktion_Enum_2:{    //主拨号_接通
                break;
            }
            case HMI_Aktion_Enum_3:{    //主拨号_挂断
                len= strlen("b.t4.txt=\"通话结束\"");
                memcpy(uart3_write,"b.t4.txt=\"通话结束\"",len);
                uart3_write[len]= 0xff;
                uart3_write[len+1]= 0Xff;
                uart3_write[len+2]= 0Xff;
                KING_UartWrite(UART_3, uart3_write, len+3, &writeLen);
                
                audio_start=0;
                break;
            }
            case HMI_Aktion_Enum_4:{    //
                break;
            }
            case HMI_Aktion_Enum_5:{     //主拨号返回_忙
                break;
            }
            case HMI_Aktion_Enum_6:{     //主拨号返回_呼叫中
                len= strlen("b.t4.txt=\"等待接听\"");
                memcpy(uart3_write,"b.t4.txt=\"等待接听\"",len);
                uart3_write[len]= 0xff;
                uart3_write[len+1]= 0Xff;
                uart3_write[len+2]= 0Xff;
                KING_UartWrite(UART_3, uart3_write, len+3, &writeLen);
                break;
            }
            case HMI_Aktion_Enum_7:{     //主拨号返回_被同意
                case_HMI_Aktion_Enum_7:
                var_reset();
                audio_start=AUDIO_MIC_TIME;     //启动录音

                len= strlen("b.t4.txt=\"通话中\"");
                memcpy(uart3_write,"b.t4.txt=\"通话中\"",len);
                uart3_write[len]= 0xff;
                uart3_write[len+1]= 0Xff;
                uart3_write[len+2]= 0Xff;
                KING_UartWrite(UART_3, uart3_write, len+3, &writeLen);

                len= strlen("b.link_tim.en=1");
                memcpy(uart3_write,"b.link_tim.en=1",len);
                uart3_write[len]= 0xff;
                uart3_write[len+1]= 0Xff;
                uart3_write[len+2]= 0Xff;
                KING_UartWrite(UART_3, uart3_write, len+3, &writeLen);
                break;
            }
            case HMI_Aktion_Enum_8:{     //主拨号返回_被拒绝
                case_HMI_Aktion_Enum_8:
                LogPrintf("MY_APP_HMI_Aktion_Index_08");
                len= strlen("b.t4.txt=\"被挂断\"");
                memcpy(uart3_write,"b.t4.txt=\"被挂断\"",len);
                uart3_write[len]= 0xff;
                uart3_write[len+1]= 0Xff;
                uart3_write[len+2]= 0Xff;
                KING_UartWrite(UART_3, uart3_write, len+3, &writeLen);
                var_reset();
                break;
            }
            case HMI_Aktion_Enum_9:{     //主拨号返回_已接通
                goto case_HMI_Aktion_Enum_7;
                break;
            }
            case HMI_Aktion_Enum_10:{      //主拨号返回_被挂断
                goto case_HMI_Aktion_Enum_8;
                break;
            }
            case HMI_Aktion_Enum_11:{     //
                LogPrintf("MY_APP_HMI_Aktion_Index_11");
                break;
            }
            case HMI_Aktion_Enum_12:{      //被拨号请求_呼叫
                LogPrintf("MY_APP_HMI_Aktion_Index_12_ID:%d",mail_list_id);
                
                if(DangQianTongHua==CURRENT_CALL_STATUS_1){     //呼叫中
                    len= strlen("b.t4.txt=\"有新的呼入\"");
                    memcpy(uart3_write,"b.t4.txt=\"有新的呼入\"",len);
                    uart3_write[len]= 0xff;
                    uart3_write[len+1]= 0Xff;
                    uart3_write[len+2]= 0Xff;
                    KING_UartWrite(UART_3, uart3_write, len+3, &writeLen);  
                }else{
                    len= strlen("b.t4.txt=\"正在呼入\"");
                    memcpy(uart3_write,"b.t4.txt=\"正在呼入\"",len);
                    uart3_write[len]= 0xff;
                    uart3_write[len+1]= 0Xff;
                    uart3_write[len+2]= 0Xff;
                    KING_UartWrite(UART_3, uart3_write, len+3, &writeLen);                    
                }


                len= strlen("b.id.val=");
                memcpy(uart3_write,"b.id.val=",len);
                uart3_write[len]= mail_list_id+0x30;
                uart3_write[len+1]= 0Xff;
                uart3_write[len+2]= 0Xff;
                uart3_write[len+3]= 0Xff;
                
                KING_UartWrite(UART_3, uart3_write, len+4, &writeLen);

                len= strlen("b.pass.val=");
                memcpy(uart3_write,"b.pass.val=",len);
                uart3_write[len]= mail_list_road;
                uart3_write[len+1]= 0Xff;
                uart3_write[len+2]= 0Xff;
                uart3_write[len+3]= 0Xff;
                KING_UartWrite(UART_3, uart3_write, len+4, &writeLen);

                len= strlen("page b");
                memcpy(uart3_write,"page b",len);
                uart3_write[len]= 0xff;
                uart3_write[len+1]= 0Xff;
                uart3_write[len+2]= 0Xff;
                KING_UartWrite(UART_3, uart3_write, len+3, &writeLen);
                break;
            }
            case HMI_Aktion_Enum_13:{      //被拨号请求_取消
                LogPrintf("MY_APP_HMI_Aktion_Index_13");
                break;
            }
            case HMI_Aktion_Enum_14:{      //被拨号请求_已接通
                LogPrintf("MY_APP_HMI_Aktion_Index_14");
                len= strlen("b.t4.txt=\"通话中\"");
                memcpy(uart3_write,"b.t4.txt=\"通话中\"",len);
                uart3_write[len]= 0xff;
                uart3_write[len+1]= 0Xff;
                uart3_write[len+2]= 0Xff;
                KING_UartWrite(UART_3, uart3_write, len+3, &writeLen);
                len= strlen("b.link_tim.en=1");
                memcpy(uart3_write,"b.link_tim.en=1",len);
                uart3_write[len]= 0xff;
                uart3_write[len+1]= 0Xff;
                uart3_write[len+2]= 0Xff;
                KING_UartWrite(UART_3, uart3_write, len+3, &writeLen);

                break;
            }
            case HMI_Aktion_Enum_15:{      //被拨号请求_挂断
                LogPrintf("MY_APP_HMI_Aktion_Index_15");
                break;
            }
            case HMI_Aktion_Enum_16:{      //
                LogPrintf("MY_APP_HMI_Aktion_Index_16");
                break;
            }
            case HMI_Aktion_Enum_17:{      //被拨号返回_忙
                LogPrintf("MY_APP_HMI_Aktion_Index_17");

                break;
            }
            case HMI_Aktion_Enum_18:{      //被拨号返回_呼叫中
                LogPrintf("MY_APP_HMI_Aktion_Index_18");
                break;
            }
            case HMI_Aktion_Enum_19:{      //被拨号返回_统一
                LogPrintf("MY_APP_HMI_Aktion_Index_19");

                break;
            }
            case HMI_Aktion_Enum_20:{      //被拨号返回_拒绝
                LogPrintf("MY_APP_HMI_Aktion_Index_20");
                break;
            }
            case HMI_Aktion_Enum_21:{      //被拨号返回_拒绝
                LogPrintf("MY_APP_HMI_Aktion_Index_21");

                break;
            }
            case HMI_Aktion_Enum_22:{      //被拨号返回_接通
                LogPrintf("MY_APP_HMI_Aktion_Index_22");

                break;
            }
            case HMI_Aktion_Enum_23:{      //被拨号返回_挂断
                LogPrintf("MY_APP_HMI_Aktion_Index_23");

                break;
            }
            case HMI_Aktion_Enum_25:{      //主拨号:无网络
                LogPrintf("MY_APP_HMI_Aktion_Index_25");
                len= strlen("b.t4.txt=\"连接失败\"");
                memcpy(uart3_write,"b.t4.txt=\"连接失败\"",len);
                uart3_write[len]= 0xff;
                uart3_write[len+1]= 0Xff;
                uart3_write[len+2]= 0Xff;
                KING_UartWrite(UART_3, uart3_write, len+3, &writeLen);
                break;
            }
        }
    }
    
}