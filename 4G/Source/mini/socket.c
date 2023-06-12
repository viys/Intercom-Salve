/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingnet.h"
#include "kingsocket.h"
#include "kingcstd.h"
#include "kingrtos.h"
#include "kingplat.h"
#include "kingCbData.h"
#include "Kinguart.h"
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
#include "KingRTC.h"
#include "KingDef.h"
#include "KingFlash.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
 #define LogPrintf(fmt, args...) while(0)
// #define LogPrintf(fmt, args...)    do { KING_SysLog("socket:"fmt, ##args); } while(0)

uint8                               IS_IP=0;  //是否ip
uint8 new_ip_success=0; //是否有新的ip
uint8 new_ip_success2=0; //是否切换到了旧的ip
// #define CHEERZING_TEST_SERVER_default    "116.62.226.17" //"1.15.153.54" //
// #define CHEERZING_TEST_PORT_default      8107
#define AUDIO_DATA_MAX 8000*5
#define PLAY_DATA_MAX 8000*5

#define CHEERZING_TEST_SERVER "123.57.9.22"
#define CHEERZING_TEST_PORT   8106
char *HTTP_URL = NULL;
/*******************************************************************************
 **   Variables
 ******************************************************************************/
 static TIMER_HANDLE  s_rcdtimer_heartbeat = NULL;
static hardwareHandle hSocketNIC = -1;
static softwareHandle hSocketContext = -1;
int s_socketFd = -1;
static bool isIpv6 = FALSE;
static uint8 socket_data_handle[256]={0xAA,0xFA,0x00,0x00,0x01};  //
static THREAD_HANDLE socketThreadID = NULL;
MSG_HANDLE    socketMsgHandle = NULL;
extern MSG_HANDLE msgHandle_audio;
extern MSG_HANDLE   MP3_msgHandle;      //MP3
static CALENDAR_TIME sysTime;

/*   录音缓存   */
extern uint8 audio_data[AUDIO_DATA_MAX];   //发送缓存
extern uint32 audio_data_num;    //发送缓存到第几个
extern uint32 audio_data_new;     //产生新数据个数
extern uint32 audio_data_en;     //产生了几包数据
#define AUDIO_DATA_EN_MAX 80       //80个字节发送一次 和 播放一次
/*   socket 收发语音   */
uint32 audio_data_num_send=0;       //已发送数据个数

/*   接收语音数据缓存   */
extern uint8 play_data[PLAY_DATA_MAX];//接收缓存
extern uint32 play_data_num;    //接收缓存到第几个
extern uint32 play_data_new;     //产生新数据个数
extern uint32 play_data_en;     //产生了几包数据


/*   用户操作   */
extern uint8 call_user_operation;     //用户操作 1发出呼叫 2等待接通 3对方接通 4语音数据 5被拨号
extern uint8 mail_list_id;  //当前连接的 IMEI ID
extern uint8 mail_list_road;
extern uint8 mail_list_road1;
extern uint8 mail_list_road2;
extern uint8 mail_list_road3;
extern uint8 mail_list_road4;
extern uint8 Call_direction;     //呼叫方向
extern uint8 call_user_tim;      //用户操作响应计时

//uint8 alert1=0;
//uint8 clert2=0; //采集板模式

/*   socket 数据解析   */
uint32 handle_deviation=0;      //已经处理的数据个数
uint32 Current_data_length=0;   //一帧需要包含的数据个数
uint32 Current_data_length_save=0;   //一帧需已经接收的数据个数
uint32 analysis_handle=0;       //解析得到的数据头


/*   socket 数据接收   */
#define PRECV_MAX 10000
uint8 pRecv[PRECV_MAX]={0};
uint32 pRecv_num=0;       //接收到了第几个位置

/*   心跳   */
uint8 heartbeat_tim=0;       //心跳
uint8 socket_en=0;      //区分第一次连接 服务器
uint8 login_success=0;  // 连接后登陆成功
uint8 logon_imei=0;  //连接后获取imei
uint8 socket_content_tim=0; // 开始连接计时
uint8 socket_content_num=0; // 开始连接计次
uint8 socket_close_tim=0; //关闭后延时再连接
#define socket_close_tim_tim 10  //关闭后延时间
uint8 socket_close_num;  // 已经尝试链接次数
#define socket_close_num_num 4 // 最大尝试链接次数
uint8 login_tim=0;  //登陆所用时间
#define login_tim_tim 10 //登陆超时时间
#define login_tim_num 4  //最大登陆次数

uint8 uart3_write[10]={0};
uint8 uart3_writenewday[10]={0}; //新一天数据
uint8 uart3_writejdq[10]={0};  //继电器数据
uint8 uart3_writelogin[10]={0};
// uint8 mail_list_ALL[256];	//群呼
uint8 is_mail_list_ALL=0; //是否正在群呼
uint8 mail_list_ALL_tim=0; //群呼计时
uint8 is_one_day=0;  //每天清除一次数据
uint8 is_new_day=0;	//记录日期

uint8 mail_list_tim=1; //是否需要获取主机imei
uint8 is_host_busy=0; //主机是否 忙
uint8 Call_reception_timeout=3; //通话接收超时时间
uint8 Call_max_tim=60;//最大通话时间
uint8 mail_list[4][16]={
    
//        {0x38, 0x36 ,0x39 ,0x37 ,0x32 ,0x36, 0x30, 0x35 ,0x32 ,0x36 ,0x37,0x37,0x36,0x30,0x36},// 第一个主机
//        {0x38, 0x36 ,0x39 ,0x37 ,0x32 ,0x36, 0x30, 0x35 ,0x32 ,0x36 ,0x37,0x35,0x32,0x38,0x37},// 第一个主机
        // {0x38,0x36,0x34,0x39,0x32,0x37,0x30,0x35,0x37,0x39,0x33,0x32,0x39,0x37,0x31}, //二级主机 2022.5.6
        //修改二级主机4692 换成2971
        //{0x38,0x36,0x39,0x37,0x32,0x36,0x30,0x35,0x32,0x36,0x39,0x34,0x36,0x39,0x32}, //二级主机
//        {0x38, 0x36 ,0x32 ,0x38 ,0x34 ,0x30, 0x30, 0x35 ,0x38 ,0x36 ,0x36,0x36,0x33,0x30,0x32},// 第一个主机
//        {0x38, 0x36 ,0x32 ,0x38 ,0x34 ,0x30, 0x30, 0x35 ,0x38 ,0x36 ,0x36,0x36,0x33,0x30,0x32},// 第一个主机
        // {0x38,0x36,0x39,0x37,0x32,0x36,0x30,0x35,0x32,0x36,0x37,0x33,0x30,0x39,0x32}, //二级主机
        
        //发出去的主机
        // {0x38, 0x36 ,0x32 ,0x38 ,0x34 ,0x30, 0x30, 0x35 ,0x38 ,0x36 ,0x33, 0x33 ,0x36 ,0x37 ,0x34},
        // {0x38, 0x36 ,0x34 ,0x39 ,0x32 ,0x37, 0x30, 0x35 ,0x37 ,0x39 ,0x33,0x31,0x35,0x33,0x36},
        
        };       //imei  IMEI
enum  Socket_instruction{                /*   命令 */
    Socket_instruction_1 = 1,  //登陆
    Socket_instruction_2  ,  //登陆成功
    Socket_instruction_3   ,    //主拨号
    Socket_instruction_4  ,   //主拨号服务端返回
    Socket_instruction_5 ,    //被拨号
    Socket_instruction_6 ,        //被拨号返回
    Socket_instruction_9=9,     //通话数据发送
    Socket_instruction_10 ,      //播放
    Socket_instruction_11=0x10 ,      //心跳
    Socket_instruction_12 ,      //心跳返回
	
    // Socket_instruction_16=0x10 ,      //心跳
    // Socket_instruction_17 ,      //返回
	
    Socket_instruction_18 ,      //索取设备 json
    Socket_instruction_19 ,      //下发设备 json
    Socket_instruction_20=0x14,      //从机获取主机的IMEI
    Socket_instruction_21=0x15,      //从机获取主机的IMEI
    Socket_instruction_22 =0x16 ,      //从机上传报警信息
    Socket_instruction_23 ,      //服务端下发从机上传的报警信息
//		Socket_instruction_24 ,      //主机获取报警信息 空
    Socket_instruction_25 =0x18,      //从机上传计数
    Socket_instruction_29 =0x20,      //继电器控制
    Socket_instruction_30 =0x21,      //继电器返回
    Socket_instruction_31 =0x22,      //向服务的获取ip
    Socket_instruction_32 =0x23,      //服务端 下发ip 例： 0,123.45.67.80:456  或： 1，asd.dfs7893.com:789
	// Socket_instruction_33 =0x24,       // 向服务端获取升级包
    // Socket_instruction_34 =0x25,       // 服务端下发升级包 数据就是请求地址
    // Socket_instruction_35 =0x26,       // 主机上传广播数据
    // Socket_instruction_36 =0x27,       // 分机接收广播数据
	Socket_instruction_33 =0x24,       // 主机上传广播数据
    Socket_instruction_34 =0x25,       // 分机接收广播数据
};
enum Msg_Evt_MP3{
    Msg_Evt_MP3_NULL,
    Msg_Evt_MP3_AUDIO,
    Msg_Evt_MP3_STOP,
    Msg_Evt_MP3_STOP_AUDIO_START,
};
enum  SLAVE_REQ_REQ_CALL_RETURN{     //被呼叫设备返回
    SLAVE_REQ_REQ_CALL_RETURN_0  = 0x30,
    SLAVE_REQ_REQ_CALL_RETURN_1,
    SLAVE_REQ_REQ_CALL_RETURN_2,
    SLAVE_REQ_REQ_CALL_RETURN_3,
    SLAVE_REQ_REQ_CALL_RETURN_4,
    SLAVE_REQ_REQ_CALL_RETURN_5,
};
enum  DIAL_REQ_REQ_CALL{     //主设备呼叫发送 & 被拨号请求
    DIAL_REQ_REQ_CALL_1  = 0x31,    //呼叫
    DIAL_REQ_REQ_CALL_2  ,      //取消
    DIAL_REQ_REQ_CALL_3  ,      //接通
    DIAL_REQ_REQ_CALL_4  ,      //挂断
};
enum  DIAL_REQ_REQ_CALL_RETURN{     //服务端呼叫返回
    DIAL_REQ_REQ_CALL_RETURN_0  = 0x30,
    DIAL_REQ_REQ_CALL_RETURN_1,
    DIAL_REQ_REQ_CALL_RETURN_2,
    DIAL_REQ_REQ_CALL_RETURN_3,
    DIAL_REQ_REQ_CALL_RETURN_4,
    DIAL_REQ_REQ_CALL_RETURN_5,
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
    MSG_EVT_HMI_NEWS,
    MSG_EVT_UART3_SEND,
    MSG_EVT_HMI_NEWS2,
    MSG_EVT_TIME_5s,
    MSG_EVT_HMI_NEWS3,
    MSG_EVT_HMI_NEWS4,
};
/*******************************************************************************
 ** Local Function Delcarations
 ******************************************************************************/
