/***********************************************************************
# Xiamen CHEERZING IOT Tech KingSDK Build System
# All Rights Reserved 2019
# Author: Nestor Zhang
#######################################################################*/

#include "lwm2mclient.h"
#include "kingrtos.h"
#include "KingCbData.h"
#include "king_lwm2m.h"

#define MAX_PACKET_SIZE 1024
#define DEFAULT_SERVER_IPV6 "[::1]"
#define DEFAULT_SERVER_IPV4 "127.0.0.1"

int g_reboot = 0;
client_data_t clientData;

#define OBJ_COUNT 6
lwm2m_object_t * objArray[OBJ_COUNT];

// only backup security and server objects
#define BACKUP_OBJECT_COUNT 2
lwm2m_object_t * backupObjectArray[BACKUP_OBJECT_COUNT];

extern int utils_textToFloat(const uint8_t * buffer,
                      int length,
                      double * dataP);

static int aepRecvData(size_t len);

void handle_value_changed(lwm2m_context_t * lwm2mH, lwm2m_uri_t * uri, const char * value, size_t valueLength, lwm2m_data_type_t type)
{
    KING_SysLog("handle_value_changed %s,%d\n",value,valueLength);
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
                KING_SysLog("Internal allocation failure !\n");
                return;
            }

            dataP->id = uri->resourceId;
            dataP->type = type;

            if (dataP->type == LWM2M_TYPE_STRING)
                lwm2m_data_encode_nstring(value, valueLength, dataP);
            else if (dataP->type == LWM2M_TYPE_OPAQUE)
                lwm2m_data_encode_opaque((uint8 *)value, valueLength, dataP);
            else if (dataP->type == LWM2M_TYPE_INTEGER)
                lwm2m_data_encode_int(atoi(value), dataP);
            else if (dataP->type == LWM2M_TYPE_FLOAT) {
                double vd;
                utils_textToFloat((uint8 *)value, valueLength, &vd);
                lwm2m_data_encode_float(vd, dataP);
            }
            else if (dataP->type == LWM2M_TYPE_BOOLEAN)
                lwm2m_data_encode_bool(atoi(value), dataP);

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
                KING_SysLog("Failed to change value!\n");
            }
            else
            {
                KING_SysLog("value changed!\n");
                lwm2m_resource_value_changed(lwm2mH, uri);
            }
            lwm2m_data_free(1, dataP);
            return;
        }
        else
        {
            KING_SysLog("write not supported for specified resource!\n");
        }
        return;
    }
    else
    {
        KING_SysLog("Object not found !\n");
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
      KING_SysLog("Connection creation failed.\n");
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
    if (0==strncmp(uri, "coaps://", strlen("coaps://"))) {
        host = uri+strlen("coaps://");
    }
    else if (0==strncmp(uri, "coap://",  strlen("coap://"))) {
        host = uri+strlen("coap://");
    }
    else {
        goto exit;
    }
    port = strrchr(host, ':');
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

    KING_SysLog("Opening connection to server at %s:%s\r\n", host, port);
    // NOTE: the dataP->sock is already created when bind device.
    newConnP = connection_create(dataP->connList, dataP->sock, host, port, dataP->addressFamily);
    if (newConnP == NULL) {
        KING_SysLog("Connection creation failed.\r\n");
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

#if 0
static void prv_output_servers(char * buffer,
                               void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *) user_data;
    lwm2m_server_t * targetP;

    targetP = lwm2mH->bootstrapServerList;

    if (lwm2mH->bootstrapServerList == NULL)
    {
        KING_SysLog("No Bootstrap Server.\r\n");
    }
    else
    {
        KING_SysLog("Bootstrap Servers:\r\n");
        for (targetP = lwm2mH->bootstrapServerList ; targetP != NULL ; targetP = targetP->next)
        {
            KING_SysLog("objid[%d] lifetime[%d] \tstatus: \r\n",
                        (targetP->secObjInstID),
                        (unsigned long)targetP->lifetime);
            switch(targetP->status)
            {
            case STATE_DEREGISTERED:
                KING_SysLog("DEREGISTERED\r\n");
                break;
            case STATE_BS_HOLD_OFF:
                KING_SysLog("CLIENT HOLD OFF\r\n");
                break;
            case STATE_BS_INITIATED:
                KING_SysLog("BOOTSTRAP INITIATED\r\n");
                break;
            case STATE_BS_PENDING:
                KING_SysLog("BOOTSTRAP PENDING\r\n");
                break;
            case STATE_BS_FINISHED:
                KING_SysLog("BOOTSTRAP FINISHED\r\n");
                break;
            case STATE_BS_FAILED:
                KING_SysLog("BOOTSTRAP FAILED\r\n");
                break;
            default:
                KING_SysLog("%d",(int)targetP->status);
            }
        }
    }

    if (lwm2mH->serverList == NULL)
    {
        KING_SysLog("No LWM2M Server.\r\n");
    }
    else
    {
        KING_SysLog("LWM2M Servers:\r\n");
        for (targetP = lwm2mH->serverList ; targetP != NULL ; targetP = targetP->next)
        {
            KING_SysLog("shortID[%D] \tstatus: ", targetP->shortID);
            switch(targetP->status)
            {
            case STATE_DEREGISTERED:
                KING_SysLog("DEREGISTERED\r\n");
                break;
            case STATE_REG_PENDING:
                KING_SysLog("REGISTRATION PENDING\r\n");
                break;
            case STATE_REGISTERED:
                KING_SysLog("%d",(unsigned long)targetP->lifetime);
                break;
            case STATE_REG_UPDATE_PENDING:
                KING_SysLog("REGISTRATION UPDATE PENDING\r\n");
                break;
            case STATE_DEREG_PENDING:
                KING_SysLog("DEREGISTRATION PENDING\r\n");
                break;
            case STATE_REG_FAILED:
                KING_SysLog("REGISTRATION FAILED\r\n");
                break;
            default:
                KING_SysLog("%d",(int)targetP->status);
            }
        }
    }
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
            KING_SysLog("%d", objectP->objID);
        }
        else
        {
            lwm2m_list_t * instanceP;

            for (instanceP = objectP->instanceList; instanceP != NULL ; instanceP = instanceP->next)
            {
                KING_SysLog("%d", instanceP->id);
            }
        }
        KING_SysLog("\r\n");
    }
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
        KING_SysLog("Object not found.\n");
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
            KING_SysLog("instance ID [%d] ", instanceP->id);
            prv_instance_dump(objectP, instanceP->id);
            KING_SysLog("\r\n");
        }
    }

    return;

