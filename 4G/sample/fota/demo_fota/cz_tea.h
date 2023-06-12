#ifndef _CZ_TEA_H_
#define _CZ_TEA_H_ 

#include "KingDef.h"

typedef struct tagTEACTX 
{ 
    uint8 buf[8] ; 
    uint8 bufPre[8] ; 
    const uint8 *pKey ; //指向16字节的key  
    uint8 *pCrypt ; 
    uint8 *pCryptPre ; 
} CZ_TEACTX, *CZ_LPTEACTX ; 

uint16 CZ_Host2NetShort(uint16 usHost);
uint16 CZ_Net2HostShort(uint16 usNet);
uint32 CZ_Host2NetLong(uint32 ulHost); 
uint32 CZ_Net2HostLong(uint32 ulHost);

uint32 CZ_Tea_Encrypt(CZ_TEACTX *pCtx, const uint8 *pPlain, uint32 ulPlainLen,  
    const uint8 *pKey, uint8 *pOut, uint32 *pOutLen);
    
uint32 CZ_Tea_Decrypt(CZ_TEACTX *pCtx, const uint8 *pCipher, uint32 ulCipherLen,  
    const uint8 *pKey, uint8 *pOut, uint32 *pOutLen) ;
 
#endif /*_WSOP_TEA_H_*/

