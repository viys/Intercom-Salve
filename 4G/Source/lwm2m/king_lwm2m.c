///////////////////////////////////////////////////////////////
// This files include functions for porting liblwm2m to KingSDK.
///////////////////////////////////////////////////////////////

#include "king_lwm2m.h"
#include "internals.h"

// Allocate a block of size bytes of memory, returning a pointer to the beginning of the block.
void * lwm2m_malloc(size_t s)
{
    int ret = 0;
    void *pMem = NULL;

    ret = KING_MemAlloc(&pMem, s);
    if (ret == 0 && pMem != NULL)
        return pMem;

    return NULL;
}


// Deallocate a block of memory previously allocated by lwm2m_malloc() or lwm2m_strdup()
void lwm2m_free(void * p)
{
    if (NULL != p)
    {
        KING_MemFree(p);
    }
}


// Allocate a memory block, duplicate the string str in it and return a pointer to this new block.
char * lwm2m_strdup(const char * str)
{
    int ret = -1;
    void *pStr = NULL;

    if (NULL == str) return NULL;

    ret = KING_MemAlloc(&pStr, strlen(str) + 1);

    if (ret != 0 || pStr == NULL) return NULL;

    memcpy(pStr, str, strlen(str) + 1);

    return (char*)pStr;
}


// This function must return the number of seconds elapsed since origin.
// The origin (Epoch, system boot, etc...) does not matter as this
// function is used only to determine the elapsed time since the last
// call to it.
// In case of error, this must return a negative value.
// Per POSIX specifications, time_t is a signed integer.
time_t lwm2m_gettime(void)
{
    int ret = -1;
    struct timeval tv;

    ret = KING_GetTimeOfDay(&tv, NULL);

    if (ret == 0)
    {
        return (tv.tv_sec);
    }

    return -1;
}

int lwm2m_strncmp(const char * s1,
                     const char * s2,
                     size_t n)
{
    return strncmp(s1, s2, n);
}


lwm2m_callback callback = NULL;

#define EVID_CASE(nEvId) \
    case nEvId:          \
        return ((uint8_t *)#nEvId)

uint8_t *lwm2m_eventName(uint8_t nEvId)
{
    switch (nEvId)
    {
        EVID_CASE(LWM2M_EVENT_BOOTSTRAP_START);
        EVID_CASE(LWM2M_EVENT_BOOTSTRAP_SUCCESS);
        EVID_CASE(LWM2M_EVENT_BOOTSTRAP_FAILED);
        EVID_CASE(LWM2M_EVENT_CONNECT_SUCCESS);
        EVID_CASE(LWM2M_EVENT_CONNECT_FAILED);
        EVID_CASE(LWM2M_EVENT_REG_SUCCESS);
        EVID_CASE(LWM2M_EVENT_REG_FAILED);
        EVID_CASE(LWM2M_EVENT_REG_TIMEOUT);
        EVID_CASE(LWM2M_EVENT_LIFETIME_TIMEOUT);
        EVID_CASE(LWM2M_EVENT_UPDATE_SUCCESS);
        EVID_CASE(LWM2M_EVENT_UPDATE_FAILED);
        EVID_CASE(LWM2M_EVENT_UPDATE_TIMEOUT);
        EVID_CASE(LWM2M_EVENT_RESPONSE_FAILED);
        EVID_CASE(LWM2M_EVENT_NOTIFY_FAILED);
        EVID_CASE(LWM2M_EVENT_NOTIFY_SEND_SUCCESS);
        EVID_CASE(LWM2M_EVENT_NOTIFY_SENT_SUCCESS);
        EVID_CASE(LWM2M_EVENT_NOTIFY_SENT_TIMEOUT);
        EVID_CASE(LWM2M_EVENT_UPDATE_BINDING_SEND);
        EVID_CASE(LWM2M_EVENT_UPDATE_BINDING_SUCCESS);
        EVID_CASE(LWM2M_EVENT_UPDATE_BINDING_FAILED);
        EVID_CASE(LWM2M_EVENT_UPDATE_BINDING_TIMEOUT);
        EVID_CASE(LWM2M_EVENT_DEREG_SUCCESS);
        EVID_CASE(LWM2M_EVENT_DEREG_FAILED);
        EVID_CASE(LWM2M_EVENT_DEREG_TIMEOUT);
        default:
        return (uint8_t *)"Unknown Event name";
    }
}

void lwm2m_sethandler(lwm2m_callback cb)
{
    callback = cb;
}

void lwm2m_postevent(LWM2M_EVENT_IND eventId, uint32_t result, uint32_t nParam1, uint32_t nParam2)
{
    //Is there a need for separate threads to handle events?
    LWM2M_EVENT_CONTEXT context = {0};
    context.id = eventId;
    context.param1 = result;
    context.param2 = nParam1;
    context.param3 = nParam2;
    if(callback != NULL)
        callback(&context);
}


