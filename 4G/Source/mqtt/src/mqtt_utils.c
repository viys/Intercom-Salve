/*******************************************************************************
 ** File Name:   mqtt_utils.c
 ** Author:      Allance.Chen
 ** Date:        2020-06-04
 ** Copyright:   Copyright 2020-2022 by Xiamen CHEERZING IOT Technology Co., Ltd.
 **              All Rights Reserved.
 **              This software is supplied under the terms of a license
 **              agreement or non-disclosure agreement with Cheerzing.
 **              Passing on and copying of this document,and communication
 **              of its contents is not permitted without prior written
 **              authorization.
 ** Description: This file contains ds functions about mqtt util.
 *******************************************************************************

 *******************************************************************************
 **                        Edit History                                        *
 ** ---------------------------------------------------------------------------*
 ** DATE           NAME             Description:
 **----------------------------------------------------------------------------*
 ** 2020-06-04     Allance.Chen         Create.
 ******************************************************************************/
/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "mqtt_utils.h"
#include "kingRtc.h"
#include "kingRtos.h"
#include "kingcstd.h"
#include "infra_defs.h"
#include "dev_sign_api.h"

/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define UINT32_MAX           4294967295u

/*******************************************************************************
 ** Variables
 ******************************************************************************/
static unsigned long int next = 1;
char _product_key[IOTX_PRODUCT_KEY_LEN + 1] = "a1MZxOdcBnO";
char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "h4I4dneEFp7EImTv";
char _device_name[IOTX_DEVICE_NAME_LEN + 1] = "test_01";
char _device_secret[IOTX_DEVICE_SECRET_LEN + 1] = "t9GmMf2jb3LgWfXBaZD2r3aJrfVWBv56";
char _host_name[DEV_SIGN_HOSTNAME_MAXLEN + 1] = {0};
char _user_name[DEV_SIGN_USERNAME_MAXLEN + 1] = {0};
char _password[DEV_SIGN_PASSWORD_MAXLEN + 1] = {0};
char _client_id[DEV_SIGN_CLIENT_ID_MAXLEN + 1] = {0};
uint16 mqtt_port = 1883;

/*******************************************************************************
 ** External Function Delcarations
 ******************************************************************************/

/*******************************************************************************
 **  location Function 
 ******************************************************************************/
static int random(void)
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}

/*******************************************************************************
 **  Function 
 ******************************************************************************/
void mqtt_utils_MutexCreate(void **mutex)
{
    int ret = 0;
    
    ret = KING_MutexCreate("mqtt_mutex", 0, mutex);
    if (0 != ret || mutex == NULL) 
    {
        mqtt_info("create mutex failed");
        return;
    }

    return;
}

void mqtt_utils_MutexDestroy(void *mutex)
{
    if (mutex == NULL) 
    {
        mqtt_info("mutex want to destroy is NULL!");
        return;
    }
    if (0 != KING_MutexDelete(mutex)) 
    {
        mqtt_info("destroy mutex failed");
    }
}

void mqtt_utils_MutexLock(void *mutex)
{
    int ret = 0;

    ret = KING_MutexLock(mutex, WAIT_OPT_INFINITE);
    if (0 != ret) 
    {
        mqtt_info("lock mutex failed: - %d", ret);
    }
}

void mqtt_utils_MutexUnlock(void *mutex)
{
    int ret = 0;

    ret = KING_MutexUnLock(mutex);
    if (0 != ret) 
    {
        mqtt_info("unlock mutex failed - %d", ret);
    }
}

void *mqtt_utils_Malloc(uint32 size)
{
    return malloc(size);
}

void *mqtt_utils_Realloc(void *ptr, uint32 size)
{
    return realloc(ptr, size);
}

void mqtt_utils_Free(void *ptr)
{
    free(ptr);
}

uint32 mqtt_utils_Random(uint32 region)
{
    return (region > 0) ? (random() % region) : 0;
}

uint64 mqtt_utils_UptimeMs(void)
{
    uint32 time_ms;

    KING_CurrentTickGet(&time_ms);
    return time_ms;
}

