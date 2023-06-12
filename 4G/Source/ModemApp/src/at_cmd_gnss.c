#include "At_errcode.h"
#include "at_common.h"
#include "At_module.h"
#include "at_utils.h"
#include "at_cmd_utils.h"
#include "KingDef.h"
#include "KingPlat.h"
#include "KingRtos.h"
#include "KingUart.h"
#include "KingGpio.h"
#include "kingsocket.h"
#include "kingcbdata.h"
#include "at_cmd_gnss.h"

#define NMEA_GNGGA  "$GNGGA"
#define NMEA_GPGSA  "$GPGSA"
#define NMEA_END  "\r\n"
#define GPIO_GNSS_POWER    14
#define TCPIP_CONTEXT_ID_MAX_NUM            4
#define  AGPS_HOST    "121.41.40.95"
#define  AGPS_PORT    2621
#define  AGPS_REQ     "user=freetrial;pwd=123456;cmd=full;gnss=gps;lat=24.48;lon=118.07;"
#define  GPS_UART  UART_3

static GPS_NMEA_DATA_T nmeaData;
static GPS_LOC_DATA_T locData;
static NMEA_RING_BUFF_T ringBuff;
static GNSS_TASK_MSG_S  gTaskInfo;
static uint8_t gGnssState = 0; //0: power off; 1: power on; 2: receiving nmea data;
static int32_t gAgpsSocket = -1;
static uint8_t gAgpsflag = 0;
static uint8_t gGnssInitFlag = 0;

static uint8_t GnssStateGet(void);
static void GnssStateSet(uint8_t state);
static int Uart3Init(void);
static int8 GnssTaskInit(void);
static int32 AgpsHandleSockRsp(uint32 eventID, void* pdata, uint32 len);
#if 0
static void AgpsPrintfHex(char *title, uint8 *buf, int32 buf_len);
#endif
static void AgpsConnectClose(void)
{
    if(gAgpsSocket != -1)
    {
        KING_SocketClose(gAgpsSocket);
        gAgpsSocket = -1;
        KING_UnRegNotifyFun(DEV_CLASS_SOCKET, gAgpsSocket, (NOTIFY_FUN)AgpsHandleSockRsp);
    }
}

static int32 AgpsHandleSockRsp(uint32 eventID, void* pdata, uint32 len)
{
    int32 ret = FAIL;
    MSG_S msg = {0};
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;
    PRD_LOG("eventID=0x%X", eventID);
    switch (eventID)
    {
        case SOCKET_CONNECT_EVENT_RSP:
            PRD_LOG("SOCKET_CONNECT_EVENT_RSP");
            if (gAgpsSocket == data->socket.socketID)
            {
                memset(&msg, 0x00, sizeof(msg));
                msg.messageID = MSG_AGPS_SOCKET_CONNECT_RSP;
                if (gTaskInfo.msgHandle == NULL)
                {
                    PRD_LOG("gnss thread is not ready!");
                } 
                else 
                {
                    KING_MsgSend(&msg, gTaskInfo.msgHandle);
                }
            }
            else
            {
                PRD_LOG("connect fail. nSocket=%d",data->socket.socketID);
            }

            break;

        case SOCKET_CLOSE_EVENT_RSP:
            PRD_LOG("SOCKET_CLOSE_EVENT_RSP");
            break;

        case SOCKET_ERROR_EVENT_IND:
            PRD_LOG("SOCKET_ERROR_EVENT_IND");
            AgpsConnectClose();
            break;
            
        case SOCKET_FULL_CLOSED_EVENT_IND:
            PRD_LOG("SOCKET_FULL_CLOSED_EVENT_IND");
            gAgpsflag = 1;
            break;

        case SOCKET_READ_EVENT_IND:
            PRD_LOG("EV_CFW_TCPIP_REV_DATA_IND");
            if ((gAgpsSocket == data->socket.socketID) && (1 == gAgpsflag))
            {
                memset(&msg, 0x00, sizeof(msg));
                msg.messageID = MSG_AGPS_SOCKET_READ_RSP;
                msg.DataLen = data->socket.availSize;
                
                PRD_LOG("availSize=%d", data->socket.availSize);
                if (gTaskInfo.msgHandle == NULL)
                {
                    PRD_LOG("gnss thread is not ready!");
                } 
                else
                {
                    KING_MsgSend(&msg, gTaskInfo.msgHandle);
                }
            }
            break;

        default:
            PRD_LOG("TCPIP unexpect asynchrous event/response %d",eventID);
            break;
    }
    return ret;
}


