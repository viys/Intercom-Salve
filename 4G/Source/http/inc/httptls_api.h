/*******************************************************************************
 ** File Name:   httptls_api.h
 ** Author:      Allance.Chen
 ** Date:        2020-06-30
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains functions about http tls api.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-06-30     Allance.Chen         Create.
 ******************************************************************************/
#ifndef _HTTPTLS_API_H_
#define _HTTPTLS_API_H_

/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingdef.h"

/*******************************************************************************
**  Type definition
********************************************************************************/
typedef enum httpTlsState{
    HTTP_TLS_STATE_NONE,
    HTTP_TLS_STATE_CREATED,
    HTTP_TLS_STATE_CONNECTING,
    HTTP_TLS_STATE_CONNECTED,
    HTTP_TLS_STATE_CLOSED,

    HTTP_TLS_STATE_NUM
} HTTP_TLS_STATE_E;

typedef struct
{
    uint32 len;
    unsigned char *crt;
} HTTPTLS_CRT_T;

/*******************************************************************************
 ** Function 
 ******************************************************************************/
int httpTls_client_init(void);
int32 httpTls_setVersion(uint16 sslVersion);
int32 httpTls_setChpherSuites(uint16 cipherSuites);
int32 httpTls_setCert(HTTPTLS_CRT_T* pCrt);
int32 httpTls_setClientCert(HTTPTLS_CRT_T* pCrt);
int32 httpTls_setClientKey(HTTPTLS_CRT_T* pCrt);
int32 httpTls_setAuthmode(uint16 authmode);
int32 httpTls_setIgnoreTime(int ignoretime);
int32 httpTls_connect(int socketFd);
int32 httpTls_read(int socketFd, char* pData, uint32 len, uint32 timeout_ms);
int32 httpTls_write(int socketFd, char* pData, uint32 len);
int32 httpTls_close(void);
uint8 httTls_getState(void);

#endif /* _HTTPTLS_API_H_ */

