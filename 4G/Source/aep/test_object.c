/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    domedambrosio - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Axel Lorente - Please refer to git log
 *    Achim Kraus, Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    Ville Skyttä - Please refer to git log
 *
 *******************************************************************************/

#include "liblwm2m.h"
#include "lwm2mclient.h"
#include "er-coap.h"
#include "internals.h"

#define PRV_TLV_BUFFER_SIZE 64
// Ported from LWIP
#ifndef isprint
#define in_range(c, lo, up)  ((uint8)c >= lo && (uint8)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)
#endif


typedef struct _skel_res_
{
    uint16 resId;
    lwm2m_data_type_t type;
    uint8 *data;
    double floatValue;
    uint32 len;
} skel_res_t;

extern size_t utils_intToText(int64_t data,
                       uint8_t * string,
                       size_t length);

extern size_t utils_floatToText(double data,
                        uint8_t * string,
                        size_t length);

/*
 * Multiple instance objects can use userdata to store data that will be shared between the different instances.
 * The lwm2m_object_t object structure - which represent every object of the liblwm2m as seen in the single instance
 * object - contain a chained list called instanceList with the object specific structure prv_instance_t:
 */
typedef struct _prv_instance_
{
    /*
     * The first two are mandatories and represent the pointer to the next instance and the ID of this one. The rest
     * is the instance scope user data (uint8 test in this case)
     */
    struct _prv_instance_ * next;   // matches lwm2m_list_t::next
    uint16 instanceId;               // matches lwm2m_list_t::id
    skel_res_t dataArray;         // only support one resource data for now.
} prv_instance_t;

static void prv_output_buffer(uint8 * buffer,
                              int length)
{
    int i;
    uint8 array[16];

    i = 0;
    while (i < length)
    {
        int j;
        LOG("  ");

        memcpy(array, buffer+i, 16);

        for (j = 0 ; j < 16 && i+j < length; j++)
        {
            LOG_ARG("%d", array[j]);
        }
        while (j < 16)
        {
            LOG("   ");
            j++;
        }
        LOG("  ");
        for (j = 0 ; j < 16 && i+j < length; j++)
        {
            if (isprint(array[j]))
                LOG_ARG("%d", array[j]);
            else
                LOG(". ");
        }
        LOG("\n");

        i += 16;
    }
}

#define _PRV_STR_LENGTH 32

static int skel_valuedecode(lwm2m_data_t * dataP, uint8 ** bufferP)
{
    size_t res;
    lwm2m_data_type_t type = dataP->type;

    switch (type)
    {
    case LWM2M_TYPE_OPAQUE:
    case LWM2M_TYPE_STRING:
        *bufferP = (uint8 *)lwm2m_malloc(dataP->value.asBuffer.length+1);
        if (*bufferP == NULL) return 0;
        memcpy(*bufferP, dataP->value.asBuffer.buffer, dataP->value.asBuffer.length);
        (*bufferP)[dataP->value.asBuffer.length] = 0;
        return (int)dataP->value.asBuffer.length;

    case LWM2M_TYPE_INTEGER:
    {
        uint8 intString[_PRV_STR_LENGTH];
        int64_t v = 0;
        lwm2m_data_decode_int(dataP,&v);
        res = utils_intToText(v, intString, _PRV_STR_LENGTH);
        if (res == 0) return -1;

        *bufferP = (uint8 *)lwm2m_malloc(res+1);
        if (NULL == *bufferP) return -1;

        memcpy(*bufferP, intString, res);
        (*bufferP)[res] = 0;
        return (int)res;
    }

    case LWM2M_TYPE_FLOAT:
    {
        uint8 floatString[_PRV_STR_LENGTH * 2];
        double v = 0;
        lwm2m_data_decode_float(dataP,&v);
        res = utils_floatToText(v, floatString, _PRV_STR_LENGTH * 2);
        if (res == 0) return -1;

        *bufferP = (uint8 *)lwm2m_malloc(res+1);
        if (NULL == *bufferP) return -1;

        memcpy(*bufferP, floatString, res);
        (*bufferP)[res] = 0;
        return (int)res;
    }

    case LWM2M_TYPE_BOOLEAN:
    {
        bool v = 0;
        lwm2m_data_decode_bool(dataP,&v);
        char *sv = v ? "1": "0";
        *bufferP = (uint8 *)lwm2m_malloc(strlen(sv)+1);
        if (NULL == *bufferP) return -1;
        strcpy((char*)*bufferP,sv);
        return strlen(sv);
    }
    case LWM2M_TYPE_UNDEFINED:
    default:
        return -1;
    }
}