static int32 AgpsConnectServer(SOCK_IN_ADDR_T ip,uint16 port)
{
    int32 ret = FAIL;
    union sockaddr_aligned sock_addr;
    struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);
    char *ipAddr = NULL;

    ret = KING_SocketCreate(AF_INET, SOCK_STREAM, 0, &gAgpsSocket);
    if (ret == FAIL)
    {
        PRD_LOG("KING_SocketCreate() fail");
        goto end;
    }

    memset(&sock_addr, 0, sizeof(struct sockaddr));
    sock_addr4->sin_family = AF_INET;
    sock_addr4->sin_port = KING_htons(port);
    sock_addr4->sin_addr.addr = ip.u_addr.ip4.addr;
    sock_addr4->sin_len = sizeof(struct sockaddr_in);
    KING_ipntoa(ip, &ipAddr);

    PRD_LOG("SocketConnect ip_addr=%u, socket=%d ,ipAddr=%s",sock_addr4->sin_addr, gAgpsSocket,ipAddr);
    ret = KING_SocketConnect(gAgpsSocket, &sock_addr, sizeof(struct sockaddr));
    if (ret == FAIL)
    {
        PRD_LOG("KING_SocketConnect fail");
        KING_SocketClose(gAgpsSocket);
        goto end;
    }
    PRD_LOG("SocketConnect RegNotify socket=%d",gAgpsSocket);
    
    KING_RegNotifyFun(DEV_CLASS_SOCKET, gAgpsSocket, (NOTIFY_FUN)AgpsHandleSockRsp);

end:
    return ret;
}



static void AgpsHandle(void)
{
    SOCK_IN_ADDR_T serverIp = {0};
    uint8  cid;
    uint32 value;
    
    for(cid = 1; cid <= TCPIP_CONTEXT_ID_MAX_NUM; cid++)
    {
        KING_NetPdpStatusGet(cid, &value);
        if(value)
        {
            break;
        }
    }

    if(cid > TCPIP_CONTEXT_ID_MAX_NUM)
    {
        int ret = 0;
        int8 count = 10;
        PRD_LOG("NO PDP ACTIVED!!!");
        
        ret = KING_NetPdpActive(1);
        if (ret == -1)
        {
            PRD_LOG("KING_NetPdpActive fail");
            return;
        }
        while(count > 0)
        {
            KING_NetPdpStatusGet(1, &value);
            if(value)
            {
                break;
            }
            KING_Sleep(100);
            count--;
        }
    }
    
    PRD_LOG("value=%d", value);
    if(value)
    {
        KING_ipaton(AGPS_HOST, &serverIp);
        if (AgpsConnectServer(serverIp, AGPS_PORT) != 0)
        {
            AgpsConnectClose();
        }

    }
}

static void GnssPowerOnOff(bool flag)
{
    //enable gnss
    if(flag)
    {
        KING_GpioSet(GPIO_GNSS_POWER, 1);
        GnssStateSet(1);
    }
    else
    {
        KING_GpioSet(GPIO_GNSS_POWER, 0);
        GnssStateSet(0);
    }
}

static uint8_t GnssStateGet(void)
{
    return gGnssState;
}

static void GnssStateSet(uint8_t state)
{
    gGnssState = state;
}

