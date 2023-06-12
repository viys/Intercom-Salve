/**
 * Copyright (c) 2019 Xiamen CHEERZING IOT Technology Co., Ltd.
 * All rights reserved.
 * Reference:
 *  tinydtls - https://sourceforge.net/projects/tinydtls/?source=navbar
**/

#include "peer.h"


static inline dtls_peer_t* dtls_malloc_peer()
{
    dtls_peer_t *p = NULL;
    KING_MemAlloc((void **)&p, sizeof(dtls_peer_t));
    return p;
}

void dtls_free_peer( dtls_peer_t *peer )
{
    dtls_handshake_free( peer->handshake_params );
    dtls_security_free( peer->security_params[0] );
    dtls_security_free( peer->security_params[1] );
    if(peer->session!=NULL)
        KING_MemFree((void *)(peer->session));
    KING_MemFree( peer );
}

dtls_peer_t* dtls_new_peer( const session_t *session )
{
    dtls_peer_t *peer;

    peer = dtls_malloc_peer();
    if ( peer )
    {
        memset( peer,0,sizeof(dtls_peer_t) );
        KING_MemAlloc((void **)&(peer->session), sizeof(session_t));
        if(!peer->session)
        {
            dtls_free_peer( peer );
            return NULL;
        }
        memcpy((void *)(peer->session),(void *)session,sizeof(session_t ));
        peer->security_params[0] = dtls_security_new();

        if ( !peer->security_params[0] )
        {
            dtls_free_peer( peer );
            return NULL;
        }
    }

    return peer;
}
