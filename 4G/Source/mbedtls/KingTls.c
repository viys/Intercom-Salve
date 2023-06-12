/*
 *  TCP/IP or UDP/IP networking functions
 *
 */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "KingTLS.h"
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "net_sockets.h"
#include "kingsocket.h"

#ifdef MBEDTLS_KING_SOCKETS

/*
 * Initialize a context
 */
void mbedtls_net_init( mbedtls_net_context *ctx )
{
    KING_SysLog("mbedtls_net_init");
    ctx->fd = -1;
}

/*
 * Set the socket blocking or non-blocking
 */
int mbedtls_net_set_block( mbedtls_net_context *ctx )
{
    int n = 0;
    
    return( KING_SocketOptSet( ctx->fd, 0, SO_BIO, &n, 0 ) );
}

int mbedtls_net_set_nonblock( mbedtls_net_context *ctx )
{
    int n = 1;
    
    return( KING_SocketOptSet( ctx->fd, 0, SO_NBIO, &n, 0 ) );
}

/*
 * Portable usleep helper
 */
void mbedtls_net_usleep( unsigned long usec )
{
    struct timeval tv;
    
    tv.tv_sec  = usec / 1000000;
    KING_SocketSelect( 0, NULL, NULL, NULL, &tv );
}

/*
 * Read at most 'len' characters
 */
int mbedtls_net_recv( void *ctx, unsigned char *buf, size_t len )
{
    int ret = -1;
    int fd = ((mbedtls_net_context *) ctx)->fd;
    
    KING_SysLog("mbedtls_net_recv() fd=%d, len=%d", fd, len);

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );
    
    ret = (int) KING_SocketRecv( fd, buf, len, 0 );
    KING_SysLog("mbedtls_net_recv() ret=%d", ret);
    if( ret < 0 )
    {
        return( MBEDTLS_ERR_NET_RECV_FAILED );
    }
    else if (ret == 0)
    {
        return( MBEDTLS_ERR_SSL_WANT_READ );
    }
    
    return( ret );
}

/*
 * Read at most 'len' characters, blocking for at most 'timeout' ms
 */
int mbedtls_net_recv_timeout( void *ctx, unsigned char *buf,
                              size_t len, uint32_t timeout )
{
    int ret = -1;
    struct timeval tv;
    fd_set read_fds;
    int fd = ((mbedtls_net_context *) ctx)->fd;
    
    KING_SysLog("mbedtls_net_recv_timeout() fd=%d, len=%d", fd, len);

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    FD_ZERO( &read_fds );
    FD_SET( fd, &read_fds );

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = ( timeout % 1000 ) * 1000;

    ret = KING_SocketSelect( fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv );
    KING_SysLog("mbedtls_net_recv_timeout() ret=%d", ret);
    /* Zero fds ready means we timed out */
    if (ret == 0)
    {
        return (MBEDTLS_ERR_SSL_TIMEOUT);
    }
    if( ret < 0 )
    {
        return( MBEDTLS_ERR_NET_RECV_FAILED );
    }

    /* This call will not block */
    return( mbedtls_net_recv( ctx, buf, len ) );
}

/*
 * Write at most 'len' characters
 */
int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len )
{
    int ret = -1;
    int fd = ((mbedtls_net_context *) ctx)->fd;
    
    KING_SysLog("mbedtls_net_send() fd=%d, len=%d", fd, len);

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    ret = (int) KING_SocketSend( fd, buf, len, 0 );
    KING_SysLog("mbedtls_net_send() ret=%d", ret);
    if( ret < 0 )
    {
        return( MBEDTLS_ERR_NET_SEND_FAILED );
    }
    return( ret );
}

/*
 * Gracefully close the connection
 */
void mbedtls_net_free( mbedtls_net_context *ctx )
{
    KING_SysLog("mbedtls_net_free() ctx->fd =%d", ctx->fd );
    if( ctx->fd == -1 )
        return;

    KING_SocketShutdown( ctx->fd, 2 );
    KING_SocketClose( ctx->fd );

    ctx->fd = -1;
}

#endif // MBEDTLS_KING_SOCKETS

