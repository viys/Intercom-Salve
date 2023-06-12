/*******************************************************************************
 ** File Name:   httpClient.h
 ** Author:      Allance.Chen
 ** Date:        2020-06-24
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains functions about http.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-06-24     Allance.Chen         Create.
 ******************************************************************************/
#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingdef.h"

/*---------------------------------------------------------------------------
 ** Defines
 ---------------------------------------------------------------------------*/
//http errcode
#define HTTP_UNKNOWN_ERR            701
#define HTTP_TIMEOUT                702
#define HTTP_BUSY                   703
#define HTTP_UART_BUSY              704
#define HTTP_NO_GET_POST_REQ        705
#define HTTP_NW_BUSY                706
#define HTTP_NW_OPEN_FAIL           707
#define HTTP_NW_NO_CONFIG           708
#define HTTP_NW_DEACTIVATED         709
#define HTTP_NW_ERR                 710
#define HTTP_URL_ERR                711
#define HTTP_EMPTY_URL              712
#define HTTP_IP_ADDR_ERR            713
#define HTTP_DNS_ERR                714
#define HTTP_SOCK_CREATE_ERR        715
#define HTTP_SOCK_CONNECT_ERR       716
#define HTTP_SOCK_RD_ERR            717
#define HTTP_SOCK_WR_ERR            718
#define HTTP_SOCK_CLOSE             719
#define HTTP_DATA_ENCODE_ERR        720
#define HTTP_DATA_DECODE_ERR        721
#define HTTP_RD_TIMEOUT             722
#define HTTP_RSP_FAIL               723
#define HTTP_INCOMING_CALL_BUSY     724
#define HTTP_VOICE_CALL_BUSY        725
#define HTTP_INPUT_TIMEOUT          726
#define HTTP_WAIT_DATA_TIMEOUT      727
#define HTTP_WAIT_HTTP_RSP_TIMEOUT  728
#define HTTP_ALLOC_MEM_FAIL         729
#define HTTP_INVALID_PARAM          730
#define HTTP_FAIL_TO_GET_LOCATION   731

//http response code
#define HTTP_RESPONSE_CODE_UNKNOW                           0

#define HTTP_RESPONSE_CODE_CONTINUE                         100
#define HTTP_RESPONSE_CODE_SWITCHING_PROTOCOLS              101

#define HTTP_RESPONSE_CODE_OK                               200
#define HTTP_RESPONSE_CODE_CREATED                          201
#define HTTP_RESPONSE_CODE_ACCEPTED                         202
#define HTTP_RESPONSE_CODE_NON_AUTHORITATIVE                203
#define HTTP_RESPONSE_CODE_NO_CONTENT                       204
#define HTTP_RESPONSE_CODE_RESET_CONTENT                    205
#define HTTP_RESPONSE_CODE_PARTIAL_CONTENT                  206

#define HTTP_RESPONSE_CODE_MULTIPLE_CHOICES                 300
#define HTTP_RESPONSE_CODE_MOVED_PERMANENTLY                301
#define HTTP_RESPONSE_CODE_FOUND                            302
#define HTTP_RESPONSE_CODE_SEE_OTHER                        303
#define HTTP_RESPONSE_CODE_NOT_MODIFIED                     304
#define HTTP_RESPONSE_CODE_USE_PROXY                        305
#define HTTP_RESPONSE_CODE_TEMPORARY_REDIRECT               307
#define HTTP_RESPONSE_CODE_PERMANENTLY_REDIRECT             308

#define HTTP_RESPONSE_CODE_BAD_REQUEST                      400
#define HTTP_RESPONSE_CODE_UNAUTHORIZED                     401
#define HTTP_RESPONSE_CODE_PAYMENT_REQUIRED                 402
#define HTTP_RESPONSE_CODE_FORBIDDEN                        403
#define HTTP_RESPONSE_CODE_NOT_FOUND                        404
#define HTTP_RESPONSE_CODE_METHOD_NOT_ALLOWED               405
#define HTTP_RESPONSE_CODE_NOT_ACCEPTABLE                   406
#define HTTP_RESPONSE_CODE_PROXY_AUTHENTICATION_REQUIRED    407
#define HTTP_RESPONSE_CODE_REQUEST_TIMEOUT                  408
#define HTTP_RESPONSE_CODE_CONFLICT                         409
#define HTTP_RESPONSE_CODE_GONE                             410
#define HTTP_RESPONSE_CODE_LENGTH_REQUIRED                  411
#define HTTP_RESPONSE_CODE_PRECONDITION_FAILED              412
#define HTTP_RESPONSE_CODE_REQUEST_ENTITY_TOO_LARGE         413
#define HTTP_RESPONSE_CODE_REQUEST_URI_TOO_LONG             414
#define HTTP_RESPONSE_CODE_UNSUPPORTED_MEDIA_TYPE           415
#define HTTP_RESPONSE_CODE_REQUESTED_RANGE_NOT_SATISFIABLE  416
#define HTTP_RESPONSE_CODE_EXPECTATION_FAILED               417

