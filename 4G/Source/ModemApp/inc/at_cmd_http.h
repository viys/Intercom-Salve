/*******************************************************************************
 ** File Name:   at_cmd_http.h
 ** Author:      Allance.Chen
 ** Date:        2020-08-10
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about http.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-08-10     Allance.Chen         Create.
 ******************************************************************************/
#ifndef __AT_CMD_HTTP_H__
#define __AT_CMD_HTTP_H__

/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingdef.h"

/*******************************************************************************
  ** Function Delcarations
 ******************************************************************************/
void AT_http_init(void);

void http_activePdpCnf(bool isActive);

bool http_isEnterDataMode(void);

void http_networkDownExit(uint8 ctxID);

#endif/*__AT_CMD_TCPIP_H__*/

