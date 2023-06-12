/***********************************************************************
# Xiamen CHEERZING IOT Tech KingSDK Build System
# All Rights Reserved 2019
# Author: Nestor Zhang
#######################################################################*/

#include "lwm2mclient.h"

#define MAX_PACKET_SIZE 1024
#define DEFAULT_SERVER_IPV6 "[::1]"
#define DEFAULT_SERVER_IPV4 "127.0.0.1"

int g_reboot = 0;
client_data_t clientData;
char myIMEI[20] = { 0 };

#define OBJ_COUNT 9
lwm2m_object_t * objArray[OBJ_COUNT];

// only backup security and server objects
# define BACKUP_OBJECT_COUNT 2
lwm2m_object_t * backupObjectArray[BACKUP_OBJECT_COUNT];


void handle_value_changed(lwm2m_context_t * lwm2mH, lwm2m_uri_t * uri, const char * value, size_t valueLength, lwm2m_data_type_t type)
{
    lwm2m_object_t * object = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList, uri->objectId);

    if (NULL != object)
    {
        if (object->writeFunc != NULL)
        {
            lwm2m_data_t * dataP;
            int result;

            dataP = lwm2m_data_new(1);
            if (dataP == NULL)
            {
                LogPrintf("Internal allocation failure !\n");
                return;
            }

            dataP->id = uri->resourceId;
            dataP->type = type;

            if (dataP->type == LWM2M_TYPE_STRING)
                lwm2m_data_encode_nstring(value, valueLength, dataP);
            else if (dataP->type == LWM2M_TYPE_OPAQUE)
                lwm2m_data_encode_opaque(value, valueLength, dataP);
            else if (dataP->type == LWM2M_TYPE_INTEGER)
                lwm2m_data_encode_int(KING_atoi(value), dataP);
            else if (dataP->type == LWM2M_TYPE_FLOAT) {
                double vd;
                utils_textToFloat(value, valueLength, &vd);
                lwm2m_data_encode_float(vd, dataP);
            }
            else if (dataP->type == LWM2M_TYPE_BOOLEAN)
                lwm2m_data_encode_bool(KING_atoi(value), dataP);

            // TODO
            result = object->writeFunc(uri->instanceId, 1, dataP, object);

            if (COAP_405_METHOD_NOT_ALLOWED == result)
            {
                switch (uri->objectId)
                {
                case LWM2M_DEVICE_OBJECT_ID:
                    result = device_change(dataP, object);
                    break;
                default:
                    break;
                }
            }

            if (COAP_204_CHANGED != result)
            {
                LogPrintf("Failed to change value!\n");
            }
            else
            {
                LogPrintf("value changed!\n");
                lwm2m_resource_value_changed(lwm2mH, uri);
            }
            lwm2m_data_free(1, dataP);
            return;
        }
        else
        {
            LogPrintf("write not supported for specified resource!\n");
        }
        return;
    }
    else
    {
        LogPrintf("Object not found !\n");
    }
}

#if (defined WITH_TINYDTLS) || (defined WITH_MBEDTLS)
void * lwm2m_connect_server(uint16 secObjInstID,
                            void * userData)
{
  client_data_t * dataP;
  lwm2m_list_t * instance;
  dtls_connection_t * newConnP = NULL;
  dataP = (client_data_t *)userData;
  lwm2m_object_t  * securityObj = dataP->securityObjP;

  instance = LWM2M_LIST_FIND(dataP->securityObjP->instanceList, secObjInstID);
  if (instance == NULL) return NULL;


  //newConnP = connection_create(dataP->connList, dataP->sock, securityObj, instance->id, dataP->lwm2mH, dataP->addressFamily);
  if (newConnP == NULL)
  {
      LogPrintf("Connection creation failed.\n");
      return NULL;
  }

  //dataP->connList = newConnP;
  return (void *)newConnP;
}
#else
void * lwm2m_connect_server(uint16 secObjInstID,
                            void * userData)
{
    client_data_t * dataP;
    char * uri;
    char * host;
    char * port;
    dtls_connection_t * newConnP = NULL;

    dataP = (client_data_t *)userData;

    uri = get_server_uri(dataP->securityObjP, secObjInstID);

    if (uri == NULL) return NULL;

    // parse uri in the form "coaps://[host]:[port]"
    if (0==strncmp(uri, "coaps://", KING_strlen("coaps://"))) {
        host = uri+KING_strlen("coaps://");
    }
    else if (0==strncmp(uri, "coap://",  KING_strlen("coap://"))) {
        host = uri+KING_strlen("coap://");
    }
    else {
        goto exit;
    }
    port = KING_strrchr(host, ':');
    if (port == NULL) goto exit;
    // remove brackets
    if (host[0] == '[')
    {
        host++;
        if (*(port - 1) == ']')
        {
            *(port - 1) = 0;
        }
        else goto exit;
    }
    // split strings
    *port = 0;
    port++;

    LogPrintf("Opening connection to server at %s:%s\r\n", host, port);
    // NOTE: the dataP->sock is already created when bind device.
    newConnP = connection_create(dataP->connList, dataP->sock, host, port, dataP->addressFamily);
    if (newConnP == NULL) {
        LogPrintf("Connection creation failed.\r\n");
    }
    else {
        newConnP->lwm2mH = dataP->lwm2mH;
        dataP->connList = newConnP;
    }

exit:
    lwm2m_free(uri);
    return (void *)newConnP;
}
#endif

