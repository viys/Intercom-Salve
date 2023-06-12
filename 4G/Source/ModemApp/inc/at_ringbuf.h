/*******************************************************************************
 ** File Name:   at_ringbuf.h
 ** Author:      Allance.Chen
 ** Date:        2020-05-27
 ** Copyright:   Copyright 2020-2020 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about ring buffer.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-05-27     Allance.Chen         Create.
 ******************************************************************************/
#ifndef __AT_RINGBUF_H__
#define __AT_RINGBUF_H__

/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "kingDef.h"
#include "kingRtos.h"


/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef struct atRingBuf{
    uint32 size;              // Total Size of the buffer.
    uint32 inPoint;    // First data in the buffer.
    uint32 outPoint;   // Last data in the buffer.
    char *bufPtr;            // buffer address.
    uint32 count;              // Data Size in the buffer.  
    MUTEX_HANDLE  mutex;
} AT_RING_BUF_S;

/*******************************************************************************
  ** Function Delcarations
 ******************************************************************************/
void AtRingBuf_Init(AT_RING_BUF_S *pRingBuf, char* pBuf, uint32 size);

void AtRingBuf_Reset(AT_RING_BUF_S *pRingBuf);

bool AtRingBuf_IsInited(AT_RING_BUF_S *pRingBuf);

void AtRingBuf_Uninit(AT_RING_BUF_S *pRingBuf);

uint32 AtRingBuf_Space(AT_RING_BUF_S *pRingBuf);

uint32 AtRingBuf_In(AT_RING_BUF_S *pRingBuf, char *pData, uint32 len);

uint32 AtRingBuf_Out(AT_RING_BUF_S *pRingBuf, char *pBuf, uint32 len);

uint32 AtRingBuf_DataSize(AT_RING_BUF_S *pRingBuf);

#endif/*__AT_RINGBUF_H__*/

