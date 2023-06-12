/**
 * Copyright (c) 2019 Xiamen CHEERZING IOT Technology Co., Ltd.
 * All rights reserved.
 * Reference:
 *  tinydtls - https://sourceforge.net/projects/tinydtls/?source=navbar
**/
#include "debug.h"

#if KING_DTLS_LOG
void dsrv_hexdump_log( const char          *name,
                       const unsigned char *buf,
                       size_t               length,
                       int                  extend )
{
    int n = 0;

    if ( extend )
    {
        LogPrintf( "%s: (%d bytes):\n", name, length );

        while ( length-- )
        {
            if ( n % 16 == 0 )
                LogPrintf( "%08X ", n );

            LogPrintf( "%02X ", *buf++ );

            n++;
            if ( n % 8 == 0 )
            {
                if ( n % 16 == 0 )
                    LogPrintf( "\n" );
                else
                    LogPrintf( " " );
            }
        }
    }
    else
    {
        LogPrintf( "%s: (%u bytes): ", name, length );
        while ( length-- )
            LogPrintf( "%02X", *buf++ );
    }
    LogPrintf( "\n" );
}
#endif