void lwm2m_close_connection(void * sessionH,
                            void * userData)
{
#if 0
    client_data_t * app_data;
#if (defined WITH_TINYDTLS) || (defined WITH_MBEDTLS)
    dtls_connection_t * targetP;
#else
    connection_t * targetP;
#endif

    app_data = (client_data_t *)userData;
#if (defined WITH_TINYDTLS) || (defined WITH_MBEDTLS)
    targetP = (dtls_connection_t *)sessionH;
#else
    targetP = (connection_t *)sessionH;
#endif

    if (targetP == app_data->connList)
    {
        app_data->connList = targetP->next;
        lwm2m_free(targetP);
    }
    else
    {
#if (defined WITH_TINYDTLS) || (defined WITH_MBEDTLS)
        dtls_connection_t * parentP;
#else
        connection_t * parentP;
#endif

        parentP = app_data->connList;
        while (parentP != NULL && parentP->next != targetP)
        {
            parentP = parentP->next;
        }
        if (parentP != NULL)
        {
            parentP->next = targetP->next;
            lwm2m_free(targetP);
        }
    }
#endif
}

static void prv_output_servers(char * buffer,
                               void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_server_t * targetP;

    targetP = lwm2mH->bootstrapServerList;

    if (lwm2mH->bootstrapServerList == NULL)
    {
        LogPrintf("No Bootstrap Server.\r\n");
    }
    else
    {
        LogPrintf("Bootstrap Servers:\r\n");
        for (targetP = lwm2mH->bootstrapServerList ; targetP != NULL ; targetP = targetP->next)
        {
            LogPrintf("objid[%d] lifetime[%d] \tstatus: \r\n",
                        (targetP->secObjInstID),
                        (unsigned long)targetP->lifetime);
            switch(targetP->status)
            {
            case STATE_DEREGISTERED:
                LogPrintf("DEREGISTERED\r\n");
                break;
            case STATE_BS_HOLD_OFF:
                LogPrintf("CLIENT HOLD OFF\r\n");
                break;
            case STATE_BS_INITIATED:
                LogPrintf("BOOTSTRAP INITIATED\r\n");
                break;
            case STATE_BS_PENDING:
                LogPrintf("BOOTSTRAP PENDING\r\n");
                break;
            case STATE_BS_FINISHED:
                LogPrintf("BOOTSTRAP FINISHED\r\n");
                break;
            case STATE_BS_FAILED:
                LogPrintf("BOOTSTRAP FAILED\r\n");
                break;
            default:
                LogPrintf((int)targetP->status);
            }
        }
    }

    if (lwm2mH->serverList == NULL)
    {
        LogPrintf("No LWM2M Server.\r\n");
    }
    else
    {
        LogPrintf("LWM2M Servers:\r\n");
        for (targetP = lwm2mH->serverList ; targetP != NULL ; targetP = targetP->next)
        {
            LogPrintf("shortID[%D] \tstatus: ", targetP->shortID);
            switch(targetP->status)
            {
            case STATE_DEREGISTERED:
                LogPrintf("DEREGISTERED\r\n");
                break;
            case STATE_REG_PENDING:
                LogPrintf("REGISTRATION PENDING\r\n");
                break;
            case STATE_REGISTERED:
                LogPrintf((unsigned long)targetP->lifetime);
                break;
            case STATE_REG_UPDATE_PENDING:
                LogPrintf("REGISTRATION UPDATE PENDING\r\n");
                break;
            case STATE_DEREG_PENDING:
                LogPrintf("DEREGISTRATION PENDING\r\n");
                break;
            case STATE_REG_FAILED:
                LogPrintf("REGISTRATION FAILED\r\n");
                break;
            default:
                LogPrintf((int)targetP->status);
            }
        }
    }
}


