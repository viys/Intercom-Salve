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
LOCAL_SRC_DIR := src
MSRCPATH =  ./Source/App/$(LOCAL_SRC_DIR)

#����ͷ�ļ�����·��
LOCAL_C_INCLUDES := ./inc,./Source/App/inc
LOCAL_COMPILER_VARIABLES := DUMMY_MACRO

LOCAL_C_INCLUDES_I := $(addprefix -I,$(subst $(comma),$(space),$(LOCAL_C_INCLUDES)))

SOURCES = $(notdir $(wildcard $(MSRCPATH)/*.c))

include $(BUILD_STATIC_LIBRARY)

