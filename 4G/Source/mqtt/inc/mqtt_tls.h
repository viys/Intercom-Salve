/*******************************************************************************
 ** File Name:   mqtt_tls.h
 ** Author:      Allance.Chen
 ** Date:        2020-06-04
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about mqtt tls.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-06-04     Allance.Chen         Create.
 ******************************************************************************/
#ifndef __MQTT_TLS_H__
#define __MQTT_TLS_H__

/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "kingsocket.h"

/*******************************************************************************
 ** Function 
 ******************************************************************************/
int mqtt_ssl_establish(SOCK_IN_ADDR_T *pHostAddr,
                            uint16 port,
                            const char *ca_crt,
                            size_t ca_crt_len,
                            const char *client_crt,
                            size_t client_crt_len,
                            const char *client_key,
                            size_t client_key_len,
                            const char *client_pwd,
                            size_t client_pwd_len
                            );
int32 mqtt_ssl_destroy(int handle);
int mqtt_ssl_write(int handle, const char *buf, int len, int timeout_ms);
int mqtt_ssl_read(int handle, char *buf, int len, int timeout_ms);
#endif