static void ocean_server_send_data(lwm2m_context_t *lwm2mH, char *buffer)
{
    lwm2m_uri_t uri;
    int result;

    result = lwm2m_stringToUri("/19/0/0", KING_strlen("/19/0/0"), &uri);
    if (result == 0) goto syntax_error;

    if (buffer == NULL || buffer[0] == 0)
    {
        LogPrintf("report change!\n");
        lwm2m_resource_value_changed(lwm2mH, &uri);
    }
    else
    {
        handle_value_changed(lwm2mH, &uri, buffer, KING_strlen(buffer), LWM2M_TYPE_OPAQUE);
    }

    ocean_server_lwm2m_step();

    return;

syntax_error:
    LogPrintf("Syntax error !\n");
}

char *testData = "7B226D736754797065223A22636C6F7564526571222C22736572766963654964223A224E42536D6F6B65416C61726D53797374656D222C227061726173223A7B227374616E64627932223A352C22696D6569223A22383639303837303330303035393736222C227374616E64627931223A31302C2276616C7565223A357D2C22636D64223A225345545F5452414E534D495353494F4E5F46524551222C226861734D6F7265223A302C226D6964223A347D";

void ocean_server_send_data_test(void)
{
    char *dataBuff = NULL;
    int len = KING_strlen(testData);

    KING_MemAlloc(&dataBuff, len + 1);
    KING_memcpy(dataBuff, testData, len + 1);
    paramHexDataParse(dataBuff, len);
    dataBuff[len/2] = '\0'; // terminator

    ocean_server_send_data(clientData.lwm2mH, dataBuff);

    KING_MemFree(dataBuff);
    dataBuff = NULL;
}

static void prv_object_list(char * buffer,
                            void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_object_t * objectP;

    for (objectP = lwm2mH->objectList; objectP != NULL; objectP = objectP->next)
    {
        if (objectP->instanceList == NULL)
        {
            LogPrintf("%d", objectP->objID);
        }
        else
        {
            lwm2m_list_t * instanceP;

            for (instanceP = objectP->instanceList; instanceP != NULL ; instanceP = instanceP->next)
            {
                LogPrintf("%d", instanceP->id);
            }
        }
        LogPrintf("\r\n");
    }
}

static void prv_instance_dump(lwm2m_object_t * objectP,
                              uint16 id)
{
    int numData;
    lwm2m_data_t * dataArray;
    uint16 res;

    numData = 0;
    res = objectP->readFunc(id, &numData, &dataArray, objectP);
    if (res != COAP_205_CONTENT)
    {
        LogPrintf("Error: %d\r\n", res);
        return;
    }

    //dump_tlv(stdout, numData, dataArray, 0);
}


static void prv_object_dump(char * buffer,
                            void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_uri_t uri;
    char * end = NULL;
    int result;
    lwm2m_object_t * objectP;

    end = get_end_of_arg(buffer);
    if (end[0] == 0) goto syntax_error;

    result = lwm2m_stringToUri(buffer, end - buffer, &uri);
    if (result == 0) goto syntax_error;
    if (LWM2M_URI_IS_SET_RESOURCE(&uri)) goto syntax_error;

    objectP = (lwm2m_object_t *)LWM2M_LIST_FIND(lwm2mH->objectList, uri.objectId);
    if (objectP == NULL)
    {
        LogPrintf("Object not found.\n");
        return;
    }

    if (LWM2M_URI_IS_SET_INSTANCE(&uri))
    {
        prv_instance_dump(objectP, uri.instanceId);
    }
    else
    {
        lwm2m_list_t * instanceP;

        for (instanceP = objectP->instanceList; instanceP != NULL ; instanceP = instanceP->next)
        {
            LogPrintf("instance ID [%d] ", instanceP->id);
            prv_instance_dump(objectP, instanceP->id);
            LogPrintf("\r\n");
        }
    }

    return;

syntax_error:
    LogPrintf("Syntax error !\n");
}

