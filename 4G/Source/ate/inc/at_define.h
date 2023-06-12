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


#ifndef _AT_DEFINE_H
#define _AT_DEFINE_H

#include "kingcstd.h"
#include "at_utils.h"
#include "at_utility.h"

extern BOOL SUL_MemSet8(
    void *pSource,  //Pointer to the start of the area.
    INT32 c,        //The byte to fill the area with
    UINT16 count    //The size of the area.
);

extern VOID *SUL_MemCopy8 (
    VOID  *dest,        //Where to copy to
    CONST VOID *src,  //Where to copy from
    UINT32 count        //The size of the area.
);



#define AT_MALLOC                          at_malloc
#define AT_FREE                            at_free

#ifndef CFW_GPRS_SUPPORT
#define AT_NO_GPRS
#endif

#define AT_MemSet                          memset
#define AT_MemCpy                          memcpy
#define AT_MemZero(str,len)                memset((str), 0, (len))
#define AT_StrLen(str)                     strlen((char*)str)
#define AT_StrCpy(dst,src)                 strcpy((char*)dst,(char*)src)
#define AT_StrNCpy(dst,src,size)           do { \
        strncpy((char*)dst,(char*)src,size); \
        dst[size] = '\0'; \
    } while(0)
#define AT_StrCmp(str1,str2)               strcmp((char*)str1,(char*)str2)
#define AT_StrNCmp(str1,str2,size)         strncmp((char*)str1,(char*)str2, size)
 #define AT_StrNCaselessCmp(str1,str2,size) strnicmp((char*)str1,(char*)str2, size)
#define AT_StrCaselessCmp(str1,str2)  strnicmp((char*)str1,(char*)str2, strlen((const char*)str1) > strlen((const char*)str2) ? strlen((const char*)str1) : strlen((const char*)str2))
#define AT_StrChr(str,ch)                  strchr((char*)str,(int)ch)
#define AT_StrRChr(str,ch)                 strrchr((char*)str,(int)ch)
#define AT_StrCat(dst, src)                strcat((char*)dst,(char*)src)
#define AT_StrAToI                         atoi

extern char *strupr(char *str);

#define AT_StrUpr(str)                     strupr((char*)str)
#define AT_Sprintf                         sprintf

#endif