static void socket_connect(SOCK_IN_ADDR_T *addr, uint16 port);
static void socket_enter(int pdpType, char *hostName);
uint32 my_strcmp(uint8* val1,uint8* val2,uint32 line);
uint32 new_connect(uint32 sign);     //判断是否发送成功
void DIAL_REQ_REQ_CALL_RETURN_hander(void);
void CALLED_REQ_REQ_CALL_RETURN_hander(void);
void call_overtime_fun(void);
void fill_audio(uint8* Pdat ,uint32 lines);
void FS_Get_Ip(uint8 *set,uint8 length);
void http_startTest(void);
void Dual_Sim(void);

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
 void Timer_heartbeat(uint32 tmrId)      //发送心跳
{
    MSG_S msg;
    MSG_S msg_socket;
    heartbeat_tim++;
    if(heartbeat_tim>130){
        KING_PowerReboot();
    }
    if(call_user_operation){
        memset(&msg, 0x00, sizeof(MSG_S));
        msg.messageID = 126;
        KING_MsgSend(&msg, socketMsgHandle);  
    }
    if(Current_data_length_save){
        if(heartbeat_tim==3){
            Current_data_length_save=0;
        }
    }
    if(mail_list_ALL_tim){
        mail_list_ALL_tim++;
        memset(&msg, 0x00, sizeof(MSG_S));
        msg.messageID = 133;
        KING_MsgSend(&msg, socketMsgHandle); 
    }
    if(( login_success == 1) && s_socketFd>0){
        if(mail_list_tim){
        if(mail_list_tim<3){ //获取主机imei
            mail_list_tim++;
            if(mail_list_tim==3){
                mail_list_tim=1;
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = 128;
                KING_MsgSend(&msg, socketMsgHandle);
            }
        }}
        if(logon_imei ==1 ){ //获取imei后
            is_one_day++;
            if(is_one_day%20==0) // 查询时间 一天上传一次计数
            {
                is_one_day=0;
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = 130;
                KING_MsgSend(&msg, socketMsgHandle);
            }
            if(is_host_busy){
                is_host_busy++;
                if(is_host_busy>10){
                    is_host_busy=0;
                }
            }
            if(heartbeat_tim%60==0){  //发送心跳
                memset(&msg, 0x00, sizeof(MSG_S));
                msg.messageID = 124;
                KING_MsgSend(&msg, socketMsgHandle);  
            }
        }
    }
    //socket连接成功figma
    if(socket_en ==1){
        if(login_success){//登陆成功

        }else{
            LogPrintf("soc_lin_tm_ok?:%d,%d,%d,",login_tim,new_ip_success,new_ip_success2);
            if(login_tim){
                login_tim++;
                if((login_tim / login_tim_tim) >= login_tim_num){ //达到最大登陆次数
                    if(new_ip_success ==1)//有新的ip
                    {
                        if(new_ip_success2 ==0) //还没有切换到旧的ip
                        {
                            FS_Get_Ip(NULL,0);  //换一个ip
                            login_tim=1; //重新计时
                            KING_SocketClose(s_socketFd);
                            if(socket_close_tim == 0)
                            socket_close_tim=socket_close_tim_tim;
                        }
                    }
                }else{
                    if(login_tim % login_tim_tim ==0){ // 登陆
                        memset(&msg_socket, 0x00, sizeof(MSG_S));
                        msg_socket.messageID = 121;
                        KING_MsgSend(&msg_socket, socketMsgHandle);
                    }
                }
            }
        }
    }
    // 连接耗时
    if(socket_content_tim){
        socket_content_tim--;
        LogPrintf("sot_cse_t2_ok?:%d,%d,%d,%d,",socket_content_tim,new_ip_success,new_ip_success2,socket_content_num);
        if(socket_content_tim ==0){
            socket_content_num++;
            if(socket_content_num< socket_close_num_num){  //最大连接次数内
                KING_SocketClose(s_socketFd);
            }else{ // 达到最大连接次数
                if(new_ip_success ==1)//有新的ip
                {
                    if(new_ip_success2 ==0) //还没有切换到旧的ip
                    {
                        FS_Get_Ip(NULL,0);  //换一个ip
                        // socket_content_tim=1; //重新计时
                        socket_content_num=0; //新计数重
                        socket_close_num=0;

                        KING_SocketClose(s_socketFd);
                        if(socket_close_tim == 0)
                        socket_close_tim=socket_close_tim_tim;
                    }
                }
            }
        }
    }
    // socket失败
    if(socket_close_tim){
        socket_close_tim--;
        LogPrintf("sot_cse_t_ok?:%d,%d,%d,%d,",socket_close_tim,new_ip_success,new_ip_success2,socket_close_num);
        if(socket_close_tim==0){
            socket_close_num++;
            if(socket_close_num< socket_close_num_num){  //最大连接次数内
                // memset(&msg_socket, 0x00, sizeof(MSG_S));
                // msg_socket.messageID = 120;
                // KING_MsgSend(&msg_socket, socketMsgHandle);
                socket_enter(0,CHEERZING_TEST_SERVER);
            }else{ // 达到最大连接次数
                if(new_ip_success ==1)//有新的ip
                {
                    if(new_ip_success2 ==0) //还没有切换到旧的ip
                    {
                        FS_Get_Ip(NULL,0);  //换一个ip
                        socket_close_tim=1; //重新计时
                        socket_close_num=0; //重新计数
                        KING_SocketClose(s_socketFd);
                        if(socket_close_tim == 0)
                        socket_close_tim=socket_close_tim_tim;
                    }
                }
            }
        }
    }

    LogPrintf("MY_APP_CALL_TIM:%d,call:%d,sock:%d,mal_lit_ti:%d,%d",heartbeat_tim,call_user_operation,s_socketFd,mail_list_tim,is_one_day);
}
static void socket_pingCallBack(
            TCPIP_PING_RESULT_E     res,          ///< ping result, 0 - succeed; other - fail
            uint32                  time_delay,   ///< ping time delay, only valid when success, unit: ms
            PING_HANDLE             ping_handle,  ///< ping handle
            uint8                   ttl,
            char*                   ipaddr        ///< uint16 ipaddr_size*/
            )
{
    //LogPrintf("socket_pingCallBack res=%d, time_delay=%d, ttl=%d, ipaddr=%s\r\n", res, time_delay, ttl, ipaddr);
}
static int socket_getNicHandle(void)
{
    int ret = 0;
    NIC_INFO info;

    memset(&info, 0x00, sizeof(NIC_INFO));
    info.NIC_Type = NIC_TYPE_WWAN;
    ret = KING_HardwareNICRequest(info, &hSocketNIC);
    //LogPrintf("socket_getNicHandle ret=%d, hSocketNIC=%d", ret, hSocketNIC);

    return ret;
}
static int socket_getContextHandle(int isIpv6)
{
    int ret = 0;
    NIC_CONTEXT ApnInfo;

    if (hSocketNIC == -1)
    {
        return -1;
    }
    memset(&ApnInfo, 0x00, sizeof(NIC_CONTEXT));
    if (isIpv6)
    {
        //strcpy((char *)ApnInfo.APN_Name, "CMNET");
        ApnInfo.IpType = IP_ADDR_TYPE_V6;
    }
    else
    {
        //strcpy((char *)ApnInfo.APN_Name, "CMNET");
        ApnInfo.IpType = IP_ADDR_TYPE_V4;
    }
    ret = KING_ConnectionAcquire(hSocketNIC, ApnInfo, &hSocketContext);
    //LogPrintf("socket_getContextHandle ret=%d, hSocketContext=%d", ret, hSocketContext);

    return ret;
}
// static void socket_pingTest(void)
// {
//     int ret =0;
//     PING_HANDLE pingH;
    
//     //LogPrintf("socket_pingTest ... ...");
//     if (hSocketContext == -1)
//     {
//         return;
//     }