static void prv_update(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    if (buffer[0] == 0) goto syntax_error;

    uint16 serverId = (uint16) KING_atoi(buffer);
    int res = lwm2m_update_registration(lwm2mH, serverId, false);
    if (res != 0)
    {
        LogPrintf("Registration update error: %d\r\n", res);
    }
    return;

syntax_error:
    LogPrintf("Syntax error !\n");
}

static void update_battery_level(lwm2m_context_t * context)
{
    static time_t next_change_time = 0;
    time_t tv_sec;

    tv_sec = lwm2m_gettime();
    if (tv_sec < 0) return;

    if (next_change_time < tv_sec)
    {
        char value[15];
        int valueLength;
        lwm2m_uri_t uri;
        int level = rand() % 100;

        if (0 > level) level = -level;
        if (lwm2m_stringToUri("/3/0/9", 6, &uri))
        {
            valueLength = KING_sprintf(value, "%d", level);
            handle_value_changed(context, &uri, value, valueLength, LWM2M_TYPE_INTEGER);
        }
        level = rand() % 20;
        if (0 > level) level = -level;
        next_change_time = tv_sec + level + 10;
    }
}

bool add_mult_obj_insts(lwm2m_context_t *lwm2mH, uint16 objid, uint32 instCount)
{
    lwm2m_object_t *objectP;
    int res;

    objectP = get_test_object(objid, instCount);
    if (objectP == NULL)
    {
        LogPrintf("Fail to get test obj[%d]", objid);
        return false;
    }

    res = lwm2m_add_object(lwm2mH, objectP);
    if (res != 0)
    {
        LogPrintf("objid[%d] status[%d]\r\n", objid, res);
        return false;
    }
    else
    {
        LogPrintf("objid inst Count[%d]", instCount);
    }
    return true;
}

#ifdef LWM2M_BOOTSTRAP

static void prv_initiate_bootstrap(char * buffer,
                                   void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_server_t * targetP;

    // HACK !!!
    lwm2mH->state = STATE_BOOTSTRAP_REQUIRED;
    targetP = lwm2mH->bootstrapServerList;
    while (targetP != NULL)
    {
        targetP->lifetime = 0;
        targetP = targetP->next;
    }
}

static void prv_display_objects(char * buffer,
                                void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    lwm2m_object_t * object;

    for (object = lwm2mH->objectList; object != NULL; object = object->next){
        if (NULL != object) {
            switch (object->objID)
            {
            case LWM2M_SECURITY_OBJECT_ID:
                display_security_object(object);
                break;
            case LWM2M_SERVER_OBJECT_ID:
                display_server_object(object);
                break;
            case LWM2M_ACL_OBJECT_ID:
                break;
            case LWM2M_DEVICE_OBJECT_ID:
                display_device_object(object);
                break;
            case LWM2M_CONN_MONITOR_OBJECT_ID:
                break;
            case LWM2M_FIRMWARE_UPDATE_OBJECT_ID:
                display_firmware_object(object);
                break;
            case LWM2M_LOCATION_OBJECT_ID:
                display_location_object(object);
                break;
            case LWM2M_CONN_STATS_OBJECT_ID:
                break;
				/*
            case TEST_OBJECT_ID:
                display_test_object(object);
                */
                break;
            }
        }
    }
}

static void prv_display_backup(char * buffer,
        void * user_data)
{
   int i;
   for (i = 0 ; i < BACKUP_OBJECT_COUNT ; i++) {
       lwm2m_object_t * object = backupObjectArray[i];
       if (NULL != object) {
           switch (object->objID)
           {
           case LWM2M_SECURITY_OBJECT_ID:
               display_security_object(object);
               break;
           case LWM2M_SERVER_OBJECT_ID:
               display_server_object(object);
               break;
           default:
               break;
           }
       }
   }
}