static void DecodeNmeaData(void)
{
    char *pstart = NULL;
    char *pend = NULL;
    char *ptmp = NULL;
    char nmeastr[NMEA_BUFEER_LEN+1];
    uint16 len = 0;
    uint8_t state;

    memset(nmeastr, 0x00, NMEA_BUFEER_LEN+1);
    len = ringBuff.count;
    
    PRD_LOG("len=%d, head=%d, tail=%d", len, ringBuff.head, ringBuff.tail);
    if(len == 0)
    {
        return;
    }

    if(ringBuff.tail > ringBuff.head)
    {
        memcpy(nmeastr, ringBuff.buffer+ringBuff.head, len);
    }
    else
    {
        memcpy(nmeastr, ringBuff.buffer+ringBuff.head, (NMEA_BUFEER_LEN-ringBuff.head));
        memcpy(nmeastr+(NMEA_BUFEER_LEN-ringBuff.head), ringBuff.buffer, len-(NMEA_BUFEER_LEN-ringBuff.head));
    }
    pstart = nmeastr;
    ptmp = strstr(pstart, NMEA_END);
    if((NULL == ptmp) && (len > GPS_NMEA_LEN_MAX))
    {
        memset(&ringBuff, 0x00, sizeof(ringBuff));
    }
    while(ptmp != NULL)
    {
        state = GnssStateGet();
        if (1 == state)
        {
            state = 2;
            GnssStateSet(state);
        }

        len = ptmp - pstart;
        ringBuff.head = ringBuff.head+len+2;
        if(ringBuff.head > NMEA_BUFEER_LEN)
        {
            ringBuff.head = ringBuff.head-NMEA_BUFEER_LEN;
        }
        ringBuff.count = ringBuff.count-len-2;

        //KING_SysLog("head=%d, tail=%d,count=%d", ringBuff.head, ringBuff.tail, ringBuff.count);
        //PRD_LOG("pstart=%s", pstart);
        if(strncmp(pstart, NMEA_GNGGA, strlen(NMEA_GNGGA)) == 0)
        {
            memset(nmeaData.gga, 0x00, sizeof(nmeaData.gga));
            strncpy(nmeaData.gga, pstart, len);
            PRD_LOG("gga=%s", nmeaData.gga);
            pstart = strchr(pstart, ',');
            pstart = pstart+1;
            pend = strchr(pstart, ',');
            strncpy(locData.utc_time, pstart, pend-pstart);
            locData.utc_time[9] = '0';    
            pstart = pend+1;
            pend = strchr(pstart, ',');
            pend = pend + 1;
            pend = strchr(pend, ',');
            strncpy(locData.lat, pstart, pend-pstart);
            pstart = pend+1;
            pend = strchr(pstart, ',');
            pend = pend + 1;
            pend = strchr(pend, ',');
            strncpy(locData.lon, pstart, pend-pstart);
            pstart = pend+1;
            pstart = strchr(pstart, ',');
            pstart = pstart + 1;
            pend = strchr(pstart, ',');
            strncpy(locData.nsat, pstart, pend-pstart);
            pstart = pend + 1;
            pend = strchr(pstart, ',');
            strncpy(locData.hop, pstart, pend-pstart);
            pstart = pend + 1;
            pend = strchr(pstart, ',');
            strncpy(locData.alti, pstart, pend-pstart);
        }
        else if(strncmp(pstart, NMEA_GPGSA, strlen(NMEA_GPGSA)) == 0)
        {
            memset(nmeaData.gsa, 0x00, sizeof(nmeaData.gsa));
            strncpy(nmeaData.gsa, pstart, len);
            PRD_LOG("gsa=%s", nmeaData.gsa);
            pstart = strchr(pstart, ',');
            pstart = pstart + 1;
            pstart = strchr(pstart, ',');
            pstart = pstart + 1;
            pend = strchr(pstart, ',');
            strncpy(locData.fix, pstart, pend-pstart);
        }
        else if(strncmp(pstart, "$GNVTG", strlen("$GNVTG")) == 0)
        {
            memset(nmeaData.vtg, 0x00, sizeof(nmeaData.vtg));
            strncpy(nmeaData.vtg, pstart, len);
            PRD_LOG("vtg=%s", nmeaData.vtg);
            pstart = strchr(pstart, ',');
            pstart = pstart + 1;
            pend = strchr(pstart, ',');
            strncpy(locData.cog, pstart, pend-pstart);
            pstart = pend + 1;
            pstart = strchr(pstart, ',');
            pstart = pstart + 1;

            pstart = strchr(pstart, ',');
            pstart = pstart + 1;

            pstart = strchr(pstart, ',');
            pstart = pstart + 1;
            pend = strchr(pstart, ',');
            strncpy(locData.spkn, pstart, pend-pstart);
            pstart = pend + 1;
            pstart = strchr(pstart, ',');
            pstart = pstart + 1;
            pend = strchr(pstart, ',');
            strncpy(locData.spkm, pstart, pend-pstart);

        }
        else if(strncmp(pstart, "$GNRMC", strlen("$GNRMC")) == 0)
        {
            memset(nmeaData.rmc, 0x00, sizeof(nmeaData.rmc));
            strncpy(nmeaData.rmc, pstart, len);
            PRD_LOG("rmc=%s", nmeaData.rmc);
            pstart = strchr(pstart, ',');
            pstart = pstart + 1;
            pstart = strchr(pstart, ',');
            pstart = pstart + 1;
            if(*pstart == 'A')
            {
              locData.bfix = true;
            }
            else
            {
              locData.bfix = false;
            }

            pstart = strchr(pstart, ',');
            pstart = pstart + 1;

            pstart = strchr(pstart, ',');
            pstart = pstart + 1;

            pstart = strchr(pstart, ',');
            pstart = pstart + 1;

            pstart = strchr(pstart, ',');
            pstart = pstart + 1;   

            pstart = strchr(pstart, ',');
            pstart = pstart + 1;

            pstart = strchr(pstart, ',');
            pstart = pstart + 1;  

            pstart = strchr(pstart, ',');
            pstart = pstart + 1;
            pend = strchr(pstart, ',');

            strncpy(locData.data, pstart, pend-pstart);
        }
        else if(strncmp(pstart, "$GPGSV", strlen("$GPGSV")) == 0)
        {
            char *pgsv = pstart;
            pstart = strchr(pstart, ',');
            pstart = pstart + 1;

            pstart = strchr(pstart, ',');
            pstart = pstart + 1;
            PRD_LOG("*pstart=%c", *pstart);
            if(*pstart == '1')
            {
              memset(nmeaData.gsv1, 0x00, sizeof(nmeaData.gsv1));
              strncpy(nmeaData.gsv1, pgsv, len);
              PRD_LOG("gsv1=%s", nmeaData.gsv1);
              // 4 ssts
              pstart = strchr(pstart, ',');
              pstart = pstart + 1;

              // 5 prn
              pstart = strchr(pstart, ',');
              pstart = pstart + 1;

              // 6 elev
              pstart = strchr(pstart, ',');
              pstart = pstart + 1;

              // 7 azimuth
              pstart = strchr(pstart, ',');
              pstart = pstart + 1;

              // 8 snr1
              pstart = strchr(pstart, ',');
              pstart = pstart + 1;
              pend = pstart;

              // 9 wxh1
              pstart = strchr(pstart, ',');

              if(NULL == pstart)
              {
                  locData.snr_store[0] = 0;
                  goto _gsvexit;
              }

              if(*pend == ',')
              {
                  locData.snr_store[0] = 0;
              }
              else
              {
                  locData.snr_store[0] = ((*pend-'0')*10)+(*(pend+1)-'0');
              }

              pstart = pstart + 1;
              // 10 yj1
              pstart = strchr(pstart, ',');

              if(NULL == pstart)
              {              
                  goto _gsvexit;
              }

              pstart = pstart + 1;
              // 11 fwj1
              pstart = strchr(pstart, ',');
              if(NULL == pstart)
              {
                  goto _gsvexit;
              }

              pstart = pstart+1;
              // 12 snr2
              pstart = strchr(pstart, ',');
              if(NULL == pstart)
              {
                  goto _gsvexit;
              }

              pstart = pstart+1;
              pend = pstart;
              // 13 wxh2
              pstart = strchr(pstart, ',');
              if(NULL == pstart)
              {
                  goto _gsvexit;
              }

              if(*pend == ',')
              {
                  locData.snr_store[1] = 0;
              }
              else
              {
                  locData.snr_store[1] = ((*pend-'0')*10)+(*(pend+1)-'0');
              }
                  
              pstart = pstart+1;
              // 14 yj2
              pstart = strchr(pstart, ',');
              if(NULL == pstart)
              {              
                  goto _gsvexit;
              }

              pstart = pstart + 1;
              // 15 fwj2
              pstart = strchr(pstart, ',');
              if(NULL == pstart)
              {
                  goto _gsvexit;
              }

              pstart = pstart+1;
              // 16 snr3
              pstart = strchr(pstart, ',');
              if(NULL == pstart)
              {
                  goto _gsvexit;
              }

              pstart = pstart+1;
              pend = pstart;
              // 17 wxh3
              pstart = strchr(pstart, ',');
              if(NULL == pstart)
              {
                  goto _gsvexit;
              }

              if(*pend == ',')
              {
                  locData.snr_store[2] = 0;
              }
              else
              {
                  locData.snr_store[2] = ((*pend-'0')*10)+(*(pend+1)-'0');
              }
                  
              pstart = pstart+1;
              // 18 yj3
              pstart = strchr(pstart, ',');
              if(NULL == pstart)
              {              
                  goto _gsvexit;
              }

              pstart = pstart + 1;
              // 19 fwj3
              pstart = strchr(pstart, ',');
              if(NULL == pstart)
              {
                  goto _gsvexit;
              }

              pstart = pstart+1;
              // 20 snr4
              pstart = strchr(pstart, ',');
              if(NULL == pstart)
              {
                  goto _gsvexit;
              }

              pstart = pstart+1;
              pend = pstart;
              // 21 wxh4
              pstart = strchr(pstart, ',');
              if(NULL == pstart)
              {
                  goto _gsvexit;
              }

              if(*pend == ',')
              {
                  locData.snr_store[3] = 0;
              }
              else
              {
                  locData.snr_store[3] = ((*pend-'0')*10)+(*(pend+1)-'0');
              }
                        
            }
            else if(*pstart == '2')
            {
              memset(nmeaData.gsv2, 0x00, sizeof(nmeaData.gsv2));
              strncpy(nmeaData.gsv2, pgsv, len);
              PRD_LOG("gsv2=%s", nmeaData.gsv2);
            }
            else if(*pstart == '3')
            {
              memset(nmeaData.gsv3, 0x00, sizeof(nmeaData.gsv3));
              strncpy(nmeaData.gsv3, pgsv, len);
            }
            else if(*pstart == '4')
            {
              memset(nmeaData.gsv4, 0x00, sizeof(nmeaData.gsv4));
              strncpy(nmeaData.gsv4, pgsv, len);
            }

        }
        _gsvexit:

        pstart = ptmp+2;
        ptmp = strstr(pstart, NMEA_END);
        //PRD_LOG("ptmp=0x%x", ptmp);
    }
}

