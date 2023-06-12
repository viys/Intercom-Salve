/*******************************************************************************
 ** File Name:   mqtt_tcp.c
 ** Author:      Allance.Chen
 ** Date:        2020-06-04
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about mqtt tcp.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-06-04     Allance.Chen         Create.
 ******************************************************************************/
 /*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingsocket.h"
#include "kingdef.h"
#include "kingcstd.h"
#include "kingplat.h"
#include "kingCbData.h"
#include "kingRtos.h"

#include "mqtt_utils.h"

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static int isMqttConnSucc = 0;
static SEM_HANDLE mqttConnSem = NULL;
static int isMqttConnRunning = 0;

/*******************************************************************************
 ** External Function Delcarations
 ******************************************************************************/

/*******************************************************************************
 ** Local Function 
 ******************************************************************************/
static uint64 mqtt_tcp_time_left(uint64 t_end, uint64 t_now)
{
    uint64 t_left;

    if (t_end > t_now) 
    {
        t_left = t_end - t_now;
    } 
    else 
    {
        t_left = 0;
    }

    return t_left;
}

static void mqtt_SocketProcessCb(uint32 eventID, void* pdata, uint32 len)
{
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;
    //uint8 socketFd = (uint8)data->socket.socketID;
    
    KING_SysLog("mqtt_SocketProcessCb() eventID=%d, dataLen=%d\r\n", eventID, len);
    //KING_SysLog("mqtt_SocketProcessCb() socketFd=%d\r\n", socketFd);
    switch (eventID)
    {
        case SOCKET_CONNECT_EVENT_RSP:
        {
            KING_SysLog("mqtt SOCKET_CONNECT_EVENT_RSP\r\n");
            isMqttConnSucc = 1;
            KING_SemPut(mqttConnSem);
        }
            break;

        case SOCKET_ACCEPT_EVENT_IND:
            KING_SysLog("mqtt SOCKET_ACCEPT_EVENT_IND\r\n");
            break;

        case SOCKET_CLOSE_EVENT_RSP:
        {
            KING_SysLog("mqtt SOCKET_CLOSE_EVENT_RSP\r\n");
        }
            break;

        case SOCKET_ERROR_EVENT_IND:
            KING_SysLog("mqtt SOCKET_ERROR_EVENT_IND\r\n");
            if (isMqttConnRunning)
            {
                isMqttConnSucc = 0;
                KING_SemPut(mqttConnSem);
            }
            break;

        case SOCKET_WRITE_EVENT_IND:
            KING_SysLog("mqtt SOCKET_WRITE_EVENT_IND ack by peer uSendDataSize=%d\r\n", data->socket.availSize);
            break;

        case SOCKET_READ_EVENT_IND:
        {
            KING_SysLog("mqtt SOCKET_READ_EVENT_IND ack by peer uSendDataSize=%d\r\n", data->socket.availSize);
        }
            break;
            
        case SOCKET_FULL_CLOSED_EVENT_IND:
            KING_SysLog("mqtt SOCKET_FULL_CLOSED_EVENT_IND\r\n");
            if (isMqttConnRunning)
            {
                isMqttConnSucc = 0;
                KING_SemPut(mqttConnSem);
            }
            break;
            
        default:
            KING_SysLog("mqtt unexpect event/response %d\r\n", eventID);
            break;
    }
}

/*******************************************************************************
 **  Function 
 ******************************************************************************/
int mqtt_tcp_establish(SOCK_IN_ADDR_T *pHostAddr, uint16 port)
{
    int fd = 0;
    union sockaddr_aligned sock_addr;
    uint32 sockaddr_size = 0;
    int ret = 0;
    int protocol = 0;
    int addr_family;
    int sock_type = 0;
    
    if (isMqttConnRunning == 1)
    {
        return -1;
    }

    KING_SysLog("establish tcp connection ");
    if (pHostAddr->type == IP_ADDR_TYPE_V6)
    {
        addr_family = AF_INET6;
    }
    else
    {
        addr_family = AF_INET;
    }
    sock_type = SOCK_STREAM;
    protocol = IPPROTO_TCP;
    
    ret = KING_SocketCreate(addr_family, sock_type, protocol, &fd);
    if (ret < 0 || fd < 0) 
    {
        KING_SysLog("mqtt_tcp_establish create socket error");
        return -1;
    }
    
    KING_RegNotifyFun(DEV_CLASS_SOCKET, fd, mqtt_SocketProcessCb);
    
    if (pHostAddr->type == IP_ADDR_TYPE_V6)
    {
        struct sockaddr_in6 *sock_addr6 = (struct sockaddr_in6 *)&(sock_addr);
        
        sock_addr6->sin6_len = sizeof(struct sockaddr_in6);
        sock_addr6->sin6_family = AF_INET6;
        sock_addr6->sin6_port = KING_htons(port);
        sock_addr6->sin6_addr.addr[0] = pHostAddr->u_addr.ip6.addr[0];
        sock_addr6->sin6_addr.addr[1] = pHostAddr->u_addr.ip6.addr[1];
        sock_addr6->sin6_addr.addr[2] = pHostAddr->u_addr.ip6.addr[2];
        sock_addr6->sin6_addr.addr[3] = pHostAddr->u_addr.ip6.addr[3];
        
        sockaddr_size = sizeof(struct sockaddr_in6);
    }
    else
    {
        struct sockaddr_in *sock_addr4 = (struct sockaddr_in *)&(sock_addr);
        
        sock_addr4->sin_len = sizeof(struct sockaddr_in);
        sock_addr4->sin_family = AF_INET;
        sock_addr4->sin_port = KING_htons(port);
        sock_addr4->sin_addr.addr = pHostAddr->u_addr.ip4.addr;
        KING_SysLog("mqtt_tcp_establish ip_addr=%u\r\n", sock_addr4->sin_addr.addr);

        sockaddr_size = sizeof(SOCK_ADDR_IN_T);
    }

    if (NULL == mqttConnSem)
    {
        ret = KING_SemCreate("MQTT CONN SEM", 0, (SEM_HANDLE *)&mqttConnSem);
        KING_SysLog("%s: KING_SemCreate() ret = %d\r\n", __FUNCTION__, ret);
        if (ret < 0 || mqttConnSem == NULL)
        {
            return -1;
        }
        //KING_SemGet(mqttConnSem, 0);
    }

    ret = KING_SocketConnect(fd, &sock_addr, sockaddr_size);
    if (ret < 0)
    {
        KING_SysLog("mqtt_tcp_establish connect fail\r\n");
        KING_SocketClose(fd);
        return -1;
    }
    
    isMqttConnRunning = 1;
    KING_SemGet(mqttConnSem, WAIT_OPT_INFINITE);
    isMqttConnRunning = 0;
    KING_SemDelete(mqttConnSem);
    mqttConnSem = NULL;
    if (isMqttConnSucc == 0)
    {
        KING_SysLog("mqtt_tcp_establish connect fail\r\n");
        KING_SocketClose(fd);
        return -1;
    }
    isMqttConnSucc = 0;

    return fd;
}