static void prv_backup_objects(lwm2m_context_t * context)
{
    uint16 i;

    for (i = 0; i < BACKUP_OBJECT_COUNT; i++) {
        if (NULL != backupObjectArray[i]) {
            switch (backupObjectArray[i]->objID)
            {
            case LWM2M_SECURITY_OBJECT_ID:
                clean_security_object(backupObjectArray[i]);
                lwm2m_free(backupObjectArray[i]);
                break;
            case LWM2M_SERVER_OBJECT_ID:
                clean_server_object(backupObjectArray[i]);
                lwm2m_free(backupObjectArray[i]);
                break;
            default:
                break;
            }
        }
        backupObjectArray[i] = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
        memset(backupObjectArray[i], 0, sizeof(lwm2m_object_t));
    }

    /*
     * Backup content of objects 0 (security) and 1 (server)
     */
    copy_security_object(backupObjectArray[0], (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SECURITY_OBJECT_ID));
    copy_server_object(backupObjectArray[1], (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SERVER_OBJECT_ID));
}

static void prv_restore_objects(lwm2m_context_t * context)
{
    lwm2m_object_t * targetP;

    /*
     * Restore content  of objects 0 (security) and 1 (server)
     */
    targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SECURITY_OBJECT_ID);
    // first delete internal content
    clean_security_object(targetP);
    // then restore previous object
    copy_security_object(targetP, backupObjectArray[0]);

    targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_SERVER_OBJECT_ID);
    // first delete internal content
    clean_server_object(targetP);
    // then restore previous object
    copy_server_object(targetP, backupObjectArray[1]);

    // restart the old servers
    LogPrintf("[BOOTSTRAP] ObjectList restored\r\n");
}

static void update_bootstrap_info(lwm2m_client_state_t * previousBootstrapState,
        lwm2m_context_t * context)
{
    if (*previousBootstrapState != context->state)
    {
        *previousBootstrapState = context->state;
        switch(context->state)
        {
            case STATE_BOOTSTRAPPING:
                LogPrintf("[BOOTSTRAP] backup security and server objects\r\n");
                prv_backup_objects(context);
                break;
            default:
                break;
        }
    }
}

static void close_backup_object()
{
    int i;
    for (i = 0; i < BACKUP_OBJECT_COUNT; i++) {
        if (NULL != backupObjectArray[i]) {
            switch (backupObjectArray[i]->objID)
            {
            case LWM2M_SECURITY_OBJECT_ID:
                clean_security_object(backupObjectArray[i]);
                lwm2m_free(backupObjectArray[i]);
                break;
            case LWM2M_SERVER_OBJECT_ID:
                clean_server_object(backupObjectArray[i]);
                lwm2m_free(backupObjectArray[i]);
                break;
            default:
                break;
            }
        }
    }
}
#endif //LWM2M_BOOTSTRAP

void SocketEventProcess(uint32 eventID, void* pdata, uint32 len)
{
    uint16 uDataSize = 0;
    LogPrintf("SocketEventProcess eventId=%d",eventID);

    switch (eventID)
    {
    case SOCKET_CONNECT_EVENT_RSP:
        LogPrintf("SOCKET_CONNECT_EVENT_RSP");
        break;

    case SOCKET_ACCEPT_EVENT_IND:
        LogPrintf("EV_CFW_TCPIP_ACCEPT_IND");
        break;

    case SOCKET_FULL_CLOSED_EVENT_IND:
        LogPrintf("SOCKET_FULL_CLOSED_EVENT_IND");
        break;

    case SOCKET_ERROR_EVENT_IND:
        LogPrintf("EV_CFW_TCPIP_ERR_IND");
        KING_SocketClose(clientData.sock);
        clientData.sock = -1;
        break;

    case SOCKET_WRITE_EVENT_IND:
        LogPrintf("SOCKET_WRITE_EVENT_IND");

        LogPrintf("ack by peer uSendDataSize=%d", len);
        break;

    case SOCKET_READ_EVENT_IND:
        LogPrintf("SOCKET_READ_EVENT_IND");
        uDataSize = len;
        LogPrintf("receive datalen=%d\r\n", uDataSize);

        ocean_server_recv_data(uDataSize);

        break;

    default:
        LogPrintf("TCPIP unexpect event/response %d",eventID);
        break;
    }

}

/*
 * This function does two things:
 *  - first it does the work needed by liblwm2m (eg. (re)sending some packets).
 *  - Secondly it adjusts the timeout value (default 60s) depending on the state of the transaction
 *    (eg. retransmission) and the time between the next operation
 */
