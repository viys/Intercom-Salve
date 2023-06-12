/*******************************************************************************
 ** File Name:   at_ringbuf.c
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
/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "at_ringbuf.h"
#include "kingcstd.h"
#include "kingPlat.h"

/*******************************************************************************
 ** Functions
 ******************************************************************************/
void AtRingBuf_Init(AT_RING_BUF_S *pRingBuf, char* pBuf, uint32 size)
{
    int ret = 0;

    pRingBuf->bufPtr = pBuf;
    pRingBuf->size = size;
    pRingBuf->inPoint = 0;
    pRingBuf->outPoint = 0;
    pRingBuf->count = 0;
    ret = KING_MutexCreate("RingBuf_mutex", 0, &(pRingBuf->mutex));
    if (ret != SUCCESS )
    {
        KING_SysLog("KING_MutexCreate fail");
        return;
    }
}

void AtRingBuf_Reset(AT_RING_BUF_S *pRingBuf)
{
    pRingBuf->inPoint = 0;
    pRingBuf->outPoint = 0;
    pRingBuf->count = 0;
}

bool AtRingBuf_IsInited(AT_RING_BUF_S *pRingBuf)
{
    return ((pRingBuf->bufPtr != NULL) && (pRingBuf->size > 0));
}

void AtRingBuf_Uninit(AT_RING_BUF_S *pRingBuf)
{
    if(pRingBuf->bufPtr != NULL)
    {
        free(pRingBuf->bufPtr);
    }
    pRingBuf->bufPtr = NULL;
    pRingBuf->size = 0;
    pRingBuf->inPoint = 0;
    pRingBuf->outPoint = 0;
    pRingBuf->count = 0;
}

uint32 AtRingBuf_Space(AT_RING_BUF_S *pRingBuf)
{
    return (pRingBuf->size - pRingBuf->count);
}

uint32 AtRingBuf_In(AT_RING_BUF_S *pRingBuf, char *pData, uint32 len)
{
    uint32 actual_size;
    uint32 space = 0;
    uint32 i;

    space = AtRingBuf_Space(pRingBuf);
    actual_size = (space >= len) ? len : space;

    for(i = 0; i < actual_size; i++)
    {
        pRingBuf->bufPtr[pRingBuf->inPoint++] = pData[i];
        if(pRingBuf->inPoint >= pRingBuf->size)
        {
            pRingBuf->inPoint = 0;
        }
    }

    KING_MutexLock(pRingBuf->mutex, WAIT_OPT_INFINITE);
    pRingBuf->count += actual_size;  
    KING_MutexUnLock(pRingBuf->mutex);
    return actual_size;
}

uint32 AtRingBuf_Out(AT_RING_BUF_S *pRingBuf, char *pBuf, uint32 len)
{
    uint32 actual_size = 0;
    uint32 data_size = 0;
    uint32 i;

    data_size = pRingBuf->count;
    actual_size = (data_size > len) ? len : data_size;
    for(i = 0; i < actual_size; i++)
    {
        pBuf[i] = pRingBuf->bufPtr[pRingBuf->outPoint++];
        if(pRingBuf->outPoint >= pRingBuf->size)
        {
            pRingBuf->outPoint = 0;
        }
    }

    KING_MutexLock(pRingBuf->mutex, WAIT_OPT_INFINITE);
    pRingBuf->count -= actual_size;  
    KING_MutexUnLock(pRingBuf->mutex);
    return actual_size;
}

uint32 AtRingBuf_DataSize(AT_RING_BUF_S *pRingBuf)
{
    return pRingBuf->count;
}