syntax_error:
    KING_SysLog("Syntax error !\n");
}

static void prv_update(char * buffer,
                       void * user_data)
{
    lwm2m_context_t * lwm2mH = (lwm2m_context_t *)user_data;
    if (buffer[0] == 0) goto syntax_error;

    uint16 serverId = (uint16) atoi(buffer);
    int res = lwm2m_update_registration(lwm2mH, serverId, false);
    if (res != 0)
    {
        KING_SysLog("Registration update error: %d\r\n", res);
    }
    return;

syntax_error:
    KING_SysLog("Syntax error !\n");
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
            valueLength = sprintf(value, "%d", level);
            handle_value_changed(context, &uri, value, valueLength, LWM2M_TYPE_INTEGER);
        }
        level = rand() % 20;
        if (0 > level) level = -level;
        next_change_time = tv_sec + level + 10;
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
        KING_SysLog("Error: %d\r\n", res);
        return;
    }

    //dump_tlv(stdout, numData, dataArray, 0);
}
#endif


bool add_mult_obj_insts(lwm2m_context_t *lwm2mH, uint16 objid, uint32 instCount)
{
    lwm2m_object_t *objectP;
    int res;

    objectP = get_test_object(objid, instCount);
    if (objectP == NULL)
    {
        KING_SysLog("Fail to get test obj[%d]", objid);
        return false;
    }

    res = lwm2m_add_object(lwm2mH, objectP);
    if (res != 0)
    {
        KING_SysLog("objid[%d] status[%d]\r\n", objid, res);
        return false;
    }
    else
    {
        KING_SysLog("objid inst Count[%d]", instCount);
    }
    return true;
}