static uint8 prv_read(uint16 instanceId,
                        int * numDataP,
                        lwm2m_data_t ** dataArrayP,
                        lwm2m_object_t * objectP)
{
    prv_instance_t *targetP;

    targetP = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    if (*numDataP == 0)
    {
        // TODO NOT SUPPORT
        return COAP_406_NOT_ACCEPTABLE;
    }

    if (*numDataP == 1)
    {
        lwm2m_data_t *dataP = (* dataArrayP);
        if (targetP->dataArray.resId == dataP->id)
        {
            uint8_t *data = targetP->dataArray.data;
            uint32_t len = targetP->dataArray.len;
            lwm2m_data_t* dataP = (*dataArrayP);

            if (data == NULL)
                return COAP_406_NOT_ACCEPTABLE;

            switch (targetP->dataArray.type)
            {
                case LWM2M_TYPE_STRING:
                    lwm2m_data_encode_string((char*)data, dataP);
                    break;
                case LWM2M_TYPE_OPAQUE:
                    lwm2m_data_encode_opaque(data, len, dataP);
                    break;
                case LWM2M_TYPE_INTEGER:
                    lwm2m_data_encode_int(atoi((const char *)data), dataP);
                    break;
                case LWM2M_TYPE_FLOAT:
                    lwm2m_data_encode_float(targetP->dataArray.floatValue, dataP);
                    break;
                case LWM2M_TYPE_BOOLEAN:
                    lwm2m_data_encode_bool(atoi((const char *)data), dataP);
                    break;
                default:
                    return COAP_406_NOT_ACCEPTABLE;
            }
            lwm2m_free(data);
            targetP->dataArray.data = NULL;
            return COAP_205_CONTENT;
        }
    }
    return COAP_404_NOT_FOUND;
}


static uint8 prv_discover(uint16 instanceId,
                            int * numDataP,
                            lwm2m_data_t ** dataArrayP,
                            lwm2m_object_t * objectP)
{
    int i;

    // is the server asking for the full object ?
    if (*numDataP == 0)
    {
        *dataArrayP = lwm2m_data_new(3);
        if (*dataArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        *numDataP = 3;
        (*dataArrayP)[0].id = 1;
        (*dataArrayP)[1].id = 2;
        (*dataArrayP)[2].id = 3;
    }
    else
    {
        for (i = 0; i < *numDataP; i++)
        {
            switch ((*dataArrayP)[i].id)
            {
            case 1:
            case 2:
            case 3:
                break;
            default:
                return COAP_404_NOT_FOUND;
            }
        }
    }

    return COAP_205_CONTENT;
}

static uint8 prv_write(uint16 instanceId,
                         int numData,
                         lwm2m_data_t * dataArray,
                         lwm2m_object_t * objectP)
{
    uint8 result = COAP_404_NOT_FOUND;
    prv_instance_t * targetP;

    targetP = (prv_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);

    if (NULL == targetP) return COAP_404_NOT_FOUND;

    if (numData == 1)
    {
        int i = 0;
        if (dataArray[i].id == targetP->dataArray.resId)
        {
            targetP->dataArray.type = dataArray[i].type;
            LOG_ARG("prv_write dataArray[%d].id=%d,type=%d",i,dataArray[i].id, dataArray[i].type);
            switch (targetP->dataArray.type)
            {
                case LWM2M_TYPE_OPAQUE:
                case LWM2M_TYPE_STRING:
                case LWM2M_TYPE_INTEGER:
                case LWM2M_TYPE_FLOAT:
                case LWM2M_TYPE_BOOLEAN:
                    {
                        uint8 *data;
                        int len = skel_valuedecode(&dataArray[i], &data);
                        if (len == -1) break;
                        if (targetP->dataArray.type == LWM2M_TYPE_FLOAT)
                        {
                            targetP->dataArray.floatValue = dataArray[i].value.asFloat;
                        }
                        if(targetP->dataArray.data != NULL)
                        {
                            lwm2m_free(targetP->dataArray.data);
                            targetP->dataArray.data = NULL;
                        }
                        targetP->dataArray.data = data;
                        targetP->dataArray.len = len;
                    }
                    result = COAP_204_CHANGED;
                    break;
                default:
                    result = COAP_404_NOT_FOUND;
            }
        }
    }
    return result;
}


static uint8 prv_delete(uint16 id,
                          lwm2m_object_t * objectP)
{
    prv_instance_t * targetP;

    objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&targetP);
    if (NULL == targetP) return COAP_404_NOT_FOUND;

    lwm2m_free(targetP);

    return COAP_202_DELETED;
}