//     ret = KING_NetPingV4Request(hSocketContext, "www.baidu.com", 20, 3000, socket_pingCallBack, &pingH);
//     if(ret == -1)
//     {
//         //LogPrintf("socket_pingTest KH_NetPingV4Request fail\r\n");
//     }
// }
static void socket_Ipv4NetInfo(void)
{
    int ret = 0;
    IPV4_ADDR_S ip;
    IPV4_ADDR_S gw;
    IPV4_DNS dns;
    char *ipAddr = NULL;
    SOCK_IN_ADDR_T destAddr;
    
    //LogPrintf("socket_Ipv4NetInfo ... ...");
    if (hSocketContext == -1)
    {
        return;
    }
    memset(&ip, 0, sizeof(IPV4_ADDR_S));
    memset(&gw, 0, sizeof(IPV4_ADDR_S));
    memset(&dns, 0, sizeof(IPV4_DNS));
    ret = KING_NetIpv4AddrInfoGet(hSocketContext, &ip, &dns, &gw);
    if(ret == 0)
    {
        memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
        destAddr.type = IP_ADDR_TYPE_V4;
        destAddr.u_addr.ip4.addr = ip.addr;
        KING_ipntoa(destAddr, &ipAddr);
        //LogPrintf("socket_Ipv4NetInfo ip=%s\r\n", ipAddr);

        memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
        destAddr.type = IP_ADDR_TYPE_V4;
        destAddr.u_addr.ip4.addr = gw.addr;
        KING_ipntoa(destAddr, &ipAddr);
        //LogPrintf("socket_Ipv4NetInfo gateway=%s\r\n", ipAddr);

        memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
        destAddr.type = IP_ADDR_TYPE_V4;
        destAddr.u_addr.ip4.addr = dns.primary_dns.addr;
        KING_ipntoa(destAddr, &ipAddr);
        //LogPrintf("socket_Ipv4NetInfo primary_dns=%s\r\n", ipAddr);

        memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
        destAddr.type = IP_ADDR_TYPE_V4;
        destAddr.u_addr.ip4.addr = dns.secondary_dns.addr;
        KING_ipntoa(destAddr, &ipAddr);
        //LogPrintf("socket_Ipv4NetInfo secondary_dns=%s\r\n", ipAddr);

        //LogPrintf("socket_Ipv4NetInfo success\r\n");
    }
    else
    {
        //LogPrintf("socket_Ipv4NetInfo fail\r\n");
    }
}
static bool socket_httpSend(void)
{
    char *httpTxBuf = NULL;
    uint16 sendTotalLen = 0, send_len = 0;
    
    httpTxBuf = malloc(sizeof(char) * (400 + 1));
    if(httpTxBuf != NULL)
    {
        memset(httpTxBuf, 0x00, (sizeof(char) * (400 + 1)));
    }
    else
    {
        return FALSE;
    }

    sprintf(httpTxBuf, "GET %s HTTP/1.1\r\n", "/");

    sprintf(httpTxBuf + strlen(httpTxBuf), "Host: %s", "ipv6.baidu.com");

    sprintf(httpTxBuf + strlen(httpTxBuf), ":");
    sprintf(httpTxBuf + strlen(httpTxBuf), "%d", 80);
    sprintf(httpTxBuf + strlen(httpTxBuf), "\r\n");

    sprintf(httpTxBuf + strlen(httpTxBuf), "Connection: keep-alive\r\n");
    sprintf(httpTxBuf + strlen(httpTxBuf), "Content-Type: %s\r\n", "application/x-www-form-urlencoded");
    sprintf(httpTxBuf + strlen(httpTxBuf), "\r\n");
    
    sendTotalLen = strlen(httpTxBuf);
    send_len = KING_SocketSend(s_socketFd,
                    (void *)httpTxBuf,
                    (size_t)sendTotalLen,
                    0);
    //LogPrintf("socket_httpSend send_len=%d\r\n", send_len);
    free(httpTxBuf);
    httpTxBuf = NULL;
    if (send_len > 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
static void socket_eventProcessCb(uint32 eventID, void* pdata, uint32 len)
{
    int ret;
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;
    MSG_S msg_socket;
    LogPrintf("socket_eventProcessCb() eventID=%d, dataLen=%d\r\n", eventID, len);
    switch (eventID)
    {
    case SOCKET_CONNECT_EVENT_RSP:
    {
        #if 1
            MSG_S msg;
                login_tim=1;
                socket_en=1;
                logon_imei=1;
                socket_content_tim=0;
            LogPrintf("SOCKET_CONNECT_EVENT_RSP KING_MsgSend ");
            memset(&msg, 0x00, sizeof(MSG_S));
            msg.messageID = 121;
            ret = KING_MsgSend(&msg, socketMsgHandle);
            if(-1 == ret)
            {
                //LogPrintf("SOCKET_CONNECT_EVENT_RSP KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
                return;
            }
        #else
        #endif
    }
        break;

    case SOCKET_ACCEPT_EVENT_IND:
        //LogPrintf("EV_CFW_TCPIP_ACCEPT_IND\r\n");
        break;

    case SOCKET_CLOSE_EVENT_RSP:
        LogPrintf("MY_APP_CALL:CLOSE_EVENT_RSP\r\n");
                        //         int a =KING_SocketShutdown(1, SHUT_SEND | SHUT_RECV);
                        // LogPrintf("sot_cse_t22_ok?%d",a);
            s_socketFd = -1;
       if(socket_close_tim == 0){
            socket_close_tim=socket_close_tim_tim;}
        socket_en=0;
        login_tim=0;
        login_success=0;
        logon_imei=0;
        break;

    case SOCKET_ERROR_EVENT_IND:
        LogPrintf("EV_CFW_TCPIP_ERR_IND, errcode=0x%x\r\n", KING_GetLastErrCode());
        // if (s_socketFd != -1)
        // {
            KING_SocketClose(s_socketFd);
            s_socketFd = -1;
        // }
        break;

    case SOCKET_WRITE_EVENT_IND:
        LogPrintf("SOCKET_WRITE_EVENT_IND ack by peer uSendDataSize=%d\r\n", data->socket.availSize);
        break;

    case SOCKET_READ_EVENT_IND:
    {
        uint16 uDataSize = 0;
        #if 1
            MSG_S msg;
        #else
        
        #endif
        
            //LogPrintf("SOCKET_READ_EVENT_IND\r\n");
            uDataSize = data->socket.availSize;
            // LogPrintf("recv datalen=%d\r\n", uDataSize);
            
        #if 1
            memset(&msg, 0x00, sizeof(MSG_S));
            msg.messageID = 122;
            msg.DataLen = uDataSize;
            ret = KING_MsgSend(&msg, socketMsgHandle);
            if(-1 == ret)
            {
                LogPrintf("SOCKET_READ_EVENT_IND KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
                return;
            }
        #else 
        #endif    
    }
        break;

    default:
        LogPrintf("TCPIP unexpect event/response %d\r\n", eventID);
            KING_SocketClose(s_socketFd);
            s_socketFd = -1;
        break;
    }
}
static void socket_connect(SOCK_IN_ADDR_T *addr, uint16 port)
{
    int ret = 0;
    union sockaddr_aligned sock_addr;
    uint32 sockaddr_size = 0;
    union sockaddr_aligned local_addr;
    uint32 localaddr_size = 0;
    
    if (isIpv6)
    {
        ret = KING_SocketCreate(AF_INET6, SOCK_STREAM, IPPROTO_TCP, &s_socketFd);
    }
    else
    {
        ret = KING_SocketCreate(AF_INET, SOCK_STREAM, IPPROTO_TCP, &s_socketFd);
        LogPrintf("socket_connect KING_SocketCreate() %d\r\n",s_socketFd);
    }
    if (ret < 0)
    {
        
        return;
    }
    socket_content_tim= socket_close_tim_tim; //连接开始计时
    KING_RegNotifyFun(DEV_CLASS_SOCKET, s_socketFd, socket_eventProcessCb);

    if (hSocketContext == -1)
    {
        return;
    }

    if (isIpv6)
    {
        IPV6_ADDR_S ip6;
        struct sockaddr_in6 *sock_addr6 = (struct sockaddr_in6 *)&(sock_addr);

        ret = KING_NetIpv6AddrInfoGet(hSocketContext, NULL, NULL, NULL);
        //LogPrintf("KING_NetIpv6AddrInfoGet() ret=%d\r\n", ret);
        memset(&ip6, 0, sizeof(IPV6_ADDR_S));
        ret = KING_NetIpv6AddrInfoGet(hSocketContext, &ip6, NULL, NULL);
        if(ret == 0)
        {
            struct sockaddr_in6 *stLocalAddr6 = (struct sockaddr_in6 *)&(local_addr);
            
            stLocalAddr6->sin6_len = sizeof(struct sockaddr_in6);
            stLocalAddr6->sin6_family = AF_INET6;
            stLocalAddr6->sin6_port = 0;
            stLocalAddr6->sin6_addr.addr[0] = ip6.addr[0];
            stLocalAddr6->sin6_addr.addr[1] = ip6.addr[1];
            stLocalAddr6->sin6_addr.addr[2] = ip6.addr[2];
            stLocalAddr6->sin6_addr.addr[3] = ip6.addr[3];

            localaddr_size = sizeof(struct sockaddr_in6);
        }
        
        sock_addr6->sin6_len = sizeof(struct sockaddr_in6);
        sock_addr6->sin6_family = AF_INET6;
        sock_addr6->sin6_port = KING_htons(port);
        sock_addr6->sin6_addr.addr[0] = addr->u_addr.ip6.addr[0];
        sock_addr6->sin6_addr.addr[1] = addr->u_addr.ip6.addr[1];
        sock_addr6->sin6_addr.addr[2] = addr->u_addr.ip6.addr[2];
        sock_addr6->sin6_addr.addr[3] = addr->u_addr.ip6.addr[3];
        
        sockaddr_size = sizeof(struct sockaddr_in6);
    }
    else
    {
        IPV4_ADDR_S ip      ;
        struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);
        
        ret = KING_NetIpv4AddrInfoGet(hSocketContext, NULL, NULL, NULL);
        LogPrintf("KING_NetIpv4AddrInfoGet() ret=%d\r\n", ret);
        memset(&ip, 0, sizeof(IPV4_ADDR_S));
        ret = KING_NetIpv4AddrInfoGet(hSocketContext, &ip, NULL, NULL);
        if(ret == 0)
        {
            struct sockaddr_in *stLocalAddr = (struct sockaddr_in *)&(local_addr);
            
            stLocalAddr->sin_len = sizeof(struct sockaddr_in);
            stLocalAddr->sin_family = AF_INET;
            stLocalAddr->sin_port = 0;
            stLocalAddr->sin_addr.addr = ip.addr;

            localaddr_size = sizeof(struct sockaddr_in);
        }
        
        sock_addr4->sin_len = sizeof(struct sockaddr_in);
        sock_addr4->sin_family = AF_INET;
        sock_addr4->sin_port = KING_htons(port);
        sock_addr4->sin_addr.addr = addr->u_addr.ip4.addr;
        LogPrintf("socket_connect ip_addr=%u, s_socketFd=%d\r\n", sock_addr4->sin_addr.addr, s_socketFd);

        sockaddr_size = sizeof(SOCK_ADDR_IN_T);
    }

    if (ret == 0)
    {
        ret = KING_SocketBind(s_socketFd, &local_addr, localaddr_size);
        LogPrintf("KING_SocketBind() ret=%d\r\n", ret);
        if (ret < 0)
        {
            LogPrintf("KING_SocketBind fail\r\n");
            KING_SocketClose(s_socketFd);
            return;
        }
    }
    
    ret = KING_SocketConnect(s_socketFd, &sock_addr, sockaddr_size);
    if (ret < 0)
    {
        LogPrintf("KING_SocketConnect fail\r\n");
        KING_SocketClose(s_socketFd);
        return;
    }
}
static void socket_dnsReqCb(void* pData, uint32 len)
{
    ASYN_DNS_PARSE_T *dnsParse = (ASYN_DNS_PARSE_T *)pData;
    
    LogPrintf("socket_dnsReqCb hostname=%s\r\n", dnsParse->hostname);
    LogPrintf("socket_dnsReqCb dnsParse->result=%d\n", dnsParse->result);
    if (dnsParse->result == DNS_PARSE_SUCCESS)
    {
        SOCK_IN_ADDR_T ipaddr = dnsParse->addr;

        if (ipaddr.type == IP_ADDR_TYPE_V6)
        {
            socket_connect(&ipaddr, 80);
        }
        else
        {
            socket_connect(&ipaddr, CHEERZING_TEST_PORT);
        }
    }
    else if (dnsParse->result == DNS_PARSE_ERROR)
    {
        LogPrintf("socket_dnsReqCb fail!\n");
    }
}
static void socket_enter(int pdpType, char *hostName)
{
    int ret = -1;
    SOCK_IN_ADDR_T destAddr;
    SOCK_IN_ADDR_T *ip = &destAddr;
    SOCK_IN_ADDR_T ipAddr;
    LogPrintf("socket_entr:%s ...post:%d ...",hostName,CHEERZING_TEST_PORT);
    if (pdpType == 1)
    {
        // socket_Ipv6NetInfo();
        // socket_ping6Test();
    }
    else
    {
        socket_Ipv4NetInfo();
        // socket_pingTest();
    }
    if (hSocketContext == -1)
    {
        return;
    }

    if(IS_IP == 1){ //  如果不是ip
        memset(&destAddr, 0, sizeof(SOCK_IN_ADDR_T));
    }

    KING_ipaton(CHEERZING_TEST_SERVER,&ipAddr); 
    if(IS_IP == 0){
        socket_connect(&ipAddr, CHEERZING_TEST_PORT);        
    } 

    if(IS_IP == 1){
       ret =  KING_NetGetHostByName(hSocketContext, hostName, ip, 10000, (DNS_CALLBACK)socket_dnsReqCb);
    }
    LogPrintf("socket_enter ret=%d", ret);
    if (ret == 0)
    {
        if (ip != NULL)
        {
            if (destAddr.type == IP_ADDR_TYPE_V6)
            {
                if (0 == destAddr.u_addr.ip6.addr[0] && 0 == destAddr.u_addr.ip6.addr[1]
                    && 0 == destAddr.u_addr.ip6.addr[2] && 0 == destAddr.u_addr.ip6.addr[3])
                {
                    //LogPrintf("socket_enter ipv6 query QUEUED\r\n");
                }
            }
            else
            {
                if (0 == destAddr.u_addr.ip4.addr)
                {
                    //LogPrintf("socket_enter query QUEUED\r\n");
                }
            }
        }
    }
    else
    {
        //LogPrintf("socket_enter KING_NetGetHostByName fail\r\n");
    }
}
static void socket_startTest(int pdpType)
{
    int ret;
    NET_STATE_E state;
    uint8 num=0;

    LogPrintf("\r\n======== socket_startTest start... ========\r\n");
    while (1)
    {
        ret = KING_NetStateGet(&state);
        num++;
        if(num>10){
            KING_PowerReboot();
        }
        LogPrintf("Get net state %d, ret : %d\r\n", state, ret);
        if (ret == 0 && state > NET_NO_SERVICE && state < NET_EMERGENCY)
        {
            NET_CESQ_S Cesq;
            ret = KING_NetSignalQualityGet(&Cesq);
            LogPrintf("Get sinagal ret is %d. rxlev:%d, ber:%d, rscp:%d, ecno:%d, rsrq:%d, rsrp:%d\r\n",
                ret,Cesq.rxlev,Cesq.ber,Cesq.rscp,Cesq.ecno,Cesq.rsrq,Cesq.rsrp);

            if (pdpType == 1)
            {
                ret = KING_NetApnSet(1, "", NULL, NULL, 0, IP_ADDR_TYPE_V6);
            }
            else
            {
                ret = KING_NetApnSet(1, "", NULL, NULL, 0, IP_ADDR_TYPE_V4);
            }
            //LogPrintf("KING_NetApnSet ret : %d\r\n", ret);
            // if (ret != 0)
            // {
            //     //LogPrintf("KING_NetApnSet fail\r\n");
            //     break;
            // }

            socket_getNicHandle();
            socket_getContextHandle(0);
            KING_Sleep(1000);
            if (pdpType == 1)
            {
                socket_enter(1, "ipv6.test-ipv6.com");
            }
            else
            {
                socket_enter(0, CHEERZING_TEST_SERVER);
            }
            LogPrintf("socket_enter complete\r\n");
            break;
        }
        KING_Sleep(1000);
    }

    //LogPrintf("\r\n[KING]======== Socket_test complete ========\r\n");
}
static void socket_test_thread(void *param)
{
    MSG_S msg;
		MSG_S msg_audio;
    int ret = -1;

    while(1)
    {
        memset(&msg, 0x00, sizeof(MSG_S));
        if (socketMsgHandle != NULL)
        {
            ret = KING_MsgRcv(&msg, socketMsgHandle, WAIT_OPT_INFINITE );
            if (ret == -1)
            {
                //LogPrintf("socket_test_thread ret=%d\r\n", ret);
                continue;
            }
            LogPrintf("socket_test_thread receive msg. MsgId=%d\r\n", msg.messageID);
            if (msg.messageID == 120)   
            {
                if (isIpv6)
                {
                    socket_startTest(1);
                }
                else
                {
                    socket_startTest(0);
                }
            }
            else if (msg.messageID == 121)      //登陆
            {
                if (isIpv6)
                {
                    socket_httpSend();
                }
                else
                {
                    uint8 buf[128] = {0};
                    memset(buf, 0x00,sizeof(buf));
                    KING_GetSysImei(buf);
                    
                    // memcpy(buf,(uint8*)"862840058634649",15);

                    socket_data_handle[5]=Socket_instruction_1;
                    socket_data_handle[6]=0;
                    socket_data_handle[7]=0x0f;
                    KING_SocketSend(s_socketFd,socket_data_handle,8,0);
                    ret = KING_SocketSend(s_socketFd, buf, 15, 0);
                    if (-1 == ret)
                    {
                        // LogPrintf("Socket send fail\r\n");
                        if (s_socketFd != -1)
                        {
                            KING_SocketClose(s_socketFd);
                            s_socketFd = -1;
                        }
                    }
                    else
                    {
                        // LogPrintf("Socket send succ1\r\n");
                    }
                    LogPrintf("Socket send succ2\r\n");
                }
            }
            else if (msg.messageID == 122)      //接收网络数据
            {
                uint16 uDataSize = 0;
                // uint8 *temp=NULL;
                uDataSize = msg.DataLen;
                // temp = malloc(uDataSize+1);
                // memset((void *)temp, 0x00, uDataSize + 1);
				memset(pRecv,0x00,uDataSize+1);

                ret = KING_SocketRecv(s_socketFd,pRecv, uDataSize, 0);
                // if(ret == -1)
                if(ret<1)
                {
                }
                else
                {
                    //  handle_deviation=0;      //已经处理的数据个数
                    //  Current_data_length=0;   //一帧需要包含的数据个数
                    //  Current_data_length_save=0;   //一帧已经接收的数据个数
                    //  analysis_handle=0;       //解析得到的数据头
                    LogPrintf("MY_APP_Socket_recv0:%d",ret);
                    handle_deviation=0;
                    if(Current_data_length_save){   //填充剩下的字节
                        switch (analysis_handle){
                            case Socket_instruction_10:{    //语音数据
                                if(ret>=Current_data_length-Current_data_length_save){
                                    fill_audio(pRecv,Current_data_length-Current_data_length_save);
                                    handle_deviation+=Current_data_length-Current_data_length_save;
                                    // LogPrintf("consoSocket001_%d,%d",handle_deviation,Current_data_length_save);
                                    Current_data_length_save=0;
                                    goto is_cmd;
                                }else{
                                    fill_audio(pRecv,ret);
                                    Current_data_length_save+=ret;
                                    // LogPrintf("consoSocket002+=%d,%d",ret,Current_data_length_save);
                                }
                                break;
                            }
                        }
                    }else{
                        is_cmd:
                        if(handle_deviation<ret)
                        {
                            if(!my_strcmp(socket_data_handle,pRecv+handle_deviation,4)){  //协议对比成功
                                analysis_handle=pRecv[5+handle_deviation];
                                Current_data_length=((pRecv[6+handle_deviation]<<8)+pRecv[7+handle_deviation]);
                                LogPrintf("MY_APP_Socket_recv_handle:%d,%d,%d,%d,%d,%d,%d,%d,",analysis_handle,Current_data_length,pRecv[6+handle_deviation],pRecv[7+handle_deviation],pRecv[8+handle_deviation],pRecv[9+handle_deviation],pRecv[10+handle_deviation],pRecv[11+handle_deviation]);
                                Current_data_length_save=0;
                                heartbeat_tim=0;    //心跳时间

                                switch (analysis_handle){
                                    case Socket_instruction_4:{ //主拨号返回
                                        DIAL_REQ_REQ_CALL_RETURN_hander();
                                        handle_deviation+=Current_data_length+8;
                                        goto is_cmd;
                                        break;
                                    }
                                    case Socket_instruction_5:{ //被拨号请求
                                        CALLED_REQ_REQ_CALL_RETURN_hander();
                                        handle_deviation+=Current_data_length+8;
                                        goto is_cmd;
                                        break;
                                    }
                                    case Socket_instruction_10:{    //语音数据
                                        if(ret-8-handle_deviation>=Current_data_length){
                                            fill_audio(pRecv+8+handle_deviation,Current_data_length);
                                            handle_deviation+=Current_data_length+8;
                                            // LogPrintf("consoSocket003+=%d,%d",Current_data_length,ret-handle_deviation);
                                            goto is_cmd;
                                            break;
                                        }else{
                                            fill_audio(pRecv+8+handle_deviation,ret-handle_deviation-8);
                                            // LogPrintf("consoSocket004+=%d,%d",ret-handle_deviation-8,ret);
                                            Current_data_length_save=(ret-handle_deviation-8);
                                        }
                                        break;
                                    }
                                    case Socket_instruction_12:{    //心跳返回
                                        handle_deviation+=Current_data_length+8;
                                        goto is_cmd;
                                        break;
                                    }
                                    case Socket_instruction_2:{    //登陆成功
										
                                        //发送直梯扶梯
                                        memcpy(uart3_writelogin,"cmd\x04",4);
                                        uart3_writelogin[4] = pRecv[9+handle_deviation];
                                        uart3_writelogin[5] = pRecv[10+handle_deviation];
                                        uart3_writelogin[6] = pRecv[11+handle_deviation]; //1 扶梯
                                        memset(&msg_audio, 0x00, sizeof(MSG_S));
                                        msg_audio.messageID = MSG_EVT_UART3_SEND;
                                        msg_audio.pData = &uart3_writelogin;
                                        msg_audio.DataLen = 10;
                                        KING_MsgSend(&msg_audio, msgHandle_audio);
                                        // socket_en=2;
                                        login_success=1;
                                        socket_close_num=0;
                                        socket_content_num=0;
                                        handle_deviation+=Current_data_length+8;
                                        if((new_ip_success == 1)&&(new_ip_success2 != 1)){
                                            new_ip_success=0;
                                            new_ip_success2=0;
                                        FS_Get_Ip(NULL,255);
                                    }
                                        goto is_cmd;
                                        break;
                                    }
                                    case Socket_instruction_21:{  //主机的IMEi
										// LogPrintf("MY_APP_LISTYU%d",pRecv[8+handle_deviation+15]);
                                        mail_list_tim=0;
                                        memcpy(mail_list[0],pRecv+8+handle_deviation,15);
                                        
                                        if(pRecv[8+handle_deviation+15]==44){
                                            memcpy(mail_list[1],pRecv+8+handle_deviation+16,15);
											// LogPrintf("MY_APP_LISTYU%s,%s",mail_list[0],mail_list[1]);
                                            if(pRecv[8+handle_deviation+15+16]==44){
                                                memcpy(mail_list[2],pRecv+8+handle_deviation+16+16,15);
                                            }else{
                                                memcpy(mail_list[2],mail_list[0],15);
                                            }
                                        }else{
                                            memcpy(mail_list[1],mail_list[0],15);
                                            memcpy(mail_list[2],mail_list[0],15);
                                        }
                                        handle_deviation+=Current_data_length+8;
                                        logon_imei=1; //获取了imei

                                        // http_startTest();
                                        goto is_cmd;
                                        break;
                                    }
                                    case Socket_instruction_29:{  //继电器控制 直接转发
                                        memcpy(uart3_writejdq,"cmd\x06",4);
                                        uart3_writejdq[4] = pRecv[7+16+handle_deviation];  //采集板id
                                        uart3_writejdq[5] = pRecv[7+17+handle_deviation];  //继电器id
                                        uart3_writejdq[6] = pRecv[7+18+handle_deviation];  //继电器状态
                                        memset(&msg_audio, 0x00, sizeof(MSG_S));
                                        msg_audio.messageID = MSG_EVT_UART3_SEND;
                                        msg_audio.pData = &uart3_writejdq;
                                        msg_audio.DataLen = 10;
                                        KING_MsgSend(&msg_audio, msgHandle_audio);
                                        
                                        handle_deviation+=Current_data_length+8;
                                        goto is_cmd;
                                    //    break;
                                    }
                                    case Socket_instruction_32:{  //修改服务器地址 改为 下发升级包地址
                                        memset(HTTP_URL,0,128+1);
                                        memcpy(HTTP_URL,pRecv+10+handle_deviation,Current_data_length-7);
                                        http_startTest();

                                        handle_deviation+=Current_data_length+8;
                                        goto is_cmd;
                                    }
                                    // case Socket_instruction_34:{  //下发升级包地址
                                    //     memset(HTTP_URL,0,128+1);
                                    //     memcpy(HTTP_URL,pRecv+8+handle_deviation,Current_data_length);
                                    //     http_startTest();
                                    //     handle_deviation+=Current_data_length+8;
                                    //     goto is_cmd;
                                    // }
                                    case Socket_instruction_34:{ //广播语音数据 （广播/挂断）(继电器bit)(语音
                                         analysis_handle=Socket_instruction_10; //模拟语音数据头
                                        if(pRecv[8+handle_deviation] ==1){  //接听广播 挂断广播
                                            if(is_mail_list_ALL==0){ //如果没有广播 就闭合继电器
                                                MSG_S msg_audio_mp3;
                                                is_mail_list_ALL=1;
                                                mail_list_ALL_tim=1; //广播计时
                                                memset(&msg_audio_mp3, 0x00, sizeof(MSG_S));
                                                msg_audio_mp3.messageID = Msg_Evt_MP3_STOP_AUDIO_START;
                                                KING_MsgSend(&msg_audio_mp3, MP3_msgHandle);

                                                memcpy(uart3_write,"cmd\x03",4);
                                                uart3_write[4] =pRecv[9+handle_deviation]; //继电器bit
                                                // uart3_write[4]=0x03;
                                                uart3_write[5] = 0;
                                                memset(&msg_audio, 0x00, sizeof(MSG_S));
                                                msg_audio.messageID = MSG_EVT_UART3_SEND;
                                                msg_audio.pData = (uint8*)uart3_write;
                                                msg_audio.DataLen = 10;
                                                KING_MsgSend(&msg_audio, msgHandle_audio);
                                            }
                                            // 接收语音数据
                                            Current_data_length=80;  // 广播数据固定80 字节
                                            if(ret-10-handle_deviation>=Current_data_length){
                                                fill_audio(pRecv+10+handle_deviation,Current_data_length);
                                                handle_deviation+=Current_data_length+10;
                                                // LogPrintf("MY_APP_Socket003+=%d,%d",Current_data_length,ret-handle_deviation);
                                                goto is_cmd;
                                                break;
                                            }else{
                                                fill_audio(pRecv+10+handle_deviation,ret-handle_deviation-10);
                                                // LogPrintf("MY_APP_Socket004+=%d,%d",ret-handle_deviation-10,ret);
                                                Current_data_length_save=(ret-handle_deviation-10);
                                            }
                                        }else{ //挂断广播
                                            if(is_mail_list_ALL==1){
                                                LogPrintf("MY_APP_Socket_recvaaa");
                                                is_mail_list_ALL=0;
                                                mail_list_ALL_tim=0;
                                                
                                                memcpy(uart3_write,"cmd\x02",4);
                                                uart3_write[4] = 0;
                                                uart3_write[5] = 0;

                                                memset(&msg_audio, 0x00, sizeof(MSG_S));
                                                msg_audio.messageID = MSG_EVT_UART3_SEND;
                                                msg_audio.pData = &uart3_write;
                                                msg_audio.DataLen = 10;
                                                KING_MsgSend(&msg_audio, msgHandle_audio);

                                                KING_AudioDevStopPocMode();
                                                KING_AudioStop();
                                            }
                                        }
                                        handle_deviation+=Current_data_length+8;
                                        goto is_cmd;
                                        
                                    }
                                }

                            }else{
                                handle_deviation++;
                                 LogPrintf("gb7g16dgb6");
                                goto is_cmd;
                            }
                        }
                    }
                }
                // free(temp);
                // temp=NULL;
            }
            else if (msg.messageID == 123){      //拨号接通挂断
                int ret=0;
                socket_data_handle[5]=Socket_instruction_3 ; 
                socket_data_handle[6]=0;
                socket_data_handle[7]=0x11;
                socket_data_handle[8]=mail_list_road;       
                socket_data_handle[9]=DIAL_REQ_REQ_CALL_1;    //拨号内容:呼叫
                memcpy(socket_data_handle+10,mail_list[0],15);
                ret = KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);
                if(!new_connect(ret)){};
            }
            else if (msg.messageID == 124){     //发送心跳
                NET_CESQ_S Cesq;
                ret = KING_NetSignalQualityGet(&Cesq);
                // LogPrintf("Get sinagal ret is %d. rxlev:%d, ber:%d, rscp:%d, ecno:%d, rsrq:%d, rsrp:%d\r\n",
                // ret,Cesq.rxlev,Cesq.ber,Cesq.rscp,Cesq.ecno,Cesq.rsrq,Cesq.rsrp);
                socket_data_handle[5]=Socket_instruction_11;        //心跳
                socket_data_handle[6]=0;
                socket_data_handle[7]=2;
                socket_data_handle[9] = Cesq.rsrq&0x00ff;   //信号质量
                socket_data_handle[8] = Cesq.rsrp&0x00ff;   //信号强度
                ret = KING_SocketSend(s_socketFd,socket_data_handle,10,0);
                if(!new_connect(ret)){};
                // 如果没有通听话
                if(call_user_operation ==0){
                    /*  清空变量  */
                    audio_data_num_send=0;      //已发送数据个数
                    mail_list_road1=0;
                    mail_list_road2=0;
                    mail_list_road3=0;
                    mail_list_road4=0;
                    handle_deviation=0;      //已经处理的数据个数
                    Current_data_length=0;   //一帧需要包含的数据个数
                    Current_data_length_save=0;   //一帧已经接收的数据个数
                    analysis_handle=0;       //解析得到的数据头
                    mail_list_id=0;
            
                    memcpy(uart3_write,"cmd\x02",4);
                    uart3_write[4] = pRecv[8+handle_deviation]-0x30;
                    uart3_write[5] = 0;
                    memset(&msg_audio, 0x00, sizeof(MSG_S));
                    msg_audio.messageID = MSG_EVT_UART3_SEND;
                    msg_audio.pData = uart3_write;
                    msg_audio.DataLen = 10;
                    KING_MsgSend(&msg_audio, msgHandle_audio);                    
                }


            }
            else if (msg.messageID == 125){      //发送音频数据
                if(audio_data_en){
                    // LogPrintf("MY_APP_Socketaudio_send:%d,",audio_data_en);
                    socket_data_handle[5]=Socket_instruction_9;
                    socket_data_handle[6]=AUDIO_DATA_EN_MAX>>8;
                    socket_data_handle[7]=AUDIO_DATA_EN_MAX&0xff;
                    KING_SocketSend(s_socketFd,socket_data_handle,8,0);
                    ret = KING_SocketSend(s_socketFd, audio_data+audio_data_num_send,AUDIO_DATA_EN_MAX, 0);
                    if(!new_connect(ret)){};
                    audio_data_en--;
                    audio_data_num_send+=AUDIO_DATA_EN_MAX;
                    if(audio_data_num_send >= AUDIO_DATA_MAX){
                        audio_data_num_send -= AUDIO_DATA_MAX;
                    }
                }
            }
            else if (msg.messageID == 126){      //通话超时
                call_overtime_fun();
            }
            else if (msg.messageID == 127){     //发出呼叫，使主机产生通话记录

                uint8 temp=0;
                if(mail_list_road1>3){ temp=mail_list_road1; mail_list_road1=2;}
                if(mail_list_road2>3){ temp=mail_list_road2; mail_list_road2=2;}
                if(mail_list_road3>3){ temp=mail_list_road3; mail_list_road3=2;}
                if(mail_list_road4>3){ temp=mail_list_road4; mail_list_road4=2;}

                LogPrintf("zvzvzcvzxcvzvb1:%d\r\n", temp);
                socket_data_handle[5]=Socket_instruction_3 ; 
                socket_data_handle[6]=0;
                socket_data_handle[7]=0x11;
                socket_data_handle[8]=temp;       
                socket_data_handle[9]=DIAL_REQ_REQ_CALL_1;    //拨号内容:呼叫
                memcpy(socket_data_handle+10,mail_list[0],15);
                KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);

                socket_data_handle[5]=Socket_instruction_3 ; 
                socket_data_handle[6]=0;
                socket_data_handle[7]=0x11;
                socket_data_handle[8]=temp;       
                socket_data_handle[9]=DIAL_REQ_REQ_CALL_2;    //拨号内容:取消
                memcpy(socket_data_handle+10,mail_list[0],15);
                KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);
            }
            else if (msg.messageID == 128){   //获取主机
                    uint8 buff[20];
                    KING_GetSysImei(buff);
                    // LogPrintf("MY_APP_HMI_CdvsdMD_2_");
                    socket_data_handle[5]=Socket_instruction_20 ;  //发送获取主机 IMEI 指令
                    socket_data_handle[6]=0;
                    socket_data_handle[7]=0x0f;
                    memcpy(socket_data_handle+8,buff,15);
                    ret = KING_SocketSend(s_socketFd,socket_data_handle,8+15,0);
            }
            else if (msg.messageID == 129){  //报警信息
                    uint8 buff[20];
                    memset(socket_data_handle+5,0,30);
                    memcpy(socket_data_handle+8+16,msg.pData,1);
                    memcpy(socket_data_handle+8+18,msg.pData+1,1);
                    memcpy(socket_data_handle+8+20,msg.pData+2,1);
                    KING_GetSysImei(buff);

                    socket_data_handle[5]=Socket_instruction_22;
                    socket_data_handle[6]=0x00;
                    socket_data_handle[7]=0x15;
                    socket_data_handle[8+15]=',';
                    socket_data_handle[8+17]=',';
                    socket_data_handle[8+19]=','; //29个
                    memcpy(socket_data_handle+8,buff,15);
                    KING_SocketSend(s_socketFd,socket_data_handle,8+15+6,0); //29个
                // 	LogPrintf("MY_APP_ddsD129:%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
                // socket_data_handle[0],
                // socket_data_handle[1],
                // socket_data_handle[2],
                // socket_data_handle[3],
                // socket_data_handle[4],
                // socket_data_handle[5],
                // socket_data_handle[6],
                // socket_data_handle[7],
                // socket_data_handle[8],
                // socket_data_handle[9],
                // socket_data_handle[10],
                // socket_data_handle[11],
                // socket_data_handle[12],
                // socket_data_handle[13],
                // socket_data_handle[14],
                // socket_data_handle[15],
                // socket_data_handle[16],
                // socket_data_handle[17],
                // socket_data_handle[18],
                // socket_data_handle[19],
                // socket_data_handle[20],
                // socket_data_handle[21],
                // socket_data_handle[22],
                // socket_data_handle[23],
                // socket_data_handle[24],
                // socket_data_handle[25],
                // socket_data_handle[26],
                // socket_data_handle[27],
                // socket_data_handle[28],
                // socket_data_handle[29]
                // );  //发送报警信息
            }
            else if(msg.messageID == 130){ //准备发送采集板计数
                // uint8 time_temp[6];
                KING_LocalTimeGet(&sysTime);
                // time_temp[0]=sysTime.yy;
                // time_temp[1]=sysTime.MM;
                // time_temp[2]=sysTime.dd;
                // time_temp[3]=sysTime.hh;
                // time_temp[4]=sysTime.mm;
                // time_temp[5]=(sysTime.ss_h<<4)+sysTime.ss_l;
                if(is_new_day!=0){
                    if(sysTime.dd!=is_new_day){ //新的一天
                        memcpy(uart3_writenewday,"cmd\x05",4);
                        uart3_writenewday[4] = 1;
                        uart3_writenewday[5] = 1;
                        memset(&msg_audio, 0x00, sizeof(MSG_S));
                        msg_audio.messageID = MSG_EVT_UART3_SEND;
                        msg_audio.pData = &uart3_writenewday;
                        msg_audio.DataLen = 10;
                        KING_MsgSend(&msg_audio, msgHandle_audio);
                    }
                }
                // LogPrintf("MY_APP_SOCKET_130:%x,%x",is_new_day,sysTime.mm);
                is_new_day=sysTime.dd;
            }
            else if(msg.messageID == 131){	//发送采集板计数
                // continue;
                uint8 buff[20];
                KING_GetSysImei(buff);
                socket_data_handle[5]=Socket_instruction_25;
                socket_data_handle[6]=0x00;
                socket_data_handle[7]=0x17;
                memcpy(socket_data_handle+8,buff,15);
                socket_data_handle[8+15]=',';
                socket_data_handle[8+16]=0; //ID
                socket_data_handle[8+17]=',';
                memcpy(socket_data_handle+8+18,msg.pData,2);
                socket_data_handle[8+20]=',';
                memcpy(socket_data_handle+8+21,msg.pData+2,2);
                   KING_SocketSend(s_socketFd,socket_data_handle,23+8,0);
                //  	LogPrintf("MY_APP_ddsD129:%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",
                //  socket_data_handle[0],
                //  socket_data_handle[1],
                //  socket_data_handle[2],
                //  socket_data_handle[3],
                //  socket_data_handle[4],
                //  socket_data_handle[5],
                //  socket_data_handle[6],
                //  socket_data_handle[7],
                //  socket_data_handle[8],
                //  socket_data_handle[9],
                //  socket_data_handle[10],
                //  socket_data_handle[11],
                //  socket_data_handle[12],
                //  socket_data_handle[13],
                //  socket_data_handle[14],
                //  socket_data_handle[15],
                //  socket_data_handle[16],
                //  socket_data_handle[17],
                //  socket_data_handle[18],
                //  socket_data_handle[19],
                //  socket_data_handle[20],
                //  socket_data_handle[21],
                //  socket_data_handle[22],
                //  socket_data_handle[23],
                //  socket_data_handle[24],
                //  socket_data_handle[25],
                //  socket_data_handle[26],
                //  socket_data_handle[27],
                //  socket_data_handle[28],
                //  socket_data_handle[29],
                //  socket_data_handle[30],
                //  socket_data_handle[31],
                //  socket_data_handle[32],
                //  socket_data_handle[33]
                // );  //发送报警信息
                    socket_data_handle[5]=Socket_instruction_25;
                    socket_data_handle[6]=0x00;
                    socket_data_handle[7]=0x17;
                    memcpy(socket_data_handle+8,buff,15);
                    socket_data_handle[8+15]=',';
                    socket_data_handle[8+16]=1; //ID
                    socket_data_handle[8+17]=',';
                    memcpy(socket_data_handle+8+18,msg.pData+4,2);
                    socket_data_handle[8+20]=',';
                    memcpy(socket_data_handle+8+21,msg.pData+6,2);
                   KING_SocketSend(s_socketFd,socket_data_handle,23+8,0);
            }
            else if(msg.messageID == 132){	//发送继电器状态
                uint8 buff[20];
                KING_GetSysImei(buff);
                // memset(socket_data_handle+5,0,30);
                memcpy(socket_data_handle+8+15,msg.pData,4); 
                //id 状态 状态 id 状态 状态
                socket_data_handle[5]=Socket_instruction_30;
                socket_data_handle[6]=0x00;
                socket_data_handle[7]=15+4;
                memcpy(socket_data_handle+8,buff,15);
                KING_SocketSend(s_socketFd,socket_data_handle,8+15+4,0);
 	            // LogPrintf("MY_APP_djjqiD129:%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",
                //  socket_data_handle[0],
                //  socket_data_handle[1],
                //  socket_data_handle[2],
                //  socket_data_handle[3],
                //  socket_data_handle[4],
                //  socket_data_handle[5],
                //  socket_data_handle[6],
                //  socket_data_handle[7],
                //  socket_data_handle[8],
                //  socket_data_handle[9],
                //  socket_data_handle[10],
                //  socket_data_handle[11],
                //  socket_data_handle[12],
                //  socket_data_handle[13],
                //  socket_data_handle[14],
                //  socket_data_handle[15],
                //  socket_data_handle[16],
                //  socket_data_handle[17],
                //  socket_data_handle[18],
                //  socket_data_handle[19],
                //  socket_data_handle[20],
                //  socket_data_handle[21],
                //  socket_data_handle[22],
                //  socket_data_handle[23],
                //  socket_data_handle[24],
                //  socket_data_handle[25],
                //  socket_data_handle[26],
                //  socket_data_handle[27],
                //  socket_data_handle[28]
                // );  //发送报警信息
            }
            else if(msg.messageID == 133){	//广播计时
                if(( mail_list_ALL_tim>Call_max_tim) || (heartbeat_tim>5)){ // 广播挂断
                    // if(is_mail_list_ALL==1){
                        is_mail_list_ALL=0;
                        mail_list_ALL_tim=0;
                        audio_stop_v2();
                        memcpy(uart3_write,"cmd\x02",4);
                        uart3_write[4] = 0;
                        uart3_write[5] = 0;
                        memset(&msg_audio, 0x00, sizeof(MSG_S));
                        msg_audio.messageID = MSG_EVT_UART3_SEND;
                        msg_audio.pData = &uart3_write;
                        msg_audio.DataLen = 10;
                        KING_MsgSend(&msg_audio, msgHandle_audio);
                    // }
                }
            }
        }
    }
}
void socket_thread(void)
{
    #if 1    
    int ret = -1;

    uint8 aaaaa[30]={0};
    // memcpy(aaaaa,(uint8*)"0,103.46.128.21:41592",sizeof("0,103.46.128.21:41592"));
    // memcpy(aaaaa,(uint8*)"0,123.57.9.22:8106",sizeof("0,123.57.9.22:8106"));
    MSG_S msg;
    THREAD_ATTR_S threadAttr;
    TIMER_ATTR_S timerattr;

    // 初始化ip和端口
    // CHEERZING_TEST_SERVER = malloc(256 + 1);
    // memset(CHEERZING_TEST_SERVER,0,256+1);
    // memcpy(CHEERZING_TEST_SERVER,CHEERZING_TEST_SERVER_default,sizeof(CHEERZING_TEST_SERVER_default));
    // CHEERZING_TEST_PORT = CHEERZING_TEST_PORT_default;
    // FS_Get_Ip(aaaaa+0,sizeof("0,123.57.9.22:8106"));
    // FS_Get_Ip(NULL,0);
    HTTP_URL = malloc(128 + 1);

    KING_TimerCreate(&s_rcdtimer_heartbeat);    //心跳
    memset(&timerattr, 0x00, sizeof(timerattr));
    timerattr.isPeriod= TRUE;
    timerattr.timeout = 1000;
    timerattr.timer_fun = Timer_heartbeat;
    KING_TimerActive(s_rcdtimer_heartbeat, &timerattr);

    ret = KING_MsgCreate(&socketMsgHandle);
    LogPrintf("socket_test is KING_MsgCreate ret %d", ret);
    Dual_Sim();
    KING_Sleep(1000);
    
    //LogPrintf("socket_test is KING_ThreadCreate ");
    memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
    threadAttr.fun = socket_test_thread;
    threadAttr.priority = THREAD_PRIORIT1;
    threadAttr.stackSize = 1024*4;
    threadAttr.queueNum = 64;
    
    //LogPrintf("socket_test is create socketThreadID 0x%08x", &socketThreadID);
    ret = KING_ThreadCreate("socketTestThread", &threadAttr, &socketThreadID);
    if (-1 == ret)
    {
        // LogPrintf("[KING]Create Test Thread Failed!! Errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    KING_Sleep(1000);
    
    //LogPrintf("APP Main is KING_MsgSend ");
    memset(&msg, 0x00, sizeof(MSG_S));
    msg.messageID = 120;
    ret = KING_MsgSend(&msg, socketMsgHandle);
    if(-1 == ret)
    {
        //LogPrintf("KING_MsgSend() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return;
    }
    #else
    socket_startTest(0);
    #endif
}
uint32 my_strcmp(uint8* val1,uint8* val2,uint32 line){     //指针内容对比函数
    uint32 p=0;
    while( p<line ){
        if(*val1!=*val2){   
            // if(p==line-1){
            //     return 0;
            // }else{
                return p+1; // 不相同的数据在数组中第 p 个
            // }
        }
        val1++;
        val2++;
        p++;
    }
    /*   对比成功   */
    return 0;  
}
void DIAL_REQ_REQ_CALL_RETURN_hander(void){      //主拨号返回 服务端返回 判断对方接通挂断忙等
    MSG_S msg_audio_mp3;
    MSG_S msg_audio;
    //uint8 uart3_write[10]={0};
    LogPrintf("MY_APP_CALL_REQ:%d",pRecv[9+handle_deviation]);
    switch (pRecv[9+handle_deviation]){
        case DIAL_REQ_REQ_CALL_RETURN_0:{   //对方忙
                if(call_user_operation == 1){ //自己拨号，对方忙

            if(mail_list_road1 > 2){ //判断是不是其他通道
                mail_list_road1=2;
                break;
            }if(mail_list_road2 > 2){
                mail_list_road2=2;
                break;
            }if(mail_list_road3 > 2){
                mail_list_road3=2;
                break;
            }if(mail_list_road4 > 2){
                mail_list_road4=2;
                break;
            }
                call_user_operation=0;
                call_user_tim=0;
                is_host_busy=1;
                msg_audio.messageID = MSG_EVT_HMI_NEWS4;
                // msg_audio.pData = pcmdata;
                // msg_audio.DataLen = HMI_Aktion_Enum_14;
                KING_MsgSend(&msg_audio, msgHandle_audio);

                memcpy(uart3_write,"cmd\x02",4);
                uart3_write[4] = pRecv[8+handle_deviation]-0x30;
                uart3_write[5] = 0;
                memset(&msg_audio, 0x00, sizeof(MSG_S));
                msg_audio.messageID = MSG_EVT_UART3_SEND;
                msg_audio.pData = uart3_write;
                msg_audio.DataLen = 10;
                KING_MsgSend(&msg_audio, msgHandle_audio);
            }

                break;
                /*   待测试   */
                socket_data_handle[5]=Socket_instruction_3 ; 
                socket_data_handle[6]=0;
                socket_data_handle[7]=0x11;
                socket_data_handle[8]=mail_list_road;       
                socket_data_handle[9]=DIAL_REQ_REQ_CALL_2;    //拨号内容:取消
                memcpy(socket_data_handle+10,mail_list[0],15);
                KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);
                memset(&msg_audio_mp3, 0x00, sizeof(MSG_S));
                msg_audio_mp3.messageID = Msg_Evt_MP3_AUDIO;
                msg_audio_mp3.pData=(char*)"/app/system/audio/end_conv.mp3";
                KING_MsgSend(&msg_audio_mp3, MP3_msgHandle);

                memcpy(uart3_write,"cmd\x02",4);
                uart3_write[4] = pRecv[8+handle_deviation]-0x30;
                uart3_write[5] = 1;
                memset(&msg_audio, 0x00, sizeof(MSG_S));
                msg_audio.messageID = MSG_EVT_UART3_SEND;
                msg_audio.pData = uart3_write;
                msg_audio.DataLen = 10;
                KING_MsgSend(&msg_audio, msgHandle_audio);

                call_user_operation=0;
                call_user_tim=0;
            break;
        }
        case DIAL_REQ_REQ_CALL_RETURN_1:{   //呼叫中
            call_user_operation=2;
            // call_user_tim=0; 
            break;
        }
        case DIAL_REQ_REQ_CALL_RETURN_2:{   //对方同意
            case_DIAL_REQ_REQ_CALL_RETURN_2:
            call_user_operation=3;
            call_user_tim=0;
            /*  清空变量  */
            audio_data_num_send=0;      //已发送数据个数
            // mail_list_road1=0;
            // mail_list_road2=0;
            // mail_list_road3=0;
            // mail_list_road4=0;
            handle_deviation=0;      //已经处理的数据个数
            Current_data_length=0;   //一帧需要包含的数据个数
            Current_data_length_save=0;   //一帧已经接收的数据个数
            analysis_handle=0;       //解析得到的数据头
            mail_list_id=0;

            memset(&msg_audio_mp3, 0x00, sizeof(MSG_S));
            msg_audio_mp3.messageID = Msg_Evt_MP3_STOP_AUDIO_START;
            KING_MsgSend(&msg_audio_mp3, MP3_msgHandle);

            break;
        }
        case DIAL_REQ_REQ_CALL_RETURN_3:{   //对方拒绝
            case_DIAL_REQ_REQ_CALL_RETURN_3:

            if(mail_list_road1 > 2){
                mail_list_road1=2;
                break;
            }if(mail_list_road2 > 2){
                mail_list_road2=2;
                break;
            }if(mail_list_road3 > 2){
                mail_list_road3=2;
                break;
            }if(mail_list_road4 > 2){
                mail_list_road4=2;
                break;
            }
            call_user_operation=0;

                if(call_user_tim>Call_max_tim-2){
                    msg_audio.messageID = MSG_EVT_HMI_NEWS3;
                    // msg_audio.pData = pcmdata;
                    // msg_audio.DataLen = HMI_Aktion_Enum_14;
                    KING_MsgSend(&msg_audio, msgHandle_audio);
                }else{
                    msg_audio.messageID = MSG_EVT_HMI_NEWS2;
                    // msg_audio.pData = pcmdata;
                    // msg_audio.DataLen = HMI_Aktion_Enum_14;
                    KING_MsgSend(&msg_audio, msgHandle_audio);
                }

            
            call_user_tim=0;

            memcpy(uart3_write,"cmd\x02",4);
            uart3_write[4] = pRecv[8+handle_deviation]-0x30;
            uart3_write[5] = 0;
            memset(&msg_audio, 0x00, sizeof(MSG_S));
            msg_audio.messageID = MSG_EVT_UART3_SEND;
            msg_audio.pData = uart3_write;
            msg_audio.DataLen = 10;
            KING_MsgSend(&msg_audio, msgHandle_audio);
            break;
        }
        case DIAL_REQ_REQ_CALL_RETURN_4:{   //已接通
            goto case_DIAL_REQ_REQ_CALL_RETURN_2;
            break;
        }
        case DIAL_REQ_REQ_CALL_RETURN_5:{   //对方挂断
            goto case_DIAL_REQ_REQ_CALL_RETURN_3;
            break;
        }
    }
}
void CALLED_REQ_REQ_CALL_RETURN_hander(void){      //被拨号请求 服务端发送的 判断本机接通挂断忙等
    MSG_S msg_audio_mp3;
    MSG_S msg_audio;
    //uint8 uart3_write[10]={0};
    
    if(is_mail_list_ALL!=0){return;}  // 广播
    
    LogPrintf("MY_APP_CALL_REQC:%d",pRecv[9+handle_deviation]);
    switch (pRecv[9+handle_deviation]){
        case DIAL_REQ_REQ_CALL_1:{      //呼叫
                // if((call_user_operation==3)||(call_user_operation==5)){ //正在通话
                if(call_user_operation){
                    // socket_data_handle[5]=Socket_instruction_6 ;
                    // socket_data_handle[6]=0;
                    // socket_data_handle[7]=0x11;
                    // socket_data_handle[8]=pRecv[8+handle_deviation];
                    // socket_data_handle[9]=SLAVE_REQ_REQ_CALL_RETURN_0;  //忙
                    // memcpy(socket_data_handle+10 , pRecv+10+handle_deviation ,15);
                    // KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);
                    mail_list_id=1;
                    break;
                }
                
                /*  清空变量  */
                audio_data_num_send=0;      //已发送数据个数
                mail_list_road1=0;
                mail_list_road2=0;
                mail_list_road3=0;
                mail_list_road4=0;
                handle_deviation=0;      //已经处理的数据个数
                Current_data_length=0;   //一帧需要包含的数据个数
                Current_data_length_save=0;   //一帧已经接收的数据个数
                analysis_handle=0;       //解析得到的数据头
                mail_list_id=0;


                call_user_operation=5;
                call_user_tim=0;

                memcpy(mail_list[3],pRecv+10+handle_deviation,15);   //imei
                mail_list[3][15]=pRecv[8+handle_deviation];  //road
                mail_list_road=pRecv[8+handle_deviation];   //road0
                
                socket_data_handle[5]=Socket_instruction_6 ;
                socket_data_handle[6]=0;
                socket_data_handle[7]=0x11;
                socket_data_handle[8]=pRecv[8+handle_deviation];
                socket_data_handle[9]=SLAVE_REQ_REQ_CALL_RETURN_2;  //同意
                memcpy(socket_data_handle+10 , pRecv+10+handle_deviation ,15);
                KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);

                memset(&msg_audio_mp3, 0x00, sizeof(MSG_S));
                msg_audio_mp3.messageID = Msg_Evt_MP3_STOP_AUDIO_START;
                KING_MsgSend(&msg_audio_mp3, MP3_msgHandle);

                memcpy(uart3_write,"cmd\x02",4);
                uart3_write[4] = pRecv[8+handle_deviation]-0x30;
                uart3_write[5] = 1;
                memset(&msg_audio, 0x00, sizeof(MSG_S));
                msg_audio.messageID = MSG_EVT_UART3_SEND;
                msg_audio.pData = (uint8*)uart3_write;
                msg_audio.DataLen = 10;
                KING_MsgSend(&msg_audio, msgHandle_audio);
            break;
        }
        case DIAL_REQ_REQ_CALL_2:{      //挂断
            case_DIAL_REQ_REQ_CALL_2:

            if(mail_list_id){
                mail_list_id=0;
                break;
            }
            call_user_operation=0;
            if(call_user_tim>Call_max_tim-2){
                msg_audio.messageID = MSG_EVT_HMI_NEWS3;
                // msg_audio.pData = pcmdata;
                // msg_audio.DataLen = HMI_Aktion_Enum_14;
                KING_MsgSend(&msg_audio, msgHandle_audio);
            }else{
                msg_audio.messageID = MSG_EVT_HMI_NEWS2;
                // msg_audio.pData = pcmdata;
                // msg_audio.DataLen = HMI_Aktion_Enum_14;
                KING_MsgSend(&msg_audio, msgHandle_audio);                
            }


            call_user_tim=0;
            memcpy(uart3_write,"cmd\x02",4);
            uart3_write[4] = pRecv[8+handle_deviation]-0x30;
            uart3_write[5] = 0;
            memset(&msg_audio, 0x00, sizeof(MSG_S));
            msg_audio.messageID = MSG_EVT_UART3_SEND;
            msg_audio.pData = &uart3_write;
            msg_audio.DataLen = 10;
            KING_MsgSend(&msg_audio, msgHandle_audio);

            break;
        }
        case DIAL_REQ_REQ_CALL_4:{      //拒绝
            goto case_DIAL_REQ_REQ_CALL_2;
            break;
        }
    }

}
void call_overtime_fun(void){   //超时自动挂断
        // 1发出呼叫 2等待接通 3对方接通 4语音数据 5被拨号
    MSG_S msg_audio;
    // MSG_S msg_audio_mp3;
    call_user_tim++;
    int32 ret=0;
    //uint8 uart3_write[10]={0};
    LogPrintf("MY_APP_CALL_res_t:%d,%d,%d",call_user_operation,call_user_tim,heartbeat_tim);
    
    switch (call_user_operation){
        case 1:{
            if(call_user_tim%3==0){
                socket_data_handle[5]=Socket_instruction_3 ; 
                socket_data_handle[6]=0;
                socket_data_handle[7]=0x11;
                socket_data_handle[8]=mail_list_road;       
                socket_data_handle[9]=DIAL_REQ_REQ_CALL_1;    //拨号内容:呼叫
                memcpy(socket_data_handle+10,mail_list[0],15);
                ret = KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);
                if(!new_connect(ret)){};
            }
            if(call_user_tim>Call_max_tim){
                socket_data_handle[5]=Socket_instruction_3 ; 
                socket_data_handle[6]=0;
                socket_data_handle[7]=0x11;
                socket_data_handle[8]=mail_list_road;       
                socket_data_handle[9]=DIAL_REQ_REQ_CALL_2;    //拨号内容:取消
                memcpy(socket_data_handle+10,mail_list[0],15);
                ret = KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);
                if(!new_connect(ret)){};

                // memset(&msg_audio_mp3, 0x00, sizeof(MSG_S));
                // msg_audio_mp3.messageID = Msg_Evt_MP3_AUDIO;
                // msg_audio_mp3.pData=(char*)"/app/system/audio/end_conv.mp3";
                // KING_MsgSend(&msg_audio_mp3, MP3_msgHandle);

                msg_audio.messageID = MSG_EVT_HMI_NEWS2;
                // msg_audio.pData = pcmdata;
                // msg_audio.DataLen = HMI_Aktion_Enum_14;
                KING_MsgSend(&msg_audio, msgHandle_audio);

                memcpy(uart3_write,"cmd\x02",4);
                uart3_write[4] = pRecv[8+handle_deviation]-0x30;
                uart3_write[5] = 0;
                memset(&msg_audio, 0x00, sizeof(MSG_S));
                msg_audio.messageID = MSG_EVT_UART3_SEND;
                msg_audio.pData = uart3_write;
                msg_audio.DataLen = 10;
                KING_MsgSend(&msg_audio, msgHandle_audio);

                call_user_tim=0;
                call_user_operation=0;
                
                /*  清空变量  */
                audio_data_num_send=0;      //已发送数据个数
                mail_list_road1=0;
                mail_list_road2=0;
                mail_list_road3=0;
                mail_list_road4=0;
                handle_deviation=0;      //已经处理的数据个数
                Current_data_length=0;   //一帧需要包含的数据个数
                Current_data_length_save=0;   //一帧已经接收的数据个数
                analysis_handle=0;       //解析得到的数据头
                mail_list_id=0;
                
            }
            break;
        }
        case 2:{
            if(call_user_tim==Call_max_tim/2){   //  呼叫第二分机
				if(mail_list[1][0]!=0){ //有第二级
                    socket_data_handle[5]=Socket_instruction_3 ; 
                    socket_data_handle[6]=0;
                    socket_data_handle[7]=0x11;
                    socket_data_handle[8]=mail_list_road;       
                    socket_data_handle[9]=DIAL_REQ_REQ_CALL_2;    //拨号内容:取消
                    memcpy(socket_data_handle+10,mail_list[0],15);
                    KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);
                                    
                    socket_data_handle[5]=Socket_instruction_3 ;
                    socket_data_handle[6]=0;
                    socket_data_handle[7]=0x11;
                    socket_data_handle[8]=mail_list_road;       
                    socket_data_handle[9]=DIAL_REQ_REQ_CALL_1;    //拨号内容:呼叫
                    memcpy(socket_data_handle+10,mail_list[1],15);
                    ret = KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);
                if(!new_connect(ret)){};								
                }else{
                    socket_data_handle[5]=Socket_instruction_3 ; 
                    socket_data_handle[6]=0;
                    socket_data_handle[7]=0x11;
                    socket_data_handle[8]=mail_list_road;       
                    socket_data_handle[9]=DIAL_REQ_REQ_CALL_2;    //拨号内容:取消
                    memcpy(socket_data_handle+10,mail_list[0],15);
                    KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);
                                    
                    socket_data_handle[5]=Socket_instruction_3 ;
                    socket_data_handle[6]=0;
                    socket_data_handle[7]=0x11;
                    socket_data_handle[8]=mail_list_road;       
                    socket_data_handle[9]=DIAL_REQ_REQ_CALL_1;    //拨号内容:呼叫
                    memcpy(socket_data_handle+10,mail_list[0],15);
                    ret = KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);
                if(!new_connect(ret)){};
                }
            }
            if(call_user_tim > Call_max_tim){
                socket_data_handle[5]=Socket_instruction_3 ; 
                socket_data_handle[6]=0;
                socket_data_handle[7]=0x11;
                socket_data_handle[8]=mail_list_road;       
                socket_data_handle[9]=DIAL_REQ_REQ_CALL_2;    //拨号内容:取消
                memcpy(socket_data_handle+10,mail_list[1],15);
                ret = KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);
                if(!new_connect(ret)){};

                // memset(&msg_audio_mp3, 0x00, sizeof(MSG_S));
                // msg_audio_mp3.messageID = Msg_Evt_MP3_AUDIO;
                // msg_audio_mp3.pData=(char*)"/app/system/audio/end_conv.mp3";
                // KING_MsgSend(&msg_audio_mp3, MP3_msgHandle);

                msg_audio.messageID = MSG_EVT_HMI_NEWS2;
                // msg_audio.pData = pcmdata;
                // msg_audio.DataLen = HMI_Aktion_Enum_14;
                KING_MsgSend(&msg_audio, msgHandle_audio);
                
                memcpy(uart3_write,"cmd\x02",4);
                uart3_write[4] = pRecv[8+handle_deviation]-0x30;
                uart3_write[5] = 0;
                memset(&msg_audio, 0x00, sizeof(MSG_S));
                msg_audio.messageID = MSG_EVT_UART3_SEND;
                msg_audio.pData = uart3_write;
                msg_audio.DataLen = 10;
                KING_MsgSend(&msg_audio, msgHandle_audio);

                call_user_tim=0;
                call_user_operation=0;
                /*  清空变量  */
                audio_data_num_send=0;      //已发送数据个数
                mail_list_road1=0;
                mail_list_road2=0;
                mail_list_road3=0;
                mail_list_road4=0;
                handle_deviation=0;      //已经处理的数据个数
                Current_data_length=0;   //一帧需要包含的数据个数
                Current_data_length_save=0;   //一帧已经接收的数据个数
                analysis_handle=0;       //解析得到的数据头
                mail_list_id=0;
            }
            break;
        }
        case 3:{
            if(heartbeat_tim>Call_reception_timeout){
                KING_SocketClose(s_socketFd);
                call_user_tim=0;
                call_user_operation=0;

                memcpy(uart3_write,"cmd\x02",4);
                uart3_write[4] = pRecv[8+handle_deviation]-0x30;
                uart3_write[5] = 0;
                memset(&msg_audio, 0x00, sizeof(MSG_S));
                msg_audio.messageID = MSG_EVT_UART3_SEND;
                msg_audio.pData = uart3_write;
                msg_audio.DataLen = 10;
                KING_MsgSend(&msg_audio, msgHandle_audio);

                /*  清空变量  */
                audio_data_num_send=0;      //已发送数据个数
                mail_list_road1=0;
                mail_list_road2=0;
                mail_list_road3=0;
                mail_list_road4=0;
                handle_deviation=0;      //已经处理的数据个数
                Current_data_length=0;   //一帧需要包含的数据个数
                Current_data_length_save=0;   //一帧已经接收的数据个数
                analysis_handle=0;       //解析得到的数据头
                mail_list_id=0;
            }
            if(call_user_tim>Call_max_tim){
                socket_data_handle[5]=Socket_instruction_3 ; 
                socket_data_handle[6]=0;
                socket_data_handle[7]=0x11;
                socket_data_handle[8]=mail_list_road;       
                socket_data_handle[9]=DIAL_REQ_REQ_CALL_2;    //拨号内容:取消
                memcpy(socket_data_handle+10,mail_list[1],15);
                ret = KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);
                if(!new_connect(ret)){};

                msg_audio.messageID = MSG_EVT_HMI_NEWS2;
                // msg_audio.pData = pcmdata;
                // msg_audio.DataLen = HMI_Aktion_Enum_14;
                KING_MsgSend(&msg_audio, msgHandle_audio);

                call_user_tim=0;
                call_user_operation=0;

                memcpy(uart3_write,"cmd\x02",4);
                uart3_write[4] = pRecv[8+handle_deviation]-0x30;
                uart3_write[5] = 0;
                memset(&msg_audio, 0x00, sizeof(MSG_S));
                msg_audio.messageID = MSG_EVT_UART3_SEND;
                msg_audio.pData = uart3_write;
                msg_audio.DataLen = 10;
                KING_MsgSend(&msg_audio, msgHandle_audio);

                /*  清空变量  */
                audio_data_num_send=0;      //已发送数据个数
                mail_list_road1=0;
                mail_list_road2=0;
                mail_list_road3=0;
                mail_list_road4=0;
                handle_deviation=0;      //已经处理的数据个数
                Current_data_length=0;   //一帧需要包含的数据个数
                Current_data_length_save=0;   //一帧已经接收的数据个数
                analysis_handle=0;       //解析得到的数据头
                mail_list_id=0;

                KING_SocketClose(s_socketFd);
                s_socketFd = -1;
            }
            break;
        }
        case 5:{
            if(heartbeat_tim>Call_reception_timeout){
                KING_SocketClose(s_socketFd);

                msg_audio.messageID = MSG_EVT_HMI_NEWS2;
                KING_MsgSend(&msg_audio, msgHandle_audio);

                call_user_tim=0;
                call_user_operation=0;

                memcpy(uart3_write,"cmd\x02",4);
                uart3_write[4] = pRecv[8+handle_deviation]-0x30;
                uart3_write[5] = 0;
                memset(&msg_audio, 0x00, sizeof(MSG_S));
                msg_audio.messageID = MSG_EVT_UART3_SEND;
                msg_audio.pData = uart3_write;
                msg_audio.DataLen = 10;
                KING_MsgSend(&msg_audio, msgHandle_audio);

                /*  清空变量  */
                audio_data_num_send=0;      //已发送数据个数
                mail_list_road1=0;
                mail_list_road2=0;
                mail_list_road3=0;
                mail_list_road4=0;
                handle_deviation=0;      //已经处理的数据个数
                Current_data_length=0;   //一帧需要包含的数据个数
                Current_data_length_save=0;   //一帧已经接收的数据个数
                analysis_handle=0;       //解析得到的数据头
                mail_list_id=0;

                /*  清空变量  */
                audio_data_num_send=0;      //已发送数据个数
                mail_list_road1=0;
                mail_list_road2=0;
                mail_list_road3=0;
                mail_list_road4=0;
                handle_deviation=0;      //已经处理的数据个数
                Current_data_length=0;   //一帧需要包含的数据个数
                Current_data_length_save=0;   //一帧已经接收的数据个数
                analysis_handle=0;       //解析得到的数据头
                mail_list_id=0;
            }
            if(call_user_tim>Call_max_tim){
                socket_data_handle[5]=Socket_instruction_6 ; 
                socket_data_handle[6]=0;
                socket_data_handle[7]=0x11;
                socket_data_handle[8]=mail_list_road;       
                socket_data_handle[9]=SLAVE_REQ_REQ_CALL_RETURN_5;    //拨号内容:挂断
                memcpy(socket_data_handle+10,mail_list[1],15);
                ret = KING_SocketSend(s_socketFd,socket_data_handle,10+15,0);
                if(!new_connect(ret)){};

                msg_audio.messageID = MSG_EVT_HMI_NEWS2;
                // msg_audio.pData = pcmdata;
                // msg_audio.DataLen = HMI_Aktion_Enum_14;
                KING_MsgSend(&msg_audio, msgHandle_audio);

                call_user_tim=0;
                call_user_operation=0;

                memcpy(uart3_write,"cmd\x02",4);
                uart3_write[4] = pRecv[8+handle_deviation]-0x30;
                uart3_write[5] = 0;
                memset(&msg_audio, 0x00, sizeof(MSG_S));
                msg_audio.messageID = MSG_EVT_UART3_SEND;
                msg_audio.pData = uart3_write;
                msg_audio.DataLen = 10;
                KING_MsgSend(&msg_audio, msgHandle_audio);

                /*  清空变量  */
                audio_data_num_send=0;      //已发送数据个数
                mail_list_road1=0;
                mail_list_road2=0;
                mail_list_road3=0;
                mail_list_road4=0;
                handle_deviation=0;      //已经处理的数据个数
                Current_data_length=0;   //一帧需要包含的数据个数
                Current_data_length_save=0;   //一帧已经接收的数据个数
                analysis_handle=0;       //解析得到的数据头
                mail_list_id=0;
            }
            break;
        }
        // default:{
        //     call_user_tim=0;
        //     call_user_operation=0;
            
        //     /*  清空变量  */
        //     audio_data_num_send=0;      //已发送数据个数
        //     mail_list_road1=0;
        //     mail_list_road2=0;
        //     mail_list_road3=0;
        //     mail_list_road4=0;
        //     handle_deviation=0;      //已经处理的数据个数
        //     Current_data_length=0;   //一帧需要包含的数据个数
        //     Current_data_length_save=0;   //一帧已经接收的数据个数
        //     analysis_handle=0;       //解析得到的数据头
        //     mail_list_id=0;
        // }
    }
}
void fill_audio(uint8* Pdat ,uint32 lines){      //填充音频播放数组
    /*   循环填充数组   */
    if(lines<=1){
        return;
    }
    if(play_data_num+lines>PLAY_DATA_MAX){
        memcpy(play_data+play_data_num, Pdat,PLAY_DATA_MAX-play_data_num);
        memcpy(play_data, Pdat+(PLAY_DATA_MAX-play_data_num),lines-(PLAY_DATA_MAX-play_data_num));
    }else{
        memcpy(play_data+play_data_num, Pdat,lines);
    }
    //  记录数据个数
    play_data_num+=lines;
    if(play_data_num>=PLAY_DATA_MAX){
        play_data_num-= PLAY_DATA_MAX;
    }
    play_data_new+=lines;
    // 修改为80个
    if(play_data_new>=80){
        play_data_en+=play_data_new/(80);   //收到数据标志
        play_data_new=play_data_new%(80);  //重新计数
    }
}
uint32 new_connect(uint32 sign){     //判断是否发送成功
    if(sign == -1){
       LogPrintf("KING_SocketBind fail\r\n");
        KING_SocketClose(s_socketFd);
        // s_socketFd=-1;
        return 0;   //重连
    }
    return 1;
}


