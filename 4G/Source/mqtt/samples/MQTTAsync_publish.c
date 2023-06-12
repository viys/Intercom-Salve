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

//#define ADDRESS     "tcp://mqtt.eclipse.org:1883"
#define ADDRESS     "tcp://137.135.83.217:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "MQTT Examples"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

volatile MQTTAsync_token deliveredtoken;

int finished = 0;

void connlost(void *context, char *cause)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;

    LogPrintf("\r\nConnection lost\r\n");
    LogPrintf("     cause: %s\r\n", cause);

    LogPrintf("Reconnecting\r\n");
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        LogPrintf("Failed to start connect, return code %d\r\n", rc);
        finished = 1;
    }
}


void onDisconnect(void* context, MQTTAsync_successData* response)
{
    LogPrintf("Successful disconnection\r\n");
    finished = 1;
}


void onSend(void* context, MQTTAsync_successData* response)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
    int rc;

    LogPrintf("Message with token value %d delivery confirmed\r\n", response->token);

    opts.onSuccess = onDisconnect;
    opts.context = client;

    if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
    {
        LogPrintf("Failed to start sendMessage, return code %d\r\n", rc);
        return rc;
    }
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
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    int rc;

    LogPrintf("Successful connection\r\n");

    opts.onSuccess = onSend;
    opts.context = client;

    pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen = (int)strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;

    if ((rc = MQTTAsync_sendMessage(client, TOPIC, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
    {
        LogPrintf("Failed to start sendMessage, return code %d\r\n", rc);
        return;
    }
}


//int main(int argc, char* argv[])
int mqtt_async_publish_test(void)
{
    MQTTAsync client;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;

    MQTTAsync_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    MQTTAsync_setCallbacks(client, NULL, connlost, NULL, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;

    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        LogPrintf("Failed to start connect, return code %d\r\n", rc);
        return rc;
    }

    LogPrintf("Waiting for publication of %s\r\n"
         "on topic %s for client with ClientID: %s\r\n",
         PAYLOAD, TOPIC, CLIENTID);

    while (!finished)
    {
        KING_Sleep(100);
    }

    MQTTAsync_destroy(&client);
     return rc;
}

