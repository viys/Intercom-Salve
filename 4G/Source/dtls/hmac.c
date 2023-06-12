/**
 * Copyright (c) 2019 Xiamen CHEERZING IOT Technology Co., Ltd.
 * All rights reserved.
 * Reference:
 *  tinydtls - https://sourceforge.net/projects/tinydtls/?source=navbar
**/

#include "hmac.h"


static inline dtls_hmac_context_t* dtls_hmac_context_new( void )
{
    dtls_hmac_context_t *p = NULL;
    KING_MemAlloc((void **)&p, sizeof(dtls_hmac_context_t));
    return p;
}

static inline void dtls_hmac_context_free( dtls_hmac_context_t *ctx )
{
    KING_MemFree( ctx );
}

void dtls_hmac_storage_init( void )
{
    /* add your code here */
}

void dtls_hmac_update( dtls_hmac_context_t *ctx,
                       const unsigned char *input,
                       size_t               ilen )
{
    if ( NULL == ctx ||
         NULL == input )
    {
        return ;
    }

    dtls_hash_update( &ctx->data, input, ilen );
}

dtls_hmac_context_t* dtls_hmac_new( const unsigned char *key,
                                    size_t               klen )
{
    dtls_hmac_context_t *ctx;

    ctx = dtls_hmac_context_new();
    if ( ctx )
        dtls_hmac_init( ctx, key, klen );

    return ctx;
}

void dtls_hmac_init( dtls_hmac_context_t *ctx,
                     const unsigned char *key,
                     size_t               klen )
{
    int i;

    if ( NULL == ctx ||
         NULL == key )
    {
        return ;
    }

    memset( ctx,0, sizeof(dtls_hmac_context_t) );

    if ( klen > DTLS_HMAC_BLOCKSIZE )
    {
        dtls_hash_init( &ctx->data );
        dtls_hash_update( &ctx->data, key, klen );
        dtls_hash_finalize( ctx->pad, &ctx->data );
    }
    else
        memmove( ctx->pad, key, klen );

    /* create ipad: */
    for ( i = 0; i < DTLS_HMAC_BLOCKSIZE; ++i )
        ctx->pad[i] ^= 0x36;

    dtls_hash_init( &ctx->data );
    dtls_hmac_update( ctx, ctx->pad, DTLS_HMAC_BLOCKSIZE );

    /* create opad by xor-ing pad[i] with 0x36 ^ 0x5C: */
    for ( i = 0; i < DTLS_HMAC_BLOCKSIZE; ++i )
        ctx->pad[i] ^= 0x6A;
}

void dtls_hmac_free( dtls_hmac_context_t *ctx )
{
    if ( ctx )
        dtls_hmac_context_free( ctx );
}

int dtls_hmac_finalize( dtls_hmac_context_t *ctx,
                        unsigned char       *result )
{
    unsigned char buf[DTLS_HMAC_DIGEST_SIZE];
    size_t len;

    if ( NULL == ctx ||
         NULL == result )
    {
        return 0;
    }

    len = dtls_hash_finalize( buf, &ctx->data );

    dtls_hash_init( &ctx->data );
    dtls_hash_update( &ctx->data, ctx->pad, DTLS_HMAC_BLOCKSIZE );
    dtls_hash_update( &ctx->data, buf, len );

    len = dtls_hash_finalize( result, &ctx->data );

    return len;
}
