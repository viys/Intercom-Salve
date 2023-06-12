#########################################################################
#
# Xiamen CHEERZING IOT Tech KingSDK Build System
# All Rights Reserved 2019
# Author: LuXiao
#########################################################################

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := lib
LOCAL_MODULE := App
LOCAL_PATH := $(CURDIR)
LOCAL_IS_HOST_MODULE := 

#����Դ��������·��
MSRCPATH =  ./Source/App

#����ͷ�ļ�����·��
#����ͷ�ļ�����·��

LOCAL_C_INCLUDES := ./inc,./Source/App,./inc/tapi,./version

LOCAL_COMPILER_VARIABLES := DUMMY_MACRO

#LOCAL_C_INCLUDES_I := $(addprefix -I,$(subst $(comma),$(space),$(LOCAL_C_INCLUDES)))

LOCAL_C_INCLUDES_I = -I ./inc 

LOCAL_C_INCLUDES_I += -I ./inc/tapi 

LOCAL_C_INCLUDES_I += -I ./version

SOURCES = $(notdir $(wildcard $(MSRCPATH)/*.c))

include $(BUILD_STATIC_LIBRARY)

