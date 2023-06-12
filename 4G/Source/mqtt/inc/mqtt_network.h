/*******************************************************************************
 ** File Name:   mqtt_network.h
 ** Author:      Allance.Chen
 ** Date:        2020-06-04
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about MQTT network.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-06-04     Allance.Chen         Create.
 ******************************************************************************/
#ifndef __MQTT_NETWORK_H__
#define __MQTT_NETWORK_H__

/*******************************************************************************
 ** Header Files
 ******************************************************************************/

#include "mqtt_tcp.h"
#include "mqtt_tls.h"
#include "kingcstd.h"
#include "kingPlat.h"
#include "kingSocket.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define network_printf(fmt, args...)    do { KING_SysLog("mqtt: "fmt, ##args); } while(0)

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef struct mqtt_network MQTT_NETWORK_S;

struct mqtt_network {
    SOCK_IN_ADDR_T *pHostAddr;
    uint16 port;
    uint8 sslMode;
    uint16 ca_crt_len;

    /**< NULL, TCP connection; NOT NULL, SSL connection */
    const char *ca_crt;
    
    uint16 client_crt_len;
    
    const char *client_crt;
    
    uint16 client_key_len;
    
    const char *client_key;
    
    uint16 client_pwd_len;
    
   const char *client_pwd;
            
    /**< connection handle: 0, NOT connection; NOT 0, handle of the connection */
    int handle;

    /**< Read data from server function pointer. */
    int (*read)(MQTT_NETWORK_S*, char *, uint32, uint32);

    /**< Send data to server function pointer. */
    int (*write)(MQTT_NETWORK_S*, const char *, uint32, uint32);

    /**< Disconnect the network */
    int (*disconnect)(MQTT_NETWORK_S*);

    /**< Establish the network */
    int (*connect)(MQTT_NETWORK_S*);
};

int mqtt_network_init
(
    MQTT_NETWORK_S *mqtt_network, 
    SOCK_IN_ADDR_T *pHostAddr, 
    uint16 port, 
    uint8 sslMode,
    const char *ca_crt, 
    const char *client_crt, 
    const char *client_key, 
    const char *client_pwd
);

#endif