int mqtt_tcp_destroy(int fd)
{
    int rc;

    /* Shutdown both send and receive operations. */
    rc = KING_SocketShutdown((int) fd, 2);
    if (0 != rc) 
    {
        KING_SysLog("shutdown error");
        return -1;
    }

    rc = KING_SocketClose((int) fd);
    if (0 != rc) 
    {
        KING_SysLog("closesocket error");
        return -1;
    }
    KING_UnRegNotifyFun(DEV_CLASS_SOCKET, fd, NULL);

    return 0;
}

int32 mqtt_tcp_write(int fd, const char *buf, uint32 len, uint32 timeout_ms)
{
    int ret;
    uint32 len_sent;
    uint64 t_end, t_left;
    fd_set sets;

    t_end = mqtt_utils_UptimeMs() + timeout_ms;
    len_sent = 0;
    ret = 1; /* send one time if timeout_ms is value 0 */
    int net_err = 0;

    do {
        t_left = mqtt_tcp_time_left(t_end, mqtt_utils_UptimeMs());

        if (0 != t_left) 
        {
            struct timeval timeout;

            FD_ZERO(&sets);
            FD_SET(fd, &sets);

            timeout.tv_sec = t_left / 1000;
            timeout.tv_usec = (t_left % 1000) * 1000;

            ret = KING_SocketSelect(fd + 1, NULL, &sets, NULL, &timeout);
            if (ret > 0) 
            {
                if (0 == FD_ISSET(fd, &sets)) 
                {
                    KING_SysLog("Should NOT arrive");
                    /* If timeout in next loop, it will not sent any data */
                    ret = 0;
                    continue;
                }
            } 
            else if (0 == ret) 
            {
                KING_SysLog("select-write timeout %d", (int)fd);
                break;
            } 
            else 
            {
                KING_SysLog("select-write fail, ret = select() = %d", ret);
                net_err = 1;
                break;
            }
        }

        if (ret > 0) 
        {
            ret = KING_SocketSend(fd, buf + len_sent, len - len_sent, 0);
            if (ret > 0) 
            {
                len_sent += ret;
            } 
            else if (0 == ret) 
            {
                KING_SysLog("No data be sent");
            } 
            else 
            {
                KING_SysLog("send fail, ret = send() = %d", ret);
                net_err = 1;
                break;
            }
        }
    } while (!net_err && (len_sent < len) && (mqtt_tcp_time_left(t_end, mqtt_utils_UptimeMs()) > 0));

    if (net_err) 
    {
        return -1;
    } 
    else 
    {
        return len_sent;
    }
}

int32 mqtt_tcp_read(int fd, char *buf, uint32 len, uint32 timeout_ms)
{
    int ret, err_code;
    uint32 len_recv;
    uint64 t_end, t_left;
    fd_set sets;
    struct timeval timeout;

    t_end = mqtt_utils_UptimeMs() + timeout_ms;
    len_recv = 0;
    err_code = 0;

    do {
        t_left = mqtt_tcp_time_left(t_end, mqtt_utils_UptimeMs());
        if (0 == t_left) 
        {
            break;
        }
        FD_ZERO(&sets);
        FD_SET(fd, &sets);

        timeout.tv_sec = t_left / 1000;
        timeout.tv_usec = (t_left % 1000) * 1000;

        ret = KING_SocketSelect(fd + 1, &sets, NULL, NULL, &timeout);
        if (ret > 0) 
        {
            ret = KING_SocketRecv(fd, buf + len_recv, len - len_recv, 0);
            if (ret > 0) 
            {
                len_recv += ret;
            } 
            else if (0 == ret) 
            {
                KING_SysLog("connection is closed");
                err_code = -1;
                break;
            } 
            else 
            {
                KING_SysLog("recv fail");
                err_code = -2;
                break;
            }
        } 
        else if (0 == ret) 
        {
            break;
        } 
        else 
        {
            KING_SysLog("select-recv fail");
            err_code = -2;
            break;
        }
    } while ((len_recv < len));

    /* priority to return data bytes if any data be received from TCP connection. */
    /* It will get error code on next calling */
    return (0 != len_recv) ? len_recv : err_code;
}
