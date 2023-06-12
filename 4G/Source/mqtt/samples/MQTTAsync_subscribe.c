/*******************************************************************************
 * Copyright (c) 2012, 2018 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include "KingDef.h"
#include "MQTTAsync.h"

//#define ADDRESS     "tcp://localhost:1883"
//#define ADDRESS     "tcp://mqtt.eclipse.org:1883"
//#define ADDRESS     "tcp://110.80.1.51:1883"
#define ADDRESS     "tcp://39.106.133.14:8599"
#define CLIENTID    "ExampleClientSub"
#define TOPIC       "MQTT Examples"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

volatile MQTTAsync_token deliveredtoken;

int disc_finished = 0;
int subscribed = 0;
int finished = 0;

void connlost(void *context, char *cause)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;

    LogPrintf("\r\nConnection lost\r\n");
    if (cause)
    {
        LogPrintf("     cause: %s\r\n", cause);
    }

    LogPrintf("Reconnecting\r\n");
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        LogPrintf("Failed to start connect, return code %d\n", rc);
        finished = 1;
    }
}


int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    int i;
    char* payloadptr;

    LogPrintf("Message arrived\r\n");
    LogPrintf("     topic: %s\r\n", topicName);
    LogPrintf("   message: ");

    payloadptr = message->payload;
    for(i=0; i<message->payloadlen; i++)
    {
        LogPrintf("%c", *payloadptr++);
    }
    LogPrintf("\r\n");
    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}


void onDisconnect(void* context, MQTTAsync_successData* response)
{
    LogPrintf("Successful disconnection\r\n");
    disc_finished = 1;
}


void onSubscribe(void* context, MQTTAsync_successData* response)
{
    LogPrintf("Subscribe succeeded\r\n");
    subscribed = 1;
}

void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
    LogPrintf("Subscribe failed, rc %d\r\n", response ? response->code : 0);
    finished = 1;
}


void onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    LogPrintf("Connect failed, rc %d\r\n", response ? response->code : 0);
    finished = 1;
}


void onConnect(void* context, MQTTAsync_successData* response)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    int rc;

    LogPrintf("Successful connection\r\n");

    LogPrintf("Subscribing to topic %s\r\nfor client %s using QoS%d\r\n\r\n"
           "Press Q<Enter> to quit\r\n\r\n", TOPIC, CLIENTID, QOS);
    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = client;

    deliveredtoken = 0;

    if ((rc = MQTTAsync_subscribe(client, TOPIC, QOS, &opts)) != MQTTASYNC_SUCCESS)
    {
        LogPrintf("Failed to start subscribe, return code %d\r\n", rc);
        //KING_ThreadExit();
        return;
    }
}


//int main(int argc, char* argv[])
int mqtt_async_subscribe_test(void)
{
    MQTTAsync client;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
    int rc;
    int ch;

    MQTTAsync_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    MQTTAsync_setCallbacks(client, client, connlost, msgarrvd, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        LogPrintf("Failed to start connect, return code %d\r\n", rc);
        //KING_ThreadExit();
        return rc;
    }

    while (!subscribed)
    {
        KING_Sleep(100);
    }

    if (finished)
        goto exit;

    disc_opts.onSuccess = onDisconnect;
    if ((rc = MQTTAsync_disconnect(client, &disc_opts)) != MQTTASYNC_SUCCESS)
    {
        LogPrintf("Failed to start disconnect, return code %d\r\n", rc);
        //KING_ThreadExit();
        return rc;
    }
    while (!disc_finished)
    {
        KING_Sleep(100);
    }

exit:
    MQTTAsync_destroy(&client);
    return rc;
}
