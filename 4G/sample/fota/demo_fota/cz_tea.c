
/******************************************************************************
** File Name:     NA2201_tea.c                                                                                                                *
** Author:                                                                                                                                           *
** Date:             2015-09-08                                                                                                             *
** Copyright:     Copyright 2015-2025 by cheerzing Communications,Inc.                                         *
**                      All Rights Reserved.                                                                                                   *
** Description:                                                                                                                                    *
*******************************************************************************

*******************************************************************************
**                      Edit History                                                                                                                *
*******************************************************************************                                                             
** DATE           NAME             Description:                                                                                           *
*******************************************************************************
**                                                                                                                                                        *
******************************************************************************/

/******************************************************************************
**                         Dependencies                                                                                                        *
******************************************************************************/
#if 1//defined(CZ_FOTA_SUPPORT)

//#include "cs_types.h"
//#include "cfw.h"
#include "KingCSTD.h"
#include "cz_tea.h"
//#include "stdlib.h"
//#include "string.h"

/**---------------------------------------------------------------------------*
**                         Compiler Flag                                                                                                         *
**---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern   "C"
{
#endif
/*
static uint16 Host2NetShort(uint16 usHost) 
{ 
    const uint16 us = 0x1234 ; 
    return ((uint8 *)&us)[0] == 0x12 ? usHost : ((usHost>>8) | (usHost<<8)) ; 
} 
  
static uint16 Net2HostShort(uint16 usNet) 
{ 
    return Host2NetShort(usNet) ; 
} 
*/
static uint32 Host2NetLong(uint32 ulHost) 
{ 
    const uint16 us = 0x1234 ; 
    return ((uint8 *)&us)[0] == 0x12 ? ulHost : (((ulHost>>8) & 0xFF00) |  
        ((ulHost<<8) & 0xFF0000) | (ulHost<<24) | (ulHost>>24)) ; 
} 
  
static uint32 Net2HostLong(uint32 ulHost) 
{ 
    return Host2NetLong(ulHost) ; 
} 
  
static void EnCipher(const uint32 *const v, const uint32 *const k, uint32 *const w) 
{ 
    register uint32  
        y     = Host2NetLong(v[0]), 
        z     = Host2NetLong(v[1]), 
        a     = Host2NetLong(k[0]), 
        b     = Host2NetLong(k[1]), 
        c     = Host2NetLong(k[2]), 
        d     = Host2NetLong(k[3]), 
        n     = 0x10,       /* do encrypt 16 (0x10) times */ 
        sum   = 0, 
        delta = 0x9E3779B9; /*  0x9E3779B9 - 0x100000000 = -0x61C88647 */ 
  
    while (n-- > 0) 
    { 
        sum += delta; 
        y += ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b); 
        z += ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d); 
    } 
  
    w[0] = Net2HostLong(y); 
    w[1] = Net2HostLong(z); 
} 
  
static void DeCipher(const uint32 *const v, const uint32 *const k, uint32 *const w) 
{ 
    register uint32 
        y     = Host2NetLong(v[0]), 
        z     = Host2NetLong(v[1]), 
        a     = Host2NetLong(k[0]), 
        b     = Host2NetLong(k[1]), 
        c     = Host2NetLong(k[2]), 
        d     = Host2NetLong(k[3]), 
        n     = 0x10, 
        sum   = 0xE3779B90,  
        /* why this ? must be related with n value*/ 
        delta = 0x9E3779B9; 
  
    /* sum = delta<<5, in general sum = delta * n */ 
    while (n-- > 0) 
    { 
        z -= ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d); 
        y -= ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b); 
        sum -= delta; 
    } 
  
    w[0] = Net2HostLong(y); 
    w[1] = Net2HostLong(z); 
} 
  
static uint32 Random(void) 
{ 
    return (uint32)rand(); 
    //return 0xdead ; 
} 
  