int ocean_server_lwm2m_step()
{
    int result = -1;
    time_t tv_sec = 60;

    result = lwm2m_step(clientData.lwm2mH, &(tv_sec));
    LogPrintf(" -> State: ");
    switch (clientData.lwm2mH->state)
    {
    case STATE_INITIAL:
        LogPrintf("STATE_INITIAL\r\n");
        break;
    case STATE_BOOTSTRAP_REQUIRED:
        LogPrintf("STATE_BOOTSTRAP_REQUIRED\r\n");
        break;
    case STATE_BOOTSTRAPPING:
        LogPrintf("STATE_BOOTSTRAPPING\r\n");
        break;
    case STATE_REGISTER_REQUIRED:
        LogPrintf("STATE_REGISTER_REQUIRED\r\n");
        break;
    case STATE_REGISTERING:
        LogPrintf("STATE_REGISTERING\r\n");
        break;
    case STATE_READY:
        LogPrintf("STATE_READY\r\n");
        break;
    default:
        LogPrintf("Unknown...\r\n");
        break;
    }

    return result;
}


int ocean_server_recv_data(size_t len)
{
    int ret = -1;
    uint8 buffer[MAX_PACKET_SIZE];
    struct sockaddr addr;
    socklen_t addrLen;

    addrLen = sizeof(struct sockaddr);

    /*
     * We retrieve the clientData received
     */
    ret = KING_SocketRecvfrom(clientData.sock, buffer, len, 0, &addr, &addrLen);

    if (ret < 0)
    {
        LogPrintf("Error in recvfrom()\r\n");
    }
    else //if (numBytes >= 0)
    {
        int i = 0;
        dtls_connection_t * connP;

        //dump data
        LogPrintf("receive Data: ");
        for (i= 0; i < len; i++)
        {
            LogPrintf("0x%X ", buffer[i]);
        }
        LogPrintf("\r\n");
#if 0
        {
        char s[46];
        uint16 port;

        if (AF_INET == addr.ss_family)
        {
            struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
            inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET6_ADDRSTRLEN);
            port = saddr->sin_port;
        }
        }
#endif
        connP = connection_find(clientData.connList, &addr, addrLen);
        if (connP != NULL)
        {
            /*
             * Let liblwm2m respond to the query depending on the context
             */
            LogPrintf("connection port: %d\r\n", KING_ntohs(connP->addr.port));
#ifdef WITH_TINYDTLS
            int result = connection_handle_packet(connP, buffer, len);
            if (0 != result)
            {
                 LogPrintf("error handling message %d\n",result);
            }
#else
            lwm2m_handle_packet(clientData.lwm2mH, buffer, len, connP);
#endif
            conn_s_updateRxStatistic(clientData.lwm2mH, len, false);

            ocean_server_lwm2m_step();
        }
        else
        {
            LogPrintf("received bytes ignored!\r\n");
        }
    }
}


