#########################################################################
# Xiamen CHEERZING IOT Tech KingSDK Build System
# All Rights Reserved 2019
# Author: Nestor Zhang
#########################################################################

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := lib
LOCAL_MODULE := mqtt
LOCAL_PATH := $(CURDIR)
LOCAL_IS_HOST_MODULE := 

#定义源代码搜索路径
LOCAL_SRC_DIR := src
MSRCPATH =  ./Source/mqtt/$(LOCAL_SRC_DIR)

#定义头文件搜索路径
#定义头文件搜索路径
LOCAL_C_INCLUDES := ./inc,./Source/mqtt/inc,./Source,./inc/private,./inc/tapi,./Source/mbedtls/include

LOCAL_COMPILER_VARIABLES := DUMMY_MACRO
#LOCAL_COMPILER_VARIABLES+=TEST_MACRO1
#LOCAL_COMPILER_VARIABLES+=TEST_MACRO2

COMMON_GLOBAL_CFLAGS += -DINFRA_SHA256

LOCAL_C_INCLUDES_I := $(addprefix -I,$(subst $(comma),$(space),$(LOCAL_C_INCLUDES)))

SOURCES = $(notdir $(wildcard $(MSRCPATH)/*.c))

include $(BUILD_STATIC_LIBRARY)