#ifdef LWM2M_BOOTSTRAP
#if 0
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
#endif

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
    KING_SysLog("[BOOTSTRAP] ObjectList restored\r\n");
}

static void update_bootstrap_info(lwm2m_client_state_t * previousBootstrapState,lwm2m_context_t * context)
{
    if (*previousBootstrapState != context->state)
    {
        *previousBootstrapState = context->state;
        switch(context->state)
        {
            case STATE_BOOTSTRAPPING:
                KING_SysLog("[BOOTSTRAP] backup security and server objects\r\n");
                prv_backup_objects(context);
                break;
            default:
                break;
        }
    }
}
#endif //LWM2M_BOOTSTRAP

void aepSocketEventProcess(uint32 eventID, void* pdata, uint32 len)
{
    uint16 uDataSize = 0;
    KH_CB_DATA_U *data = (KH_CB_DATA_U *)pdata;

    switch (eventID)
    {
    case SOCKET_CONNECT_EVENT_RSP:
        KING_SysLog("SOCKET_CONNECT_EVENT_RSP");
        break;

    case SOCKET_ACCEPT_EVENT_IND:
        KING_SysLog("EV_CFW_TCPIP_ACCEPT_IND");
        break;

    case SOCKET_FULL_CLOSED_EVENT_IND:
        KING_SysLog("SOCKET_FULL_CLOSED_EVENT_IND");
        break;

    case SOCKET_ERROR_EVENT_IND:
        KING_SysLog("EV_CFW_TCPIP_ERR_IND");
        KING_SocketClose(clientData.sock);
        clientData.sock = -1;
        break;

    case SOCKET_WRITE_EVENT_IND:
        KING_SysLog("SOCKET_WRITE_EVENT_IND");
        break;

    case SOCKET_READ_EVENT_IND:
        uDataSize = data->socket.availSize;
        KING_SysLog("SOCKET_READ_EVENT_IND receive datalen=%d\r\n",uDataSize);
        aepRecvData(uDataSize);
        break;

    default:
        KING_SysLog("unexpect event/response %d",eventID);
        break;
    }

}

static void aepEventHandle(LWM2M_EVENT_CONTEXT* event)
{
    KING_SysLog("aepEventHandle id: %d ",event->id);
    uint32_t result = event->param1;
    uint32_t result_param1 = event->param2;
    uint32_t result_param2 = event->param3;
    switch(event->id)
    {
        case EVENT_IND:
        {
            char refstr[50] = {0};
            sprintf(refstr, "+LWM2MEVENT: %s", lwm2m_eventName(result));
            KING_SysLog("%s",refstr);
        }
        break;

        case OBJ_OBSERVE_IND:
        {
            uint16_t objid = result;
            int16_t insid = result_param1;
            int16_t resid = result_param2;
            KING_SysLog("OBJ_OBSERVE_IND objid:%d,insid:%d,resid:%d",objid,insid,resid);
            if(objid == 19 && insid ==0 && resid == 0)
            {
                KING_SysLog("AEP observe success!!");
            }
            
        }
        break;
        
        case OBJ_WRITE_IND:
        {
            uint8 *data = (uint8 *)result_param1;
            int len = result_param2;
            KING_SysLog("AEP_WRITE_IND data: %s, len: %d",data,len);
        }
        break;

        case OBJ_READ_IND:
        case OBJ_EXE_IND:
        case OBJ_OPERATE_RSP_IND:
        case SERVER_REGCMD_RESULT_IND:
        case SERVER_QUIT_IND:
        case FOTA_DOWNLOADING_IND:
        case FOTA_DOWNLOAD_FAILED_IND:
        case FOTA_DOWNLOAD_SUCCESS_IND:
        case FOTA_PACKAGE_CHECK_IND:
        case FOTA_UPGRADE_OK_IND:
        break;
        
        case DATA_SEND_IND:
        {
            char refstr[50] = {0};
            sprintf(refstr, "+LWM2MEVENT: %s", lwm2m_eventName(result));
            KING_SysLog("%s",refstr);
        }
        break;

        default: break;
    }
}
/*
 * This function does two things:
 *  - first it does the work needed by liblwm2m (eg. (re)sending some packets).
 *  - Secondly it adjusts the timeout value (default 60s) depending on the state of the transaction
 *    (eg. retransmission) and the time between the next operation
 */
