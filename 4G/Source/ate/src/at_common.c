/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#include "at_sa.h"
#include "at_module.h"
#include "at_utils.h"
#include "at_cfg.h"
#include "at_common.h"
#include "at_utility.h"
#include "at_cmd_engine.h"
#include "at_define.h"

// //////////////////////////////////////////////////////////////////////////////
// UTI: user transaction id table defines
// add by wangqunyang 2008.06.02
// //////////////////////////////////////////////////////////////////////////////
UINT16 nATUserTransIdSpareHeader = 0;
UINT16 nATUserTransIdSpareTail = 0;
AT_UTI_TABLE ATUserTransIdTable[AT_USER_TRANS_ID_SIZE];
struct ATWaitingEvent gAT_WaitingEvent[AT_WAITING_EVENT_MAX] = {
    {0, 0, 0, 0},
};

UINT8 g_rspStr[AT_RSP_STR_LEN] = { 0 };


PAT_CMD_RESULT AT_CreateRC(UINT32 uReturnValue,
                           UINT32 uResultCode,
                           UINT32 uErrorCode,
                           UINT8 nErrorType,
                           UINT32 nDelayTime,
                           UINT8 *pBuffer,
                           UINT16 nDataSize,
                           UINT8 nDLCI)
{
    // There are additional size in PAT_CMD_RESULT, but "+1" looks better
    PAT_CMD_RESULT pResult = (PAT_CMD_RESULT)AT_MALLOC(sizeof(AT_CMD_RESULT) + nDataSize + 1);
    if (pResult == NULL)
        return NULL;

    AT_MemZero(pResult, sizeof(AT_CMD_RESULT) + nDataSize + 1);
    pResult->uReturnValue = uReturnValue;

    pResult->uResultCode = uResultCode;
    pResult->uErrorCode = uErrorCode;
    pResult->nErrorType = nErrorType;
    pResult->nDataSize = nDataSize;
    pResult->nDelayTime = nDelayTime;
    pResult->engine = at_CmdGetByChannel(nDLCI);

    if (pBuffer != NULL)
    {
        AT_MemCpy(pResult->pData, pBuffer, nDataSize);

        // Make a protection on RSP format.
        AT_Util_TrimRspStringSuffixCrLf(pResult->pData, &pResult->nDataSize);
        pResult->pData[pResult->nDataSize] = '\0';
    }

    return pResult;
}

/******************************************************************************************
Function            :   SMS_UnicodeBigend2Unicode
Description     :       unicode BigEnd to unicode normal(LittlteEnd)
Called By           :
Data Accessed       :
Data Updated        :
Input           :       UINT8 * pUniBigData, UINT8 *pUniData, UINT16 nDataLen
Output          :
Return          :   BOOL
Others          :       build by wangqunyang 30/04/2008
*******************************************************************************************/
BOOL AT_UnicodeBigEnd2Unicode(UINT8 *pUniBigData, UINT8 *pUniData, UINT16 nDataLen)
{

    /* check the pointer is NULL or not */
    if ((NULL == pUniData) || (NULL == pUniBigData))
    {
        return FALSE;
    }

    /* there is odd chars and we make
       ** it even, discard the last char */
    if (nDataLen % 2)
    {
        nDataLen = nDataLen - 1;
    }

    while (nDataLen > 0)
    {
        pUniData[nDataLen - 2] = pUniBigData[nDataLen - 1];
        pUniData[nDataLen - 1] = pUniBigData[nDataLen - 2];

        nDataLen -= 2;
    }

    return TRUE;
}

// caoxh [+]2008-05-9
char *strupr(char *pStr)
{
    char *p = pStr;

    if (p == NULL)
        return NULL;

    while (*p != '\0')
    {
        if (*p >= 'a' && *p <= 'z')
            *p = *p - 32;

        p++;
    }

    return pStr;
}

// caoxh [+]2008-05-9

