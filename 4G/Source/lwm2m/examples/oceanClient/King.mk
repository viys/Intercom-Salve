#########################################################################
# Xiamen CHEERZING IOT Tech KingSDK Build System
# All Rights Reserved 2019
# Author: Nestor Zhang
#########################################################################

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := lib
LOCAL_MODULE := oceanClient
LOCAL_PATH := $(CURDIR)
LOCAL_IS_HOST_MODULE := 

#定义源代码搜索路径
MSRCPATH =  ./Source/oceanClient

#定义头文件搜索路径
#定义头文件搜索路径

LOCAL_C_INCLUDES := ./inc,./Source/coap,./Source/lwm2m,./Source/NA2201,./Source

LOCAL_COMPILER_VARIABLES := DUMMY_MACRO
LOCAL_COMPILER_VARIABLES+=TEST_MACRO1
LOCAL_COMPILER_VARIABLES+=TEST_MACRO2

#LOCAL_C_INCLUDES_I := $(addprefix,-I,$(subst $(comma),$(space),$(LOCAL_C_INCLUDES)))

LOCAL_C_INCLUDES_I = -I ./inc 
LOCAL_C_INCLUDES_I +=-I ./Source/coap 
LOCAL_C_INCLUDES_I +=-I ./Source/lwm2m 
LOCAL_C_INCLUDES_I +=-I ./Source/oceanClient 
LOCAL_C_INCLUDES_I +=-I ./Source/NA2201 
LOCAL_C_INCLUDES_I +=-I ./Source 

SOURCES = $(notdir $(wildcard $(MSRCPATH)/*.c))

include $(BUILD_STATIC_LIBRARY)