int ocean_server_bind_device(void)
{
    int result;
    lwm2m_context_t *lwm2mH = NULL;
    const char *localPort = "56830";
    const char *server = NULL;
    const char *serverPort = LWM2M_STANDARD_PORT_STR;
    char *name = NULL;
    int lifetime = 60;
    bool bootstrapRequested = false;
    char serverUri[50];
    int serverId = 123;

#ifdef LWM2M_BOOTSTRAP
    lwm2m_client_state_t previousState = STATE_INITIAL;
#endif

#if 1//def WITH_TINYDTLS
    char *pskId = NULL;
    char *psk = NULL;
    uint16 pskLen = -1;
    char *pskBuffer = NULL;
#endif

    memset(&clientData, 0, sizeof(client_data_t));

    //KING_sprintf(cmdline,"-n %s -h %s -p %d -4 -t 60 -i %s -s %s -u %d",imei,ipaddress,uPort,imei,psk,gATCurrentRAIMode)
    //KING_sprintf(cmdline,"-n %s -h %s -p %d -4 -t 60 -u %d",imei,ipaddress,uPort,gATCurrentRAIMode);
    get_system_imei(myIMEI);
    name = myIMEI; // Test IMEI
    server = "180.101.147.115"; // Ocean IOT server.
    serverPort = "5683";
    clientData.addressFamily = AF_INET;
#if (defined WITH_TINYDTLS) || (defined WITH_MBEDTLS)
     //pskId = "869087030005976";
     //psk = "";
#endif

    if (!server)
    {
        server = (AF_INET == clientData.addressFamily ? DEFAULT_SERVER_IPV4 : DEFAULT_SERVER_IPV6);
    }

    /*
     *This call an internal function that create an IPV6 socket on the port 5683.
     */
    LogPrintf("[%s]Trying to bind LWM2M Client to port %s\r\n", name, localPort);
    clientData.sock = create_socket(localPort, clientData.addressFamily);

    if (clientData.sock < 0)
    {
        LogPrintf("failed to create socket!\r\n");
        return -1;
    }

    KING_RegNotifyFun(DEV_CLASS_SOCKET, clientData.sock, SocketEventProcess);

    /*
     * Now the main function fill an array with each object, this list will be later passed to liblwm2m.
     * Those functions are located in their respective object file.
     */
#if (defined WITH_TINYDTLS) || (defined WITH_MBEDTLS)
    if (psk != NULL)
    {
        pskLen = KING_strlen(psk) / 2;
        pskBuffer = malloc(pskLen);

        if (NULL == pskBuffer)
        {
            LogPrintf("Failed to create PSK binary buffer\r\n");
            return -1;
        }
        // Hex string to binary
        char *h = psk;
        char *b = pskBuffer;
        char xlate[] = "0123456789ABCDEF";

        for ( ; *h; h += 2, ++b)
        {
            char *l = KING_strchr(xlate, toupper(*h));
            char *r = KING_strchr(xlate, toupper(*(h+1)));

            if (!r || !l)
            {
                LogPrintf("Failed to parse Pre-Shared-Key HEXSTRING\r\n");
                return -1;
            }

            *b = ((l - xlate) << 4) + (r - xlate);
        }
    }
#endif

#if (defined WITH_TINYDTLS) || (defined WITH_MBEDTLS)
    KING_sprintf(serverUri, "coaps://%s:%s", server, serverPort);
#else
    KING_sprintf(serverUri, "coap://%s:%s", server, serverPort);
#endif

#ifdef LWM2M_BOOTSTRAP
    objArray[0] = get_security_object(serverId, serverUri, pskId, pskBuffer, pskLen, bootstrapRequested);
#else
    objArray[0] = get_security_object(serverId, serverUri, pskId, pskBuffer, pskLen, false);
#endif

    if (NULL == objArray[0])
    {
        LogPrintf("Failed to create security object\r\n");
        return -1;
    }
    clientData.securityObjP = objArray[0];

    objArray[1] = get_server_object(serverId, "U", lifetime, false);
    if (NULL == objArray[1])
    {
        LogPrintf("Failed to create server object\r\n");
        return -1;
    }

    objArray[2] = get_object_device();
    if (NULL == objArray[2])
    {
        LogPrintf("Failed to create Device object\r\n");
        return -1;
    }

    objArray[3] = get_object_firmware();
    if (NULL == objArray[3])
    {
        LogPrintf("Failed to create Firmware object\r\n");
        return -1;
    }

    objArray[4] = get_object_location();
    if (NULL == objArray[4])
    {
        LogPrintf("Failed to create location object\r\n");
        return -1;
    }

    /*
     * The liblwm2m library is now initialized with the functions that will be in
     * charge of communication
     */
    lwm2mH = lwm2m_init(&clientData);
    if (NULL == lwm2mH)
    {
        LogPrintf("lwm2m_init() failed\r\n");
        return -1;
    }

    clientData.lwm2mH = lwm2mH;

    /*
     * We configure the liblwm2m library with the name of the client - which shall be unique for each client -
     * the number of objects we will be passing through and the objects array
     */
    result = lwm2m_configure(lwm2mH, name, NULL, NULL, OBJ_COUNT, objArray);
    if (result != 0)
    {
        LogPrintf("lwm2m_configure returns %d", result);
        return -1;
    }

    /**
     * Initialize value changed callback.
     */
    init_value_change(lwm2mH);

    // special for ocean server connect.
    add_mult_obj_insts(lwm2mH, 19, 2);

    ocean_server_lwm2m_step();

    if (result != 0)
    {
        LogPrintf("lwm2m_step returns error: %d", result);
        if(previousState == STATE_BOOTSTRAPPING)
        {
            LogPrintf("[BOOTSTRAP] restore security and server objects\r\n");
            prv_restore_objects(lwm2mH);
            lwm2mH->state = STATE_INITIAL;
        }
        else return -1;
    }
#ifdef LWM2M_BOOTSTRAP
    update_bootstrap_info(&previousState, lwm2mH);
#endif

    return 0;
}