/******************************************************************************************
Function            :   AT_Bytes2String
Description     :       transfer bytes to ascii string
Called By           :   ATS moudle
Data Accessed       :
Data Updated    :
Input           :   UINT8 * pSource, UINT8 * pDest, UINT8 nSourceLen
Output          :
Return          :   INT8
Others          :   build by wangqunyang 2008.05.22
*******************************************************************************************/
BOOL AT_Bytes2String(UINT8 *pDest, UINT8 *pSource, UINT8 *nSourceLen)
{

    UINT8 nTemp = 0;
    UINT8 nDestLen = 0;

    if ((NULL == pSource) || (NULL == pDest))
    {
        KING_SysLog("AT_Bytes2String: pointer is NULL");
        return FALSE;
    }

    KING_SysLog("AT_Bytes2String: nSourceLen = %u", *nSourceLen);

    while (nTemp < *nSourceLen)
    {
        /* get high byte */
        pDest[nDestLen] = (pSource[nTemp] >> 4) & 0x0f;

        if (pDest[nDestLen] < 10)
        {
            pDest[nDestLen] |= '0';
        }
        else
        {
            pDest[nDestLen] += 'A' - 10;
        }

        nDestLen++;

        /* get low byte */
        pDest[nDestLen] = pSource[nTemp] & 0x0f;

        if (pDest[nDestLen] < 10)
        {
            pDest[nDestLen] |= '0';
        }
        else
        {
            pDest[nDestLen] += 'A' - 10;
        }

        nDestLen++;

        nTemp++;
    }

    pDest[nDestLen] = '\0';

    *nSourceLen = nDestLen;

    /* string char counter must be even */

    if (*nSourceLen % 2)
    {
        KING_SysLog("AT_Bytes2String: source len is not even, nSourceLen = %u", nSourceLen);
        return FALSE;
    }

    return TRUE;
}

/******************************************************************************************
Function            :   AT_String2Bytes
Description     :       This functions can transfer ascii string to bytes
Called By           :   ATS moudle
Data Accessed       :
Data Updated        :
Input           :       UINT8 * pDest, UINT8 * pSource, UINT8* pLen
Output          :
Return          :   INT8
Others          :   build by wangqunyang 2008.05.22
*******************************************************************************************/
BOOL AT_String2Bytes(UINT8 *pDest, UINT8 *pSource, UINT8 *pLen)
{
    UINT8 nSourceLen = *pLen;
    UINT8 nTemp = 0;
    UINT8 nByteNumber = 0;

    if ((NULL == pSource) || (NULL == pDest))
    {
        KING_SysLog("AT_String2Bytes: pointer is NULL");
        return FALSE;
    }

    KING_SysLog("AT_String2Bytes: nSourceLen = %u", nSourceLen);

    /* string char counter must be even */
    if (nSourceLen % 2)
    {
        KING_SysLog("AT_String2Bytes: source len is not even");
        return FALSE;
    }

    while (nTemp < nSourceLen)
    {
        /* get high half byte */
        if ((pSource[nTemp] > 0x2f) && (pSource[nTemp] < 0x3a))
        {
            pDest[nByteNumber] = (pSource[nTemp] - '0') << 4;
        }
        else if ((pSource[nTemp] > 0x40) && (pSource[nTemp] < 0x47))
        {
            pDest[nByteNumber] = (pSource[nTemp] - 'A' + 10) << 4;
        }
        else if ((pSource[nTemp] > 0x60) && (pSource[nTemp] < 0x67))
        {
            pDest[nByteNumber] = (pSource[nTemp] - 'a' + 10) << 4;
        }
        else
        {
            KING_SysLog("high half byte more than 'F' of HEX: pSource[nTemp]: = %u ", pSource[nTemp]);
            return FALSE;
        }

        /* get low half byte */
        nTemp++;

        if ((pSource[nTemp] > 0x2f) && (pSource[nTemp] < 0x3a))
        {
            pDest[nByteNumber] += (pSource[nTemp] - '0');
        }
        else if ((pSource[nTemp] > 0x40) && (pSource[nTemp] < 0x47))
        {
            pDest[nByteNumber] += (pSource[nTemp] - 'A' + 10);
        }
        else if ((pSource[nTemp] > 0x60) && (pSource[nTemp] < 0x67))
        {
            pDest[nByteNumber] += (pSource[nTemp] - 'a' + 10);
        }
        else
        {
            KING_SysLog("low half byte more than 'F' of HEX: pSource[nTemp]: = %u ", pSource[nTemp]);
            return FALSE;
        }

        nTemp++;

        nByteNumber++;
    }

    pDest[nByteNumber] = '\0';

    *pLen = nByteNumber;

    return TRUE;
}

