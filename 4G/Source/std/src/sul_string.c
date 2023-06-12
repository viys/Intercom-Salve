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


#include<stdio.h>

/* flags definition */
/* 4 bits reserved for base :) */
#define FLAG_BASE_8     8
#define FLAG_BASE_10    10
#define FLAG_BASE_16    16
#define MASK_FLAG_BASE  0x1a

#define FLAG_HALF_WORD  0x01
#define FLAG_LONG_WORD  0x04
#define FLAG_ZERO_PAD   0x20
#define FLAG_RIGHT_PAD  0x40
#define FLAG_WITH_PREC  0x80
#define FLAG_SPACE      0x100
#define FLAG_PLUS       0x200
#define FLAG_SIGNED     0x400

#define _put(c,buf,ebuf,count)  __put(c,&buf,ebuf,&count)
static void __put(char c,char **_buf,char *ebuf,int *pcount)
{
    char *buf = *_buf;

    if (ebuf>buf)
    {
        *buf++ = (c);
        *_buf = buf;
        *pcount += 1;
    }
    else
        *buf = '\0';
}
/*
 * Macros for converting digits to letters and vice versa
 */
#define to_digit(c) ((c) - '0')
// #define is_digit(c) ((unsigned)to_digit(c) <= 9)
#define to_char(n)  ((char)((n) + '0'))

INT32 SUL_StrVPrint(
    TCHAR   *str,
    CONST TCHAR *fmtstr,
    va_list arg
);

static int
new_printn(unsigned long ul, unsigned int width, unsigned int flags,char *tbuf, char *ebuf, unsigned int capcase)
{
    /* hold a long in base 8 */
    char *p, buf[(sizeof(long) * 8 / 3) + 1];
    unsigned int len, pad;
    char sign = 0;
    unsigned int base = flags & MASK_FLAG_BASE;
    int count=0;
    const char *nllow = "0123456789abcdef";
    const char *nlcap = "0123456789ABCDEF";
    const char *nl;

    if (capcase)
    {
        nl = nlcap;
    }
    else
    {
        nl = nllow;
    }

    if (flags & FLAG_SIGNED)
    {
        if ( (flags & FLAG_HALF_WORD) && ((short) ul < 0))
        {
            sign = '-';
            ul = -(short)ul;
        }
        else if ((long)ul < 0)
        {
            sign = '-';
            ul = -(long)ul;
        }
        /* Plus overrides space ... */
        else if (flags & FLAG_PLUS)
        {
            sign = '+';
        }
        else if (flags & FLAG_SPACE)
        {
            sign = ' ';
        }
    }

    else if (flags & FLAG_HALF_WORD)
        ul = (unsigned short) ul;

    p = buf;
    do
    {
        *p++ = nl[ul % base];
    }
    while (ul /= base);

    len = (unsigned int) (p - buf);
    pad =  width - len;

    if (sign)
    {
        if (flags&FLAG_ZERO_PAD) _put(sign,tbuf,ebuf,count);
        pad--;
    }

    if ( !(flags&FLAG_RIGHT_PAD) && (width > len))
    {
        while (pad--)
        {
            _put((flags & FLAG_ZERO_PAD) ? '0' : ' ',tbuf,ebuf,count);
        }
    }

    if (sign && !(flags & FLAG_ZERO_PAD))
        _put(sign,tbuf,ebuf,count);

    do
    {
        _put(*--p,tbuf,ebuf,count);
    }
    while (p > buf);

    if ( (flags&FLAG_RIGHT_PAD) && (width > len))
    {
        while (pad--)
        {
            _put(' ',tbuf,ebuf,count);
        }
    }

    return count;
}

