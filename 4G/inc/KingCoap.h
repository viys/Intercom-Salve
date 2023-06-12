#ifndef __KING_COAP_H__
#define __KING_COAP_H__

#include "KingDef.h"


/*******************************************************************************
 ** MACROS
 ******************************************************************************/

#define COAP_BLOCK_MAX_SIZE 64   /* maximum size of block */

typedef enum {
    COAP_GET_SUCCESS = 0,
    COAP_GET_FAIL,
    COAP_PUT_SUCCESS,
    COAP_PUT_FAIL,
    COAP_POST_SUCCESS,
    COAP_POST_FAIL,
    COAP_DELETE_SUCCESS,
    COAP_DELETE_FAIL
} COAP_EVENT;

typedef struct _coap_data
{
    uint8 *gBuff[COAP_BLOCK_MAX_SIZE];
    int gNum;               ///< valid gBuff number of COAP_DATA
    int gLength;            ///< total length of all gBuff in COAP_DATA
    int gLen[COAP_BLOCK_MAX_SIZE];  ///< length of specific gBuff
} COAP_DATA_T;

/**
 * callback function for COAP events.
 *
 * @param event_type COAP_EVENT
 * @param event_content COAP_DATA_T
 */
typedef void (*COAP_CALLBACK)(COAP_EVENT event_type, void* event_content);

int KING_CoapInit(void);

int KING_CoapSetCallBack(COAP_CALLBACK cb);

int KING_CoapSetData(uint16 length, uint8* data);

int KING_CoapGet(uint8 *url, uint8 *cmdline, uint8 wait_seconds);

int KING_CoapPut(uint8 *url, uint8 *cmdline, uint8 wait_seconds, bool has_payload);

int KING_CoapPost(uint8 *url, uint8 *cmdline, uint8 wait_seconds, bool has_payload);

int KING_CoapDelete(uint8 *url, uint8 *cmdline, uint8 wait_seconds);

int KING_CoapDeinit(void);

#endif