#ifdef MMI_ONLY_AT
UINT8 isEarphonePlugged_1 = 0;
UINT8 GetHandsetInPhone(VOID)
{
    return isEarphonePlugged_1;
}
VOID SetHandsetInPhone(UINT8 h_status)
{
    isEarphonePlugged_1 = h_status;
}
#endif

UINT32 AT_GetFreeUTI(UINT32 nServiceId, UINT8 *pUTI)
{
    return SUCCESS; // CFW_GetFreeUTI(nServiceId, pUTI);
}

/******************************************************************************************
Function            :   AT_InitUtiTable
Description     :       Initial the UTI table when AT Module beiginning
Called By           :   AT moudle
Data Accessed       :
Data Updated        :
Input           :       VOID
Output          :
Return          :   VOID
Others          :   build by wangqunyang 2008.06.02
*******************************************************************************************/
VOID AT_InitUtiTable()
{
    UINT16 nIndex = 0;

    AT_MemZero(&ATUserTransIdTable[0], AT_USER_TRANS_ID_SIZE * sizeof(AT_UTI_TABLE));

    nATUserTransIdSpareHeader = 1;
    nATUserTransIdSpareTail = 0;

    for (nIndex = 1; nIndex < AT_USER_TRANS_ID_SIZE - 1; nIndex++)
    {
        ATUserTransIdTable[nIndex].nNextUTISpareTable = nIndex + 1;
    }

    nATUserTransIdSpareTail = nIndex;
    ATUserTransIdTable[nATUserTransIdSpareTail].nNextUTISpareTable = 0;
}

/******************************************************************************************
Function            :   AT_AllocUserTransID
Description     :       malloc one user transaction id
Called By           :   AT moudle
Data Accessed       :
Data Updated        :
Input           :       VOID
Output          :
Return          :   UINT8
Others          :   build by wangqunyang 2008.06.02
*******************************************************************************************/
UINT16 AT_AllocUserTransID()
{
    UINT16 TempIndex = 0;

    // verify the parameters
    if ((nATUserTransIdSpareHeader <= 0) || (nATUserTransIdSpareTail > AT_USER_TRANS_ID_SIZE - 1) || (nATUserTransIdSpareHeader == nATUserTransIdSpareTail)) // leave at least one node
    {
        KING_SysLog("AT alloc UTI exception");
        return TempIndex;
    }

    // alloc the array element
    TempIndex = nATUserTransIdSpareHeader;
    nATUserTransIdSpareHeader = ATUserTransIdTable[nATUserTransIdSpareHeader].nNextUTISpareTable;
    AT_MemZero(&ATUserTransIdTable[TempIndex], sizeof(AT_UTI_TABLE));

    return TempIndex;
}

/******************************************************************************************
Function            :   AT_FreeUserTransID
Description     :       free one user transaction id
Called By           :   AT moudle
Data Accessed       :
Data Updated        :
Input           :       UINT8 nUTI
Output          :
Return          :   VOID
Others          :   build by wangqunyang 2008.06.02
*******************************************************************************************/
VOID AT_FreeUserTransID(UINT16 nUTI)
{
    // verify the parameters
    if ((nUTI >= AT_USER_TRANS_ID_SIZE) || (ATUserTransIdTable[nUTI].nNextUTISpareTable != 0))
    {
        KING_SysLog("AT free UTI exception");
        return;
    }

    // Free the array element
    ATUserTransIdTable[nATUserTransIdSpareTail].nNextUTISpareTable = nUTI;
    nATUserTransIdSpareTail = nUTI;
    AT_MemZero(&(ATUserTransIdTable[nUTI]), sizeof(AT_UTI_TABLE));
}

UINT32 AT_GetWaitingEventOnDLCI(UINT8 sim, UINT8 dlci)
{
    for (int i = 0; i < AT_WAITING_EVENT_MAX; i++)
    {
        if ((gAT_WaitingEvent[i].enable) &&
            (gAT_WaitingEvent[i].nDLCI == dlci) &&
            (gAT_WaitingEvent[i].nSim == sim))
        {
            ATLOGI("AT get waiting #%d for dlci/%d sim/%d: event/%d ", i, dlci, sim,
                   gAT_WaitingEvent[i].nEvent);
            return gAT_WaitingEvent[i].nEvent;
        }
    }
    ATLOGI("AT fail get waiting for dlci/%d sim/%d ", dlci, sim);
    return 0;
}