static int
new_doprnt(const char *fmt,char *buf,char *ebuf, va_list ap)
{
    char *p;
    const char *fmt0;
    int ch;
    unsigned long ul;
    unsigned int flags = 0;
    unsigned int width = 0;
    unsigned int prec = 0;
    int count=0,tmp_count=0;

    // Handle the null string.
    if (fmt == NULL)
    {
        return 0;
    }

    for (;;)
    {
        while ((ch = *fmt++) != '%')
        {
            if (ch == '\0')
            {
                _put(ch,buf,ebuf,count);
                return count-1;
            }
            _put(ch,buf,ebuf,count);
        }
        flags = width = prec = 0;
        fmt0 = fmt; // useful to test if we are at the first char
reswitch:   switch (ch = *fmt++)
        {
            case '0':
                // 0 can be either zero padding if first after %
                //   or part of a precision or width if after... /
                if (fmt == fmt0 + 1)
                {
                    flags |= FLAG_ZERO_PAD;
                    goto reswitch;
                }
            // else fall through
            case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                if (flags & FLAG_WITH_PREC)
                {
                    prec = 10 * prec + to_digit(ch);
                }
                else
                {
                    width = 10 * width + to_digit(ch);
                }
                goto reswitch;
            case '-':
                flags |= FLAG_RIGHT_PAD;
                goto reswitch;
            case ' ':
                flags |= FLAG_SPACE;
                goto reswitch;
            case '+':
                flags |= FLAG_PLUS;
                goto reswitch;
            case '.':
                flags |= FLAG_WITH_PREC;
                goto reswitch;
            case 'h':
                flags |= FLAG_HALF_WORD;
                goto reswitch;
            case 'l':
                flags |= FLAG_LONG_WORD;
                goto reswitch;
            case 'c':
                ch = va_arg(ap, int);
                _put(ch & 0x7f,buf,ebuf,count);
                break;
            case 's':
                p = va_arg(ap, char *);
                if(p != NULL)
                {
                    if ((flags & FLAG_WITH_PREC) && prec)
                    {
                        while ((prec--) && (ch = *p++))
                            _put(ch,buf,ebuf,count);
                    }
                    else
                    {
                        while ((ch = *p++))
                            _put(ch,buf,ebuf,count);
                    }
                }
                break;
            case 'i':
            case 'd':
                ul = va_arg(ap, long);
                flags |= (FLAG_BASE_10|FLAG_SIGNED);
                tmp_count = new_printn(ul, width, flags, buf, ebuf, 0);
                buf+= tmp_count;
                count += tmp_count;
                break;
            case 'o':
                ul = va_arg(ap, unsigned long);
                flags |= FLAG_BASE_8;
                tmp_count = new_printn(ul, width, flags, buf, ebuf, 0);
                buf += tmp_count;
                count += tmp_count;
                break;
            case 'u':
                ul = va_arg(ap, unsigned long);
                flags |= FLAG_BASE_10;
                tmp_count = new_printn(ul, width, flags, buf, ebuf, 0);
                buf += tmp_count;
                count += tmp_count;
                break;
            case 'p':
                _put('0',buf,ebuf,count);
                _put('x',buf,ebuf,count);
            // fall through
            case 'x':
                ul = va_arg(ap, unsigned long);
                flags |= FLAG_BASE_16;
                tmp_count = new_printn(ul, width, flags, buf, ebuf, 0);
                buf += tmp_count;
                count += tmp_count;
                break;
            case 'X':
                ul = va_arg(ap, unsigned long);
                flags |= FLAG_BASE_16;
                tmp_count = new_printn(ul, width, flags, buf, ebuf, 1);
                buf += tmp_count;
                count += tmp_count;
                break;
            case '%':
                _put(ch,buf,ebuf,count);
                break;
            default:
                _put('%',buf,ebuf,count);
                if (flags & FLAG_HALF_WORD)
                    _put('h',buf,ebuf,count);
                if (flags & FLAG_LONG_WORD)
                    _put('l',buf,ebuf,count);
                // dingmx fix the bug
                if (ch == '\0')
                {
                    _put(ch,buf,ebuf,count);
                    return count-1;
                }
                _put(ch,buf,ebuf,count);
        } /* end switch */
    } /* end for */
}


static int
new_vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
    int count;
    char *ebuf = buf+size-1;
    count = new_doprnt(fmt, buf, ebuf, ap);
    return (count);
}

static int
new_vsprintf(char *buf, const char *fmt, va_list ap)
{
    return (new_vsnprintf(buf, -(size_t)buf, fmt, ap));
}


INT32 SUL_Vsprintf(TCHAR *buf, const TCHAR *fmt, va_list args)
{
    return new_vsprintf((char*)buf,  (char*)fmt, args);
}


BOOL SUL_MemSet8(
    void *pSource,  //Pointer to the start of the area.
    INT32 c,        //The byte to fill the area with
    UINT16 count    //The size of the area.
)
{
    TCHAR *xs = (TCHAR *) pSource;

    while (count--)
        *xs++ = (TCHAR)c;

    return(1);
}

VOID *SUL_MemCopy8 (
    VOID  *dest,        //Where to copy to
    CONST VOID *src,  //Where to copy from
    UINT32 count        //The size of the area.
)
{
    TCHAR *tmp = (TCHAR *) dest, *s = (TCHAR *) src;

    while (count--)
        *tmp++ = *s++;

    return dest;
}

INT32 SUL_StrPrint(
    TCHAR   *buffer,        // Storage location for output
    CONST TCHAR *format,        // Format-control string
    ...                         // Optional arguments
)
{
    va_list args;
    INT32 Tmp;

    va_start(args, format);
    Tmp = SUL_StrVPrint(buffer, format, args);
    va_end(args);
    return Tmp;
}
INT32 SUL_StrVPrint(
    TCHAR   *str,
    CONST TCHAR *fmtstr,
    va_list arg
)
{

    return(SUL_Vsprintf(str, fmtstr, arg));
}


