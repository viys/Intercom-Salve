#########################################################################
#
# Xiamen CHEERZING IOT Tech KingSDK Build System
# All Rights Reserved 2019
# Author: Nestor Zhang
#########################################################################

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := lib
LOCAL_MODULE := mbedtls
LOCAL_PATH := $(CURDIR)
LOCAL_IS_HOST_MODULE := 

#定义源代码搜索路径
MSRCPATH =  ./Source/mbedtls

#定义头文件搜索路径
#定义头文件搜索路径

LOCAL_C_INCLUDES := ./inc,./Source/mbedtls,./Source/mbedtls/include

LOCAL_COMPILER_VARIABLES := DUMMY_MACRO
LOCAL_COMPILER_VARIABLES+=TEST_MACRO1
LOCAL_COMPILER_VARIABLES+=TEST_MACRO2

#LOCAL_C_INCLUDES_I := $(addprefix,-I,$(subst $(comma),$(space),$(LOCAL_C_INCLUDES)))

LOCAL_C_INCLUDES_I = -I ./inc -I ./inc/private -I ./inc/tapi
LOCAL_C_INCLUDES_I +=-I ./Source/mbedtls/include 
LOCAL_C_INCLUDES_I +=-I ./Source/mbedtls 
LOCAL_C_INCLUDES_I +=-I ./Source 

SOURCES = $(notdir $(wildcard $(MSRCPATH)/*.c))

include $(BUILD_STATIC_LIBRARY)