int aepLwm2mStep()
{
    int result = -1;
    time_t tv_sec = 60;

    result = lwm2m_step(clientData.lwm2mH, &(tv_sec));
    KING_SysLog(" -> State: ");
    switch (clientData.lwm2mH->state)
    {
    case STATE_INITIAL:
        KING_SysLog("STATE_INITIAL\r\n");
        break;
    case STATE_BOOTSTRAP_REQUIRED:
        KING_SysLog("STATE_BOOTSTRAP_REQUIRED\r\n");
        break;
    case STATE_BOOTSTRAPPING:
        KING_SysLog("STATE_BOOTSTRAPPING\r\n");
        break;
    case STATE_REGISTER_REQUIRED:
        KING_SysLog("STATE_REGISTER_REQUIRED\r\n");
        break;
    case STATE_REGISTERING:
        KING_SysLog("STATE_REGISTERING\r\n");
        break;
    case STATE_READY:
        KING_SysLog("STATE_READY\r\n");
        break;
    default:
        KING_SysLog("Unknown...\r\n");
        break;
    }

    return result;
}

void aepSendData(char *buffer)
{
    lwm2m_uri_t uri;
    int result;

    result = lwm2m_stringToUri("/19/0/0", strlen("/19/0/0"), &uri);
    if (result == 0) goto syntax_error;

    if (buffer == NULL || buffer[0] == 0)
    {
        KING_SysLog("report change!\n");
        lwm2m_resource_value_changed(clientData.lwm2mH, &uri);
    }
    else
    {
        handle_value_changed(clientData.lwm2mH, &uri, buffer, strlen(buffer), LWM2M_TYPE_OPAQUE);
    }
    aepLwm2mStep();
    return;

syntax_error:
    KING_SysLog("Syntax error !\n");
}


static int aepRecvData(size_t len)
{
    int ret = -1;
    uint8 buffer[MAX_PACKET_SIZE];
    struct sockaddr addr;
    socklen_t addrLen;

    addrLen = sizeof(struct sockaddr);

    /*
     * We retrieve the clientData received
     */
    ret = KING_SocketRecvfrom(clientData.sock, buffer, len, 0, NULL, NULL);
    
    KING_SysLog("aep_server_recv_data ret %d",ret);

    if (ret < 0)
    {
        KING_SysLog("Error in recvfrom()\r\n");
    }
    else
    {
        dtls_connection_t * connP;

        connP = connection_find(clientData.connList, &addr, addrLen);
        if (connP != NULL)
        {
            /*
             * Let liblwm2m respond to the query depending on the context
             */
#ifdef WITH_TINYDTLS
            int result = connection_handle_packet(connP, buffer, len);
            if (0 != result)
            {
                 KING_SysLog("error handling message %d\n",result);
            }
#else
            lwm2m_handle_packet(clientData.lwm2mH, buffer, len, connP);
#endif
            conn_s_updateRxStatistic(clientData.lwm2mH, len, false);
            aepLwm2mStep();
        }
        else
        {
            KING_SysLog("received bytes ignored!\r\n");
        }
    }
    return 0;
}