void mqtt_utils_SleepMs(uint32 ms)
{
    KING_Sleep(ms);
}

void mqtt_utils_time_start(mqtt_time_t *timer)
{
    if (!timer) 
    {
        return;
    }

    timer->time = mqtt_utils_UptimeMs();
}

uint32 mqtt_utils_time_spend(mqtt_time_t *start)
{
    uint32 now, res;

    if (!start) 
    {
        return 0;
    }

    now = mqtt_utils_UptimeMs();
    res = now - start->time;
    return res;
}

uint32 mqtt_utils_time_left(mqtt_time_t *end)
{
    uint32 now, res;

    if (!end) 
    {
        return 0;
    }

    if (mqtt_utils_time_is_expired(end)) 
    {
        return 0;
    }

    now = mqtt_utils_UptimeMs();
    if (end->time > now)
    {
        res = end->time - now;
    }
    else
    {
        res = 0;
    }
    return res;
}

uint32 mqtt_utils_time_is_expired(mqtt_time_t *timer)
{
    uint32 cur_time;

    if (!timer) 
    {
        return 1;
    }

    cur_time = mqtt_utils_UptimeMs();
    if ((cur_time - timer->time) < (UINT32_MAX / 2)) 
    {
        return 1;
    } 
    else 
    {
        return 0;
    }
}

void mqtt_utils_time_init(mqtt_time_t *timer)
{
    if (!timer) 
    {
        return;
    }

    timer->time = 0;
}

void mqtt_utils_time_countdown_ms(mqtt_time_t *timer, uint32 millisecond)
{
    if (!timer) 
    {
        return;
    }

    timer->time = mqtt_utils_UptimeMs() + millisecond;
}

uint32 mqtt_utils_time_get_ms(void)
{
    return mqtt_utils_UptimeMs();
}

int mqtt_utils_SetProductKey(char *product_key)
{
    int len = strlen(product_key);
    
    if (len > IOTX_PRODUCT_KEY_LEN) 
    {
        return -1;
    }
    memset(_product_key, 0x00, IOTX_PRODUCT_KEY_LEN + 1);
    memcpy(_product_key, product_key, len);
    
    return len;
}

int mqtt_utils_SetDeviceName(char *device_name)
{
    int len = strlen(device_name);
    
    if (len > IOTX_DEVICE_NAME_LEN) 
    {
        return -1;
    }
    memset(_device_name, 0x00, IOTX_DEVICE_NAME_LEN + 1);
    memcpy(_device_name, device_name, len);
    
    return len;
}

int mqtt_utils_SetDeviceSecret(char *device_secret)
{
    int len = strlen(device_secret);
    
    if (len > IOTX_DEVICE_SECRET_LEN) 
    {
        return -1;
    }
    memset(_device_secret, 0x00, IOTX_DEVICE_SECRET_LEN + 1);
    memcpy(_device_secret, device_secret, len);

    return len;
}

int mqtt_utils_SetHostName(char *host_name)
{
    int len = strlen(host_name);
    
    if (len > DEV_SIGN_HOSTNAME_MAXLEN) 
    {
        return -1;
    }
    memset(_host_name, 0x00, DEV_SIGN_HOSTNAME_MAXLEN + 1);
    memcpy(_host_name, host_name, len);
    
    return len;
}

int mqtt_utils_SetUserName(char *user_name)
{
    int len = strlen(user_name);
    
    if (len > DEV_SIGN_USERNAME_MAXLEN) 
    {
        return -1;
    }
    memset(_user_name, 0x00, DEV_SIGN_USERNAME_MAXLEN + 1);
    memcpy(_user_name, user_name, len);
    
    return len;
}

int mqtt_utils_SetPassword(char *password)
{
    int len = strlen(password);
    
    if (len > DEV_SIGN_PASSWORD_MAXLEN) 
    {
        return -1;
    }
    memset(_password, 0x00, DEV_SIGN_PASSWORD_MAXLEN + 1);
    memcpy(_password, password, len);
    
    return len;
}

int mqtt_utils_SetClientID(char *clientID)
{
    int len = strlen(clientID);
    
    if (len > DEV_SIGN_CLIENT_ID_MAXLEN) 
    {
        return -1;
    }
    memset(_client_id, 0x00, DEV_SIGN_CLIENT_ID_MAXLEN + 1);
    memcpy(_client_id, clientID, len);
    
    return len;
}

void mqtt_utils_SetPort(uint16 port )
{
    mqtt_port = port;
}

int mqtt_utils_SetProductSecret(char *product_secret)
{
    int len = strlen(product_secret);
    
    if (len > IOTX_PRODUCT_SECRET_LEN) 
    {
        return -1;
    }
    memset(_product_secret, 0x00, IOTX_PRODUCT_SECRET_LEN + 1);
    memcpy(_product_secret, product_secret, len);
    
    return len;
}

int mqtt_utils_GetProductKey(char *product_key)
{
    int len = strlen(_product_key);
    
    if (len > IOTX_PRODUCT_KEY_LEN) 
    {
        return -1;
    }    
    memset(product_key, 0x00, IOTX_PRODUCT_KEY_LEN + 1);
    memcpy(product_key, _product_key, len);

    return len;
}

int mqtt_utils_GetProductSecret(char *product_secret)
{
    int len = strlen(_product_secret);
    
    if (len > IOTX_PRODUCT_SECRET_LEN) 
    {
        return -1;
    }    
    memset(product_secret, 0x00, IOTX_PRODUCT_SECRET_LEN + 1);
    memcpy(product_secret, _product_secret, len);

    return len;
}

int mqtt_utils_GetDeviceName(char *device_name)
{
    int len = strlen(_device_name);
    
    if (len > IOTX_DEVICE_NAME_LEN) 
    {
        return -1;
    }    
    memset(device_name, 0x00, IOTX_DEVICE_NAME_LEN + 1);
    memcpy(device_name, _device_name, len);

    return strlen(device_name);
}

int mqtt_utils_GetDeviceSecret(char *device_secret)
{
    int len = strlen(_device_secret);
    
    if (len > IOTX_DEVICE_SECRET_LEN) 
    {
        return -1;
    }    
    
    memset(device_secret, 0x00, IOTX_DEVICE_SECRET_LEN + 1);
    memcpy(device_secret, _device_secret, len);

    return len;
}

int mqtt_utils_GetHostName(char *host_name)
{
    int len = strlen(_host_name);
    
    if (len > DEV_SIGN_HOSTNAME_MAXLEN) 
    {
        return -1;
    }     
    memset(host_name, 0x00, DEV_SIGN_HOSTNAME_MAXLEN + 1);
    memcpy(host_name, _host_name, len);
    
    return strlen(host_name);
}

int mqtt_utils_GetUserName(char *user_name)
{
    int len = strlen(_user_name);
    
    if (len > DEV_SIGN_USERNAME_MAXLEN) 
    {
        return -1;
    }     
    memset(user_name, 0x00, DEV_SIGN_USERNAME_MAXLEN + 1);
    memcpy(user_name, _user_name, len);
    
    return strlen(user_name);
}

int mqtt_utils_GetPassword(char *password)
{
    int len = strlen(_password);
    
    if (len > DEV_SIGN_PASSWORD_MAXLEN) 
    {
        return -1;
    }     
    memset(password, 0x00, DEV_SIGN_PASSWORD_MAXLEN + 1);
    memcpy(password, _password, len);
    
    return strlen(password);
}

int mqtt_utils_GetClientID(char *clientID)
{
    int len = strlen(_client_id);
    
    if (len > DEV_SIGN_CLIENT_ID_MAXLEN) 
    {
        return -1;
    }
    memset(clientID, 0x00, DEV_SIGN_CLIENT_ID_MAXLEN + 1);
    memcpy(clientID, _client_id, len);
    
    return len;
}

uint16 mqtt_utils_GetPort(void)
{
    return mqtt_port;
}