static void EncryptEach8Bytes(CZ_TEACTX *pCtx) 
{ 
#ifdef CRYPT_ONE_BYTE 
    uint32 i ; 
    uint8 *pPlain8, *pPlainPre8, *pCrypt8, *pCryptPre8 ; 
    pPlain8 = (uint8 *)pCtx->buf ; 
    pPlainPre8 = (uint8 *)pCtx->bufPre ; 
    pCrypt8 = (uint8 *)pCtx->pCrypt ; 
    pCryptPre8 = (uint8 *)pCtx->pCryptPre ; 
    for(i=0; i<8; i++) 
        pPlain8[i] ^= pCryptPre8[i] ; 
    EnCipher((uint32 *)pPlain8, (uint32 *)pCtx->pKey, (uint32 *)pCrypt8) ; 
    for(i=0; i<8; i++) 
        pCrypt8[i] ^= pPlainPre8[i] ; 
    // 
    for(i=0; i<8; i++) 
        pPlainPre8[i] = pPlain8[i] ; 
#else 
    uint32 *pPlain8, *pPlainPre8, *pCrypt8, *pCryptPre8 ; 
    pPlain8 = (uint32 *)pCtx->buf ; 
    pPlainPre8 = (uint32 *)pCtx->bufPre ; 
    pCrypt8 = (uint32 *)pCtx->pCrypt ; 
    pCryptPre8 = (uint32 *)pCtx->pCryptPre ; 
    pPlain8[0] ^= pCryptPre8[0] ; 
    pPlain8[1] ^= pCryptPre8[1] ; 
    EnCipher(pPlain8, (const uint32 *)pCtx->pKey, pCrypt8) ; 
    pCrypt8[0] ^= pPlainPre8[0] ; 
    pCrypt8[1] ^= pPlainPre8[1] ; 
    pPlainPre8[0] = pPlain8[0] ; 
    pPlainPre8[1] = pPlain8[1] ; 
#endif 
    pCtx->pCryptPre = pCtx->pCrypt ; 
    pCtx->pCrypt += 8 ; 
} 
  
uint32 CZ_Tea_Encrypt(CZ_TEACTX *pCtx, const uint8 *pPlain, uint32 ulPlainLen,  
    const uint8 *pKey, uint8 *pOut, uint32 *pOutLen) 
{ 
    uint32 ulPos, ulPadding, ulOut ; 
    const uint8 *p ; 
    if(pPlain == NULL || ulPlainLen == 0 || pOutLen == NULL) 
        return 0 ; 
    ulPos = (8 - ((ulPlainLen + 10) & 0x07)) & 0x07 ; 
    ulOut = 1 + ulPos + 2 + ulPlainLen + 7 ; 
    if(*pOutLen < ulOut) 
    { 
        *pOutLen = ulOut ; 
        return 0 ; 
    } 
    *pOutLen = ulOut ; 
    memset(pCtx, 0, sizeof(CZ_TEACTX)) ; 
    pCtx->pCrypt = pOut ; 
    pCtx->pCryptPre = pCtx->bufPre ; 
    pCtx->pKey = pKey ; 
    pCtx->buf[0] = (uint8)((Random() & 0xF8) | ulPos) ; 
    memset(pCtx->buf+1, (uint8)Random(), ulPos++) ; 
    for(ulPadding=0; ulPadding<2; ulPadding++) 
    { 
        if(ulPos == 8) 
        { 
            EncryptEach8Bytes(pCtx) ; 
            ulPos = 0 ; 
        } 
        pCtx->buf[ulPos++] = (uint8)Random() ; 
    } 
    p = pPlain ; 
    while(ulPlainLen > 0) 
    { 
        if(ulPos == 8) 
        { 
            EncryptEach8Bytes(pCtx) ; 
            ulPos = 0 ; 
        } 
        pCtx->buf[ulPos++] = *(p++) ; 
        ulPlainLen-- ; 
    } 
    for(ulPadding=0; ulPadding<7; ulPadding++) 
        pCtx->buf[ulPos++] = 0x00 ; 
    EncryptEach8Bytes(pCtx) ; 
    return ulOut ; 
} 
  