#define HTTP_RESPONSE_CODE_INTERNAL_SERVER_ERROR            500
#define HTTP_RESPONSE_CODE_NOT_IMPLEMENTED                  501
#define HTTP_RESPONSE_CODE_BAD_GATEWAY                      502
#define HTTP_RESPONSE_CODE_SERVICE_UNAVAILABLE              503
#define HTTP_RESPONSE_CODE_GATEWAY_TIMEOUT                  504
#define HTTP_RESPONSE_CODE_VERSION_NOT_SUPPORTED            505

#define HTTP_RESPONSE_CODE_NOT_HTTP_PDU                     600
#define HTTP_RESPONSE_CODE_NETWORK_ERROR                    601
#define HTTP_RESPONSE_CODE_NO_MEMORY                        602
#define HTTP_RESPONSE_CODE_DNS_ERROR                        603
#define HTTP_RESPONSE_CODE_STACK_BUSY                       604

#define HTTP_RESPONSE_CODE_HEADER_CONTINUE                  701


/*boundary string for content-type = multipart/form-data */
#define HTTP_BOUNDARY_STR       "--------boundaryxmcheerzing"


/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef enum httpStateType {
    HTTP_STATE_NONE = 0,
    HTTP_STATE_URL_GETTING,
    HTTP_STATE_URL_READY,
    HTTP_STATE_IP_GETTING,
    HTTP_STATE_IP_GOT,
    HTTP_STATE_CONNECTING,
    HTTP_STATE_CONNECTED,
    HTTP_STATE_SEND,
    HTTP_STATE_RECEIVE_START,
    HTTP_STATE_READ,

    HTTP_STATE_NUM
} HTTP_STATE_TYPE_E;

typedef enum httpContentType {
    HTTP_CONTENT_APPLICATION_URLENCODED = 0,
    HTTP_CONTENT_TEXT_PLAIN,
    HTTP_CONTENT_APPLICATION_OCTET_STREAM,
    HTTP_CONTENT_MULIPART_FORM_DATA,
    HTTP_CONTENT_APPLICATION_JSON,
    HTTP_CONTENT_AUDIO_MP3,

    HTTP_CONTENT_MAX_NUM
} HTTP_CONTENT_TYPE_E;

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef void (*HTTP_RSP_CALLBACK)(uint16 errCode, int16 httprspcode, uint32 contentLen);

/*******************************************************************************
  ** Function Delcarations
 ******************************************************************************/
void http_init(void);

void http_reqHeaderSet(uint8 reqHeader);
uint8 http_reqHeaderGet(void);

int http_addHeaderKv(char *key, char *value);

void http_rspHeaderSet(uint8 rspHeader);
uint8 http_rspHeaderGet(void);

void http_contentTypeSet(HTTP_CONTENT_TYPE_E contentType);

void http_stateTypeSet(HTTP_STATE_TYPE_E  stateType);
HTTP_STATE_TYPE_E http_stateTypeGet(void);

int http_urlParse(char *pUrl);

void http_get(int32 hContext, char *pHeaderStr, 
    uint32 headLen, uint16 rspTimeout, HTTP_RSP_CALLBACK rspCb);

void http_post(int32 hContext, char *pData, 
    uint32 dataLen, uint16 rspTimeout, HTTP_RSP_CALLBACK rspCb);

int http_canReadData(void);

void http_readHeaderData(char **ppHeaderStr, uint32 *headerLen);

int http_readContentData(uint32 waitTimeout, uint32 dataLen, char *pData, uint8 *readEnd);

int http_readContentDataAndLen(
    uint32 waitTimeout,
    uint32 dataLen, char *pData,
    uint32 *actualDataLen, uint8 *readEnd
);

void http_clearStatus(void);

#endif /*__HTTP_CLIENT_H__*/
