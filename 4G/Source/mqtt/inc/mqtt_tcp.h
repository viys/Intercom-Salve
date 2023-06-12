/*******************************************************************************
 ** File Name:   mqtt_tcp.h
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
#ifndef __MQTT_TCP_H__
#define __MQTT_TCP_H__

#include "kingdef.h"
#include "kingsocket.h"

/*******************************************************************************
 ** Function 
 ******************************************************************************/
int mqtt_tcp_establish(SOCK_IN_ADDR_T *pHostAddr, uint16 port);
int32 mqtt_tcp_destroy(int fd);
int32 mqtt_tcp_write(int fd, const char *buf, uint32 len, uint32 timeout_ms);
int32 mqtt_tcp_read(int fd, char *buf, uint32 len, uint32 timeout_ms);

#endif
