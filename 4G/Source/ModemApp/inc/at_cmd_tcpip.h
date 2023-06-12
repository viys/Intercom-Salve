/*******************************************************************************
 ** File Name:   at_cmd_tcpip.h
 ** Author:      Allance.Chen
 ** Date:        2020-08-04
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about tcpip.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-08-04     Allance.Chen         Create.
 ******************************************************************************/
#ifndef __AT_CMD_TCPIP_H__
#define __AT_CMD_TCPIP_H__

/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingdef.h"
#include "At_common.h"


/*---------------------------------------------------------------------------
  ** Defines
 ---------------------------------------------------------------------------*/
#define PDP_ACTIVE_SOCK_QIDNSGIP_ACTIVE     0xF3
#define PDP_ACTIVE_SOCK_HTTP_ACTIVE         0xF4

#define PDP_ACTIVE_SOCK_JUST_ACTIVE         0xFF

/*******************************************************************************
  ** Function Delcarations
 ******************************************************************************/
void tcpip_init(void);

int tcpip_IsPdpActiveAllowed(void);

int tcpip_IsPdpActived(uint8 ctxID);

int tcpip_activeByCtxID(AT_CMD_ENGINE_T *atEngine, uint8 ctxID, uint16 actType);

int32 tcpip_getSwHandleByCID(uint8 ctxID);

bool tcpip_ttConnected(void);

void tcpip_ttEscapeDataMode(uint8 nDLCI);

void tcpip_ttResumeDataMode(uint8 nDLCI);

#endif/*__AT_CMD_TCPIP_H__*/