static uint8 prv_create(uint16 instanceId,
                          int numData,
                          lwm2m_data_t * dataArray,
                          lwm2m_object_t * objectP)
{
    prv_instance_t * targetP;
    uint8 result;


    targetP = (prv_instance_t *)lwm2m_malloc(sizeof(prv_instance_t));
    if (NULL == targetP) return COAP_500_INTERNAL_SERVER_ERROR;
    memset(targetP, 0, sizeof(prv_instance_t));

    targetP->instanceId = instanceId;
    objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, targetP);

    result = prv_write(instanceId, numData, dataArray, objectP);

    if (result != COAP_204_CHANGED)
    {
        (void)prv_delete(instanceId, objectP);
    }
    else
    {
        result = COAP_201_CREATED;
    }

    return result;
}

static uint8 prv_exec(uint16 instanceId,
                        uint16 resourceId,
                        uint8 * buffer,
                        int length,
                        lwm2m_object_t * objectP)
{

    if (NULL == lwm2m_list_find(objectP->instanceList, instanceId)) return COAP_404_NOT_FOUND;

    switch (resourceId)
    {
    case 1:
        return COAP_405_METHOD_NOT_ALLOWED;
    case 2:
        LOG_ARG("\r\n-----------------\r\n"
                        "Execute on %hu/%d/%d\r\n"
                        " Parameter (%d bytes):\r\n",
                        objectP->objID, instanceId, resourceId, length);
        prv_output_buffer((uint8*)buffer, length);
        LOG("-----------------\r\n\r\n");
        return COAP_204_CHANGED;
    case 3:
        return COAP_405_METHOD_NOT_ALLOWED;
    default:
        return COAP_404_NOT_FOUND;
    }
}

void display_test_object(lwm2m_object_t * object)
{
#ifdef WITH_LOGS
    LOG_ARG("%d",object->objID);
    prv_instance_t * instance = (prv_instance_t *)object->instanceList;
    while (instance != NULL)
    {
        LOG_ARG("test: %u\r\n",
                object->objID, instance->instanceId,
                instance->instanceId, instance->test);
        instance = (prv_instance_t *)instance->next;
    }
#endif
}

lwm2m_object_t * get_test_object(uint16 objid, uint32 instCount)
{
    lwm2m_object_t * testObj;

    testObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));

    if (NULL != testObj)
    {
        int i;
        prv_instance_t * targetP;

        memset(testObj, 0, sizeof(lwm2m_object_t));

        testObj->objID = objid;
        for (i=0 ; i < instCount ; i++)
        {
            targetP = (prv_instance_t *)lwm2m_malloc(sizeof(prv_instance_t));
            if (NULL == targetP) return NULL;
            memset(targetP, 0, sizeof(prv_instance_t));
            targetP->instanceId = i;
            targetP->dataArray.resId = 0;
            testObj->instanceList = LWM2M_LIST_ADD(testObj->instanceList, targetP);
        }
        /*
         * From a single instance object, two more functions are available.
         * - The first one (createFunc) create a new instance and filled it with the provided informations. If an ID is
         *   provided a check is done for verifying his disponibility, or a new one is generated.
         * - The other one (deleteFunc) delete an instance by removing it from the instance list (and freeing the memory
         *   allocated to it)
         */
        testObj->readFunc = prv_read;
        testObj->discoverFunc = prv_discover;
        testObj->writeFunc = prv_write;
        testObj->executeFunc = prv_exec;
        testObj->createFunc = prv_create;
        testObj->deleteFunc = prv_delete;
    }

    return testObj;
}

void free_test_object(lwm2m_object_t * object)
{
    LWM2M_LIST_FREE(object->instanceList);
    if (object->userData != NULL)
    {
        lwm2m_free(object->userData);
        object->userData = NULL;
    }
    lwm2m_free(object);
}