int aepRegDevice(void)
{
    int result;
    lwm2m_context_t *lwm2mH = NULL;
    const char *localPort = "56830";
    const char *server = NULL;
    const char *serverPort = LWM2M_STANDARD_PORT_STR;
    char *name = NULL;
    int lifetime = 86400;
    bool bootstrapRequested = false;
    char serverUri[50];
    int serverId = 123;
    char myIMEI[20] = { 0 };

#ifdef LWM2M_BOOTSTRAP
    lwm2m_client_state_t previousState = STATE_INITIAL;
#endif

    char * pskId = NULL;
#ifdef WITH_TINYDTLS
    char * psk = NULL;
#endif
    uint16_t pskLen = -1;
    char * pskBuffer = NULL;

    memset(&clientData, 0, sizeof(client_data_t));
    get_system_imei((uint8 *)myIMEI);
    name = myIMEI; // Test IMEI
    server = "221.229.214.202"; // AEP IOT server.
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
    KING_SysLog("[%s]Trying to bind LWM2M Client to port %s\r\n", name, localPort);
    clientData.sock = create_socket(localPort, clientData.addressFamily);

    if (clientData.sock < 0)
    {
        KING_SysLog("failed to create socket!\r\n");
        return -1;
    }

    KING_RegNotifyFun(DEV_CLASS_SOCKET, clientData.sock, aepSocketEventProcess);

    /*
     * Now the main function fill an array with each object, this list will be later passed to liblwm2m.
     * Those functions are located in their respective object file.
     */
#if (defined WITH_TINYDTLS) || (defined WITH_MBEDTLS)
    if (psk != NULL)
    {
        pskLen = strlen(psk) / 2;
        pskBuffer = malloc(pskLen);

        if (NULL == pskBuffer)
        {
            KING_SysLog("Failed to create PSK binary buffer\r\n");
            return -1;
        }
        // Hex string to binary
        char *h = psk;
        char *b = pskBuffer;
        char xlate[] = "0123456789ABCDEF";

        for ( ; *h; h += 2, ++b)
        {
            char *l = strchr(xlate, toupper(*h));
            char *r = strchr(xlate, toupper(*(h+1)));

            if (!r || !l)
            {
                KING_SysLog("Failed to parse Pre-Shared-Key HEXSTRING\r\n");
                return -1;
            }

            *b = ((l - xlate) << 4) + (r - xlate);
        }
    }
#endif

#if (defined WITH_TINYDTLS) || (defined WITH_MBEDTLS)
    sprintf(serverUri, "coaps://%s:%s", server, serverPort);
#else
    sprintf(serverUri, "coap://%s:%s", server, serverPort);
#endif

#ifdef LWM2M_BOOTSTRAP
    objArray[0] = get_security_object(serverId, serverUri, pskId, pskBuffer, pskLen, bootstrapRequested);
#else
    objArray[0] = get_security_object(serverId, serverUri, pskId, pskBuffer, pskLen, false);
#endif

    if (NULL == objArray[0])
    {
        KING_SysLog("Failed to create security object\r\n");
        return -1;
    }
    KING_SysLog("create security object success\r\n");
    
    clientData.securityObjP = objArray[0];

    objArray[1] = get_server_object(serverId, "U", lifetime, false);
    if (NULL == objArray[1])
    {
        KING_SysLog("Failed to create server object\r\n");
        return -1;
    }
    KING_SysLog("create server object success\r\n");

    objArray[2] = get_object_device();
    if (NULL == objArray[2])
    {
        KING_SysLog("Failed to create Device object\r\n");
        return -1;
    }
    KING_SysLog("create device object success\r\n");

    objArray[3] = get_object_firmware();
    if (NULL == objArray[3])
    {
        KING_SysLog("Failed to create Firmware object\r\n");
        return -1;
    }
    KING_SysLog("create device firmware success\r\n");

    objArray[4] = get_object_location();
    if (NULL == objArray[4])
    {
        KING_SysLog("Failed to create location object\r\n");
        return -1;
    }
    KING_SysLog("create device location success\r\n");

    /*
     * The liblwm2m library is now initialized with the functions that will be in
     * charge of communication
     */
    lwm2mH = lwm2m_init(&clientData);
    if (NULL == lwm2mH)
    {
        KING_SysLog("lwm2m_init() failed\r\n");
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
        KING_SysLog("lwm2m_configure returns %d", result);
        return -1;
    }

    /**
     * Initialize value changed callback.
     */
    init_value_change(lwm2mH);

    // special for aep server connect.
    add_mult_obj_insts(lwm2mH, 19, 2);

    lwm2m_sethandler(aepEventHandle);
    result = aepLwm2mStep();

    if (result != 0)
    {
        KING_SysLog("lwm2m_step returns error: %d", result);
        if(previousState == STATE_BOOTSTRAPPING)
        {
            KING_SysLog("[BOOTSTRAP] restore security and server objects\r\n");
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