static void DecryptEach8Bytes(CZ_TEACTX *pCtx) 
{ 
#ifdef CRYPT_ONE_BYTE 
    uint32 i ; 
    uint8 bufTemp[8] ; 
    uint8 *pBuf8, *pBufPre8, *pCrypt8, *pCryptPre8 ; 
    pBuf8 = (uint8 *)pCtx->buf ; 
    pBufPre8 = (uint8 *)pCtx->bufPre ; 
    pCrypt8 = (uint8 *)pCtx->pCrypt ; 
    pCryptPre8 = (uint8 *)pCtx->pCryptPre ; 
    for(i=0; i<8; i++) 
        bufTemp[i] = pCrypt8[i] ^ pBufPre8[i] ; 
    DeCipher((uint32 *)bufTemp, (uint32 *)pCtx->pKey, (uint32 *)pBufPre8) ; 
    for(i=0; i<8; i++) 
        pBuf8[i] = pBufPre8[i] ^ pCryptPre8[i] ; 
#else 
    uint32 bufTemp[2] ; 
    uint32 *pBuf8, *pBufPre8, *pCrypt8, *pCryptPre8 ; 
    pBuf8 = (uint32 *)pCtx->buf ; 
    pBufPre8 = (uint32 *)pCtx->bufPre ; 
    pCrypt8 = (uint32 *)pCtx->pCrypt ; 
    pCryptPre8 = (uint32 *)pCtx->pCryptPre ; 
    bufTemp[0] = pCrypt8[0] ^ pBufPre8[0] ; 
    bufTemp[1] = pCrypt8[1] ^ pBufPre8[1] ; 
    DeCipher(bufTemp, (const uint32 *)pCtx->pKey, pBufPre8) ; 
    pBuf8[0] = pBufPre8[0] ^ pCryptPre8[0] ; 
    pBuf8[1] = pBufPre8[1] ^ pCryptPre8[1] ; 
#endif 
    pCtx->pCryptPre = pCtx->pCrypt ; 
    pCtx->pCrypt += 8 ; 
} 
  
uint32 CZ_Tea_Decrypt(CZ_TEACTX *pCtx, const uint8 *pCipher, uint32 ulCipherLen,  
    const uint8 *pKey, uint8 *pOut, uint32 *pOutLen) 
{ 
    uint32 ulPos, ulPadding, ulOut, ul ; 
    if(pCipher == NULL || pOutLen == NULL ||  
            ulCipherLen < 16 || (ulCipherLen & 0x07) != 0) 
        return 0 ;

    memset(pCtx, 0, sizeof(CZ_TEACTX)) ; 
    pCtx->pCrypt = pOut ; 
    pCtx->pCryptPre = pCtx->bufPre ; 
    pCtx->pKey = pKey ;

    DeCipher((const uint32 *)pCipher, (const uint32 *)pKey, (uint32 *)pCtx->bufPre) ; 
    for(ul=0; ul<8; ul++) 
        pCtx->buf[ul] = pCtx->bufPre[ul] ; 
    ulPos = pCtx->buf[0] & 0x07 ; 
    if(ulPos > 1) 
    { 
        for(ulOut=2; ulOut<=ulPos; ulOut++) 
        { 
            if(pCtx->buf[1] != pCtx->buf[ulOut]) 
            { 
                *pOutLen = 0 ; 
                return 0 ;
            } 
        } 
    } 
    ulOut = ulCipherLen - ulPos - 10 ; 
    if(ulPos + 10 > ulCipherLen || *pOutLen < ulOut) 
        return 0 ; 
    pCtx->pCryptPre = (uint8 *)pCipher ; 
    pCtx->pCrypt = (uint8 *)pCipher + 8 ; 
    ulPos++ ; 
    for(ulPadding=0; ulPadding<2; ulPadding++) 
    { 
        if(ulPos == 8) 
        { 
            DecryptEach8Bytes(pCtx) ; 
            ulPos = 0 ; 
        } 
        ulPos++ ; 
    } 
    // 
    for(ul=0; ul<ulOut; ul++) 
    { 
        if(ulPos == 8) 
        { 
            DecryptEach8Bytes(pCtx) ; 
            ulPos = 0 ; 
        } 
        pOut[ul] = pCtx->buf[ulPos] ; 
        ulPos++ ; 
    } 
    // 
    for(ulPadding=0; ulPadding<7; ulPadding++) 
    { 
        if(ulPos < 8) 
        { 
            if(pCtx->buf[ulPos] != 0x00) 
            { 
                *pOutLen = 0 ; 
                return 0 ; 
            } 
        } 
        ulPos++ ; 
    } 
    *pOutLen = ulOut ; 
    return 1 ; 
} 

/**---------------------------------------------------------------------------*
 **                         Compiler Flag                                     *
 **---------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif
