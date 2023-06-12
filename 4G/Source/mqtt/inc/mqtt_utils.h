/*******************************************************************************
 ** File Name:   mqtt_utils.h
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
 ** 2020-06-04    Allance.Chen         Create.
 ******************************************************************************/
#ifndef __MQTT_UTILS_H__
#define __MQTT_UTILS_H__
 
/*******************************************************************************
 ** Header Files
 ******************************************************************************/
#include "KingDef.h"
#include "KingPlat.h"
#include "KingCSTD.h"


/*---------------------------------------------------------------------------
 ** Defines
---------------------------------------------------------------------------*/
#define mqtt_info(fmt, args...)              do{KING_SysLog("mqtt info: "fmt, ##args);KING_SysLog("\r\n");}while(0)

/*******************************************************************************
 ** Type Definitions
 ******************************************************************************/
typedef struct {
    uint32 time;
} mqtt_time_t;
        
/*******************************************************************************
 **  Function 
 ******************************************************************************/
void mqtt_utils_MutexCreate(void **mutex);
void mqtt_utils_MutexDestroy(void *mutex);
void mqtt_utils_MutexLock(void *mutex);
void mqtt_utils_MutexUnlock(void *mutex);

void *mqtt_utils_Malloc(uint32 size);
void *mqtt_utils_Realloc(void *ptr, uint32 size);
void mqtt_utils_Free(void *ptr);

uint32 mqtt_utils_Random(uint32 region);

uint64 mqtt_utils_UptimeMs(void);
void mqtt_utils_SleepMs(uint32 ms);

void mqtt_utils_time_start(mqtt_time_t *timer);
uint32 mqtt_utils_time_spend(mqtt_time_t *start);
uint32 mqtt_utils_time_left(mqtt_time_t *end);
uint32 mqtt_utils_time_is_expired(mqtt_time_t *timer);
void mqtt_utils_time_init(mqtt_time_t *timer);
void mqtt_utils_time_countdown_ms(mqtt_time_t *timer, uint32 millisecond);
uint32 mqtt_utils_time_get_ms(void);

int mqtt_utils_SetProductKey(char *product_key);
int mqtt_utils_SetDeviceName(char *device_name);
int mqtt_utils_SetDeviceSecret(char *device_secret);
int mqtt_utils_SetHostName(char *host_name);
void mqtt_utils_SetPort(uint16 port );
int mqtt_utils_SetProductSecret(char *product_secret);
int mqtt_utils_SetUserName(char *user_name);
int mqtt_utils_SetPassword(char *password);
int mqtt_utils_SetClientID(char *clientID);

int mqtt_utils_GetProductKey(char *product_key);
int mqtt_utils_GetProductSecret(char *product_secret);
int mqtt_utils_GetDeviceName(char *device_name);
int mqtt_utils_GetDeviceSecret(char *device_secret);
int mqtt_utils_GetHostName(char *host_name);
uint16 mqtt_utils_GetPort(void);
int mqtt_utils_GetUserName(char *user_name);
int mqtt_utils_GetPassword(char *password);
int mqtt_utils_GetClientID(char *clientID);

#endif
