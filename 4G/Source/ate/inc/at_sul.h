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

#if !defined(__AT_SUL_H__)
#define __AT_SUL_H__


/******************************************************************************/
/*                           [begin] Ctype Manipulation                       */
/******************************************************************************/
#define _U  0x01    // upper 
#define _L  0x02    // lower 
#define _D  0x04    // digit 
#define _C  0x08    // cntrl 
#define _P  0x10    // punct 
#define _S  0x20    // white space (space/lf/tab) 
#define _X  0x40    // hex digit 
#define _SP 0x80    // hard space (0x20) 

const unsigned char _SUL_ctype[] =
{
    _C, _C, _C, _C, _C, _C, _C, _C,         /* 0-7 */
    _C, _C | _S, _C | _S, _C | _S, _C | _S, _C | _S, _C, _C,        /* 8-15 */
    _C, _C, _C, _C, _C, _C, _C, _C,         /* 16-23 */
    _C, _C, _C, _C, _C, _C, _C, _C,         /* 24-31 */
    _S | _SP, _P, _P, _P, _P, _P, _P, _P,           /* 32-39 */
    _P, _P, _P, _P, _P, _P, _P, _P,         /* 40-47 */
    _D, _D, _D, _D, _D, _D, _D, _D,         /* 48-55 */
    _D, _D, _P, _P, _P, _P, _P, _P,         /* 56-63 */
    _P, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U,   /* 64-71 */
    _U, _U, _U, _U, _U, _U, _U, _U,         /* 72-79 */
    _U, _U, _U, _U, _U, _U, _U, _U,         /* 80-87 */
    _U, _U, _U, _P, _P, _P, _P, _P,         /* 88-95 */
    _P, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L,   /* 96-103 */
    _L, _L, _L, _L, _L, _L, _L, _L,         /* 104-111 */
    _L, _L, _L, _L, _L, _L, _L, _L,         /* 112-119 */
    _L, _L, _L, _P, _P, _P, _P, _C,         /* 120-127 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     /* 128-143 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     /* 144-159 */
    _S | _SP, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, /* 160-175 */
    _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, _P, /* 176-191 */
    _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, _U, /* 192-207 */
    _U, _U, _U, _U, _U, _U, _U, _P, _U, _U, _U, _U, _U, _U, _U, _L, /* 208-223 */
    _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, _L, /* 224-239 */
    _L, _L, _L, _L, _L, _L, _L, _P, _L, _L, _L, _L, _L, _L, _L, _L
};      /* 240-255 */

#define _SUL_ismask(x) (_SUL_ctype[(int)(unsigned char)(x)])
#define SUL_isalnum(c)  ((_SUL_ismask(c)&(_U|_L|_D)) != 0)
#define SUL_isalpha(c)  ((_SUL_ismask(c)&(_U|_L)) != 0)
#define SUL_iscntrl(c)  ((_SUL_ismask(c)&(_C)) != 0)
#define SUL_isdigit(c)  ((_SUL_ismask(c)&(_D)) != 0)
#define SUL_isgraph(c)  ((_SUL_ismask(c)&(_P|_U|_L|_D)) != 0)
#define SUL_islower(c)  ((_SUL_ismask(c)&(_L)) != 0)
#define SUL_isprint(c)  ((_SUL_ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define SUL_ispunct(c)  ((_SUL_ismask(c)&(_P)) != 0)
#define SUL_isspace(c)  ((_SUL_ismask(c)&(_S)) != 0)
#define SUL_isupper(c)  ((_SUL_ismask(c)&(_U)) != 0)
#define SUL_isxdigit(c) ((_SUL_ismask(c)&(_D|_X)) != 0)
#define SUL_isascii(c) (((unsigned char)(c))<=0x7f)
#define SUL_toascii(c) (((unsigned char)(c))&0x7f)

static inline unsigned char _SUL_tolower(unsigned char c)
{
    if (SUL_isupper(c))
        c -= 'A' - 'a';
    return c;
}

static inline unsigned char _SUL_toupper(unsigned char c)
{
    if (SUL_islower(c))
        c -= 'a' - 'A';
    return c;
}


#define SUL_tolower(c) _SUL_tolower(c)
#define SUL_toupper(c) _SUL_toupper(c)

#define SUL_mkupper(c)  ( (c)-'a'+'A' )
#define SUL_mklower(c)  ( (c)-'A'+'a' )
#define SUL_mkupper(c)  ( (c)-'a'+'A' )
#define SUL_mklower(c)  ( (c)-'A'+'a' )

/******************************************************************************/
/*                           [end] Ctype Manipulation                         */
/******************************************************************************/



/******************************************************************************/
/*                           [begin] String Manipulation                      */
/******************************************************************************/

#endif // _H_