static void UartCb(uint32 eventID, void* pData, uint32 len)
{

    PRD_LOG("eventID=%d, len=%d", eventID, len);
    if (eventID == UART_EVT_DATA_TO_READ)
    {
        MSG_S msg={0};
        
        memset(&msg, 0x00, sizeof(msg));
        msg.messageID = MSG_UART_DATA_READ;
        KING_MsgSend(&msg, gTaskInfo.msgHandle);
    }  
}

static void GnssInit(void)
{
    int ret ;
    GPIO_CFG_S cfg;

    memset(&cfg, 0x00, sizeof(GPIO_CFG_S));
    cfg.default_val = 0;
    cfg.int_type = GPIO_INT_DISABLE;
    cfg.pull_mode = GPIO_PULL_UP;
    cfg.dir = GPIO_DIR_OUTPUT;
    ret = KING_GpioModeSet(GPIO_GNSS_POWER, &cfg);
    if (FAIL == ret)
    {
        PRD_LOG("GPIO_GNSS_POWER Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    memset(&nmeaData, 0x00, sizeof(nmeaData));
    memset(&locData, 0x00, sizeof(locData));
    memset(&ringBuff, 0x00, sizeof(ringBuff));
    memset(&gTaskInfo, 0x00, sizeof(gTaskInfo));
    GnssTaskInit();
    Uart3Init();
}

static int Uart3Init(void)
{
    int ret = FAIL;
    UART_CONFIG_S cfg;
    cfg.baudrate = UART_BAUD_9600;
    cfg.byte_size = UART_BYTE_LEN_8;
    cfg.flow_ctrl = UART_NO_FLOW_CONTROL;
    cfg.parity = UART_NO_PARITY;
    cfg.stop_bits = UART_ONE_STOP_BIT; 
    ret = KING_UartInit(GPS_UART, &cfg);

    PRD_LOG("ret=%d\r\n", ret);
    if (FAIL == ret)
    {
        PRD_LOG("KING_UartInit() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
        return ret;
    }
    ret = KING_RegNotifyFun(DEV_CLASS_UART, GPS_UART, UartCb);
    if (FAIL == ret)
    {
        PRD_LOG("KING_RegNotifyFun() Failed! errcode=0x%x\r\n", KING_GetLastErrCode());
    }
    return ret;
}

#if 0
static void AgpsPrintfHex(char *title, uint8 *buf, int32 buf_len)
{
    char temp[128];
    int32 i;

    memset(temp, 0, sizeof(temp));
    PRD_LOG("%s hex len:%d", title, buf_len);
    if (buf_len <= 0)
    {
        return;
    }

    for (i = 0; i < buf_len; i++)
    {
        sprintf(temp + strlen(temp),"%02X ",buf[i]);
        if ((i%16) == 15)
        {
            PRD_LOG("%s",temp);
            memset(temp, 0, sizeof(temp));
        }
    }
    PRD_LOG("%s",temp);
}
#endif

static void AgpsDataRead(uint32 len)
{
    uint8  *data = NULL;
    uint32 rlen = 0;
    int32 tmp = 0;
    MSG_S msg;

    PRD_LOG("len=%d", len);
    data = malloc(len+1);
    if(data != NULL)
    {
        memset(data, 0x00, len+1);
        while(rlen < len)
        {
            tmp = KING_SocketRecv(gAgpsSocket, data, len, 0);
            
            PRD_LOG("tmp=%d", tmp);
            if(tmp > 0)
            {
                rlen = rlen+tmp;
            }
            else
            {
                break;
            }
        }
        memset(&msg, 0x00, sizeof(msg));
        msg.messageID = MSG_AGPS_DATA_UART_WRITE;
        msg.pData = data;
        msg.DataLen = rlen;
        KING_MsgSend(&msg, gTaskInfo.msgHandle);
        
        PRD_LOG("data=0x%x, rlen=%ld, data[0]=0x%x", data, rlen, data[0]);
        #if 0
        AgpsPrintfHex("agpsdata", data, len);
        #endif
    }
}

static void GnssTaskHandle(uint32 argc, void *argv)
{
    int32 ret = FAIL;
    MSG_S msg = {0};

    PRD_LOG("GnssThread is running...");

    ret = KING_MsgCreate(&gTaskInfo.msgHandle);
    if (ret == FAIL)
    {
        PRD_LOG("GnssThread: Failed to create Msg Handle!");
        KING_ThreadExit();
        return;
    }

    while (1)
    {
        KING_MsgRcv(&msg, gTaskInfo.msgHandle, WAIT_OPT_INFINITE);
        PRD_LOG("messageID=%d, dataLen=%d", msg.messageID, msg.DataLen);

        switch (msg.messageID)
        {
            case MSG_UART_DATA_READ:
            {
                uint32 bytesRead = 0;
                char   tmp[NMEA_BUFEER_LEN];

                memset(tmp, 0x00, NMEA_BUFEER_LEN);
                
                KING_UartRead(GPS_UART, (uint8*)tmp, NMEA_BUFEER_LEN-ringBuff.count, &bytesRead);
                //PRD_LOG("uartstr=%s", tmp);
                PRD_LOG("bytesRead=%d, head=%d, tail=%d,count=%d", bytesRead, ringBuff.head, ringBuff.tail,ringBuff.count);
                if(ringBuff.head <= ringBuff.tail)
                {
                    if((NMEA_BUFEER_LEN-ringBuff.tail) >= bytesRead)
                    {
                        memcpy(ringBuff.buffer+ringBuff.tail, tmp, bytesRead);
                        ringBuff.tail = ringBuff.tail+bytesRead;
                    }
                    else
                    {
                        memcpy(ringBuff.buffer+ringBuff.tail, tmp, (NMEA_BUFEER_LEN-ringBuff.tail));
                        memcpy(ringBuff.buffer, tmp+(NMEA_BUFEER_LEN-ringBuff.tail), bytesRead-(NMEA_BUFEER_LEN-ringBuff.tail));
                        ringBuff.tail = bytesRead-(NMEA_BUFEER_LEN-ringBuff.tail);
                    }
                    ringBuff.count = ringBuff.count+bytesRead;
                }
                else
                {
                    memcpy(ringBuff.buffer+ringBuff.tail, tmp, bytesRead);
                    ringBuff.tail = ringBuff.tail+bytesRead;
                    ringBuff.count = ringBuff.count+bytesRead;
                }

                //PRD_LOG("bytesRead=%d, head=%d, tail=%d,count=%d", bytesRead, ringBuff.head, ringBuff.tail,ringBuff.count);
                DecodeNmeaData();
            }
            break;

            case MSG_AGPS_SOCKET_CONNECT_RSP:
            {
                ret = KING_SocketSend(gAgpsSocket, AGPS_REQ, strlen(AGPS_REQ), 0);
                PRD_LOG("SocketSend ret = %d", ret);
                if(ret == FAIL)
                {
                    AgpsConnectClose();
                }
            }
            break;

            case MSG_AGPS_SOCKET_READ_RSP:
            {
                AgpsDataRead(msg.DataLen);
            }
            break;

            case MSG_AGPS_PDP_ACTIVE:
            {
                AgpsHandle();
            }
            break;

            case MSG_AGPS_DATA_UART_WRITE:
            {
                uint32 len = 0;
                uint8 *data = NULL;
                uint32 wlen = 0; 
                int ret = SUCCESS;
                uint32 totallen = 0;

                len = msg.DataLen;
                data = msg.pData;
                
                //ret = KING_UartWrite(GPS_UART, data, len, &wlen);
                
                //PRD_LOG("ret=%d, data=0x%x, wlen=%ld, len=%ld,data[0]=0x%x", ret, data, wlen, len,data[0]);
                while(totallen < msg.DataLen)
                {
                    if(len > 1024)
                    {
                        ret = KING_UartWrite(GPS_UART, data+totallen, 1024, &wlen);
                    }
                    else
                    {
                        ret = KING_UartWrite(GPS_UART, data+totallen, len, &wlen);
                    }
                    PRD_LOG("ret=%d", ret);
                    if(FAIL == ret)
                    {
                        break;
                    }
                    
                    len = len - wlen;
                    totallen = totallen+wlen;  
                    PRD_LOG("len=%ld, totallen=%ld, wlen=%ld", len, totallen, wlen);
                }
            }
            break;
            
         default:
            break;
        }
        if(msg.pData)
        {
            free(msg.pData);
        }
    }
    KING_ThreadExit();
}

static int8 GnssTaskInit(void)
{
    int8 ret = FAIL;

    PRD_LOG("taskId: %d",gTaskInfo.taskId);
    if (gTaskInfo.taskId == NULL)
    {
        THREAD_ATTR_S threadAttr = {0};

        memset(&threadAttr, 0x00, sizeof(THREAD_ATTR_S));
        threadAttr.fun = (THREAD_ENTRY_FUN)GnssTaskHandle;
        threadAttr.priority = THREAD_PRIORIT1;
        threadAttr.stackSize = 4 * 1024;
    
        ret = KING_ThreadCreate("GnssThread", &threadAttr, &gTaskInfo.taskId);
        if (ret == FAIL)
        {
            PRD_LOG("Create Test Thread Failed!! Errcode=0x%x", KING_GetLastErrCode());
        }
    }

    return ret;
}

static void ProcessLocReport(char *buffer, uint16 len)
{
    char lattemp[15] = {0};
    char lontemp[15] = {0};
    char *ptmp = NULL;

    memset(lattemp, 0x00, sizeof(lattemp));
    if(strlen(locData.lat) > 11)
    {
        strncpy(lattemp, locData.lat, 9);
        ptmp = locData.lat;
        ptmp = strchr(ptmp, ',');
        strcat(lattemp, ptmp);
    }
    memset(lontemp, 0x00, sizeof(lontemp));
    if(strlen(locData.lon) > 11)
    {
        strncpy(lontemp, locData.lon, 10);
        ptmp = locData.lon;
        ptmp = strchr(ptmp, ',');
        strcat(lontemp, ptmp);
    }

    snprintf(buffer,len,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
    locData.utc_time,
    lattemp,
    lontemp,
    locData.hop,
    locData.alti,
    locData.fix,
    locData.cog,
    locData.spkm,
    locData.spkn,
    locData.data,
    locData.nsat);
}

void AT_CmdFunc_QGPS(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_EXE:
        {
            MSG_S msg = {0};
            if(0 == gGnssInitFlag)
            {
                GnssInit();
                gGnssInitFlag = 1;
            }
            GnssPowerOnOff(true);
            if(0 == gAgpsflag)
            {
                memset(&msg, 0x00, sizeof(msg));
                msg.messageID = MSG_AGPS_PDP_ACTIVE;
                KING_MsgSend(&msg, gTaskInfo.msgHandle);
            }
            at_CmdRespOK(pParam->engine);
        }
        break;

        case AT_CMD_READ:
        {
            char rsp[64];
            memset(rsp, 0x00, sizeof(rsp));
            sprintf(rsp, "+QGPS: %d", GnssStateGet());
            at_CmdRespInfoText(pParam->engine, (const uint8_t *)rsp);
            at_CmdRespOK(pParam->engine);
        }
        break;

        case AT_CMD_TEST:
            at_CmdRespOK(pParam->engine);
            break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
            break;
    }
}

void AT_CmdFunc_QGPSEND(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
    case AT_CMD_EXE:
        GnssPowerOnOff(false);
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_READ:
        at_CmdRespOK(pParam->engine);
        break;

    case AT_CMD_TEST:
        at_CmdRespOK(pParam->engine);
        break;

    default:
        at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
        break;
    }
}

void AT_CmdFunc_QGPSLOC(AT_CMD_PARA *pParam)
{
    switch (pParam->iType)
    {
        case AT_CMD_EXE:
        {
            if(0 == GnssStateGet())
            {
                at_CmdRespCmeError(pParam->engine, ERR_AT_CME_EXE_FAIL);
            }
            else
            {
                char rsp[512];
                uint16 len = 0;
                memset(rsp, 0x00, sizeof(rsp));
                strcpy(rsp,"+QGPSLOC:");
                len = strlen(rsp);
                PRD_LOG("bfix=%d", locData.bfix);
                if(locData.bfix)
                {
                    ProcessLocReport(rsp+len, 512-len);  
                }
                else
                {
                    strcat(rsp, "NO FIX");
                }
                at_CmdRespInfoText(pParam->engine, (const uint8_t *)rsp);
                at_CmdRespOK(pParam->engine);
            }
        }
        break;

        case AT_CMD_READ:
        {
            at_CmdRespOK(pParam->engine);
        }
        break;

        case AT_CMD_TEST:
        {
            char rsp[128];
            memset(rsp, 0x00, sizeof(rsp));
            sprintf(rsp, "+QGPSLOC: %s", "<UTC>,<latitude>,<longitude>,<hdop>,<altitude>,<fix>,<cog>,<spkm>,<spkn>,<date>,<nsat>");
            at_CmdRespInfoText(pParam->engine, (const uint8_t *)rsp);
            at_CmdRespOK(pParam->engine);
        }
        break;

        default:
            at_CmdRespCmeError(pParam->engine, ERR_AT_CME_OPERATION_NOT_SUPPORTED);
            break;
    }
}

