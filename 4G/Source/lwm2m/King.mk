#########################################################################
#
# Xiamen CHEERZING IOT Tech KingSDK Build System
# All Rights Reserved 2019
# Author: Nestor Zhang
#########################################################################

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := lib
LOCAL_MODULE := lwm2m
LOCAL_PATH := $(CURDIR)
LOCAL_IS_HOST_MODULE := 

#����Դ��������·��
MSRCPATH =  ./Source/lwm2m

#����ͷ�ļ�����·��
#����ͷ�ļ�����·��

LOCAL_C_INCLUDES := ./inc,./Source/coap,./Source/lwm2m

LOCAL_COMPILER_VARIABLES := DUMMY_MACRO
LOCAL_COMPILER_VARIABLES+=TEST_MACRO1
LOCAL_COMPILER_VARIABLES+=TEST_MACRO2

#LOCAL_C_INCLUDES_I := $(addprefix,-I,$(subst $(comma),$(space),$(LOCAL_C_INCLUDES)))

LOCAL_C_INCLUDES_I = -I ./inc 
LOCAL_C_INCLUDES_I +=-I ./Source/coap 
LOCAL_C_INCLUDES_I +=-I ./Source/lwm2m 

SOURCES = $(notdir $(wildcard $(MSRCPATH)/*.c))

include $(BUILD_STATIC_LIBRARY)