VOID AT_AddWaitingEvent(UINT32 event, UINT8 sim, UINT8 dlci)
{
    for (int i = 0; i < AT_WAITING_EVENT_MAX; i++)
    {
        if (gAT_WaitingEvent[i].enable == 0)
        {
            gAT_WaitingEvent[i].nEvent = event;
            gAT_WaitingEvent[i].nSim = sim;
            gAT_WaitingEvent[i].nDLCI = dlci;
            gAT_WaitingEvent[i].enable = 1;
            ATLOGI("AT add waiting to #%d, event/%d sim/%d dlci/%d", i, event, sim, dlci);
            return;
        }
    }
    ATLOGI("AT fail add waiting, event/%d sim/%d dlci/%d", event, sim, dlci);
    return;
}

BOOL AT_isWaitingEvent(UINT32 event, UINT8 sim, UINT8 dlci)
{
    for (int i = 0; i < AT_WAITING_EVENT_MAX; i++)
    {
        if ((gAT_WaitingEvent[i].enable) &&
            (gAT_WaitingEvent[i].nEvent == event) &&
            (gAT_WaitingEvent[i].nSim == sim))
        {
            ATLOGI("AT is waiting #%d, event/%d sim/%d dlci/%d", i, event, sim, dlci);
            return TRUE;
        }
    }
    ATLOGI("AT not waiting, event/%d sim/%d dlci/%d", event, sim, dlci);
    return FALSE;
}

VOID AT_DelWaitingEventOnDLCI(UINT8 dlci)
{
    for (int i = 0; i < AT_WAITING_EVENT_MAX; i++)
    {
        if ((gAT_WaitingEvent[i].enable) && (gAT_WaitingEvent[i].nDLCI == dlci))
        {
            ATLOGI("AT delete waiting #%d, dlci/%d: event/%d sim/%d", i, dlci,
                   gAT_WaitingEvent[i].nEvent, gAT_WaitingEvent[i].nSim);
            gAT_WaitingEvent[i].nEvent = 0;
            gAT_WaitingEvent[i].nSim = 0;
            gAT_WaitingEvent[i].nDLCI = 0;
            gAT_WaitingEvent[i].enable = 0;
            return;
        }
    }
    ATLOGI("AT fail delete waiting, dlci/%d", dlci);
    return;
}

VOID AT_DelWaitingEvent(UINT32 event, UINT8 sim, UINT8 dlci)
{
    for (int i = 0; i < AT_WAITING_EVENT_MAX; i++)
    {
        if ((gAT_WaitingEvent[i].enable) &&
            (gAT_WaitingEvent[i].nEvent == event) &&
            (gAT_WaitingEvent[i].nSim == sim))
        {
            ATLOGI("AT delete waiting #%d, event/%d sim/%d dlci/%d", i, event, sim, dlci);
            gAT_WaitingEvent[i].nEvent = 0;
            gAT_WaitingEvent[i].nSim = 0;
            gAT_WaitingEvent[i].nDLCI = 0;
            gAT_WaitingEvent[i].enable = 0;
            return;
        }
    }
    ATLOGI("AT fail delete waiting, event/%d sim/%d dlci/%d", event, sim, dlci);
    return;
}

UINT8 AT_GetDLCIFromWaitingEvent(UINT32 event, UINT8 sim)
{
    for (int i = 0; i < AT_WAITING_EVENT_MAX; i++)
    {
        if ((gAT_WaitingEvent[i].enable) &&
            (gAT_WaitingEvent[i].nEvent == event) &&
            (gAT_WaitingEvent[i].nSim == sim))
        {
            ATLOGI("AT get waiting #%d, event/%d sim/%d: dlci/%d", i, event, sim,
                   gAT_WaitingEvent[i].nDLCI);
            return gAT_WaitingEvent[i].nDLCI;
        }
    }
    ATLOGI("AT fail get waiting, event/%d sim/%d", event, sim);
    return 0;
}
