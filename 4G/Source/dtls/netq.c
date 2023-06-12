/**
 * Copyright (c) 2019 Xiamen CHEERZING IOT Technology Co., Ltd.
 * All rights reserved.
 * Reference:
 *  tinydtls - https://sourceforge.net/projects/tinydtls/?source=navbar
**/

#include "netq.h"
#include "debug.h"


static inline netq_t* netq_malloc_node( size_t size )
{
    netq_t *p = NULL;
    KING_MemAlloc((void **)&p, sizeof(netq_t)+size );
    return p;
}

static inline void netq_free_node( netq_t *node )
{
    KING_MemFree( node );
}

int netq_insert_node( list_t  queue,
                      netq_t *node )
{
    netq_t *p;

    if ( NULL == queue ||
         NULL == node )
    {
        return 0;
    }

    p = (netq_t *)list_head( queue );
    while ( p && p->t <= node->t && list_item_next( p ) )
        p =(netq_t *)list_item_next( p );

    if ( p )
        list_insert( queue, p, node );
    else
        list_push( queue, node );

    return 1;
}

netq_t* netq_head( list_t queue )
{
    if ( !queue )
        return NULL;

    return (netq_t* )list_head( queue );
}

netq_t* netq_next( netq_t *p )
{
    if ( !p )
        return NULL;

    return (netq_t* )list_item_next( p );
}

void netq_remove( list_t  queue,
                  netq_t *p )
{
    if ( !queue || !p )
        return;

    list_remove( queue, p );
}

netq_t* netq_pop_first( list_t queue )
{
    if ( !queue )
        return NULL;

    return (netq_t* )list_pop( queue );
}

netq_t* netq_node_new( size_t size )
{
    netq_t *node;
    node = netq_malloc_node( size );

    if ( !node )
        dtls_warn( "netq_node_new: malloc\n" );

    if ( node!=NULL )
        memset( node,0, sizeof(netq_t) );

    return node;
}

void netq_node_free( netq_t *node )
{
    if ( node )
        netq_free_node( node );
}

void netq_delete_all( list_t queue )
{
    netq_t *p;
    if ( queue )
    {
        while ( (p = (netq_t *)list_pop( queue ))!=NULL )
            netq_free_node( p );
    }
}