/*********************
    u8 data[30]={0};
    memcpy(data,"0,sdfada16168.com:123",23);
    FS_Get_Ip(data+10,15);
**************************/
// 写入IP地址
void FS_Get_Ip(uint8 *set,uint8 length) // 字符串地址 长度
{
    return;

}

// 双卡使能 //需要在回调中APP自行决定 是否执行切换
void Dual_Sim(void){
    uint8 enable=0;
    SIM_ID ID;
    if(KING_DualSimInit()==0){ //是否支持双卡功能
        LogPrintf("KING_DualSimInit ok");
        if( KING_DualSimGet (&enable,&ID)==0){ //是否开启双卡功能
            LogPrintf("King_DualSinGet ok enable:%d,id:%d",enable,ID); 
            KING_DualSimCurrent(&ID); //当前业务卡id
            LogPrintf("KING_DualSimCurrent ok id:%d",ID);
            if(enable==0){ //没有开启双卡功能
                if(KING_DualSimSet(1,SIM_0)==0){ //开启双卡功能
                    LogPrintf("KING_DualSimSet ok Sim0");
                    KING_Sleep(10);
                    KING_PowerReboot();
                }else{
                    LogPrintf("KING_DualSimSet err Sim0");
                }
            }
        }else{
            LogPrintf("KING_DualSimGet Err");
        }
    } else{
        LogPrintf("king_Dual_SimInit Err");
    }
}