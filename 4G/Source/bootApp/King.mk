#########################################################################
#
# Xiamen CHEERZING IOT Tech KingSDK Build System
# All Rights Reserved 2019
# Author: LuXiao
#########################################################################

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := lib
LOCAL_MODULE := bootApp
LOCAL_PATH := $(CURDIR)
LOCAL_IS_HOST_MODULE :=

#定义源代码搜索路径
LOCAL_SRC_DIR := $(PLATFORM_NAME)/src
MSRCPATH = ./Source/bootApp/$(LOCAL_SRC_DIR)

#定义头文件搜索路径
LOCAL_C_INCLUDES := ./Source/bootApp/$(PLATFORM_NAME)/inc

LOCAL_COMPILER_VARIABLES := DUMMY_MACRO
LOCAL_COMPILER_VARIABLES+=TEST_MACRO1
LOCAL_COMPILER_VARIABLES+=TEST_MACRO2

LOCAL_C_INCLUDES_I += -I ./Source/bootApp/$(PLATFORM_NAME)/inc
LOCAL_C_INCLUDES_I += -I ./Source/std/inc 
LOCAL_C_INCLUDES_I += -I ./Source/bootloader/$(PLATFORM_NAME)/inc
LOCAL_C_INCLUDES_I += -I ./Source/bootloader/$(PLATFORM_NAME)/chip/hal/inc
LOCAL_C_INCLUDES_I += -I ./Source/bootloader/$(PLATFORM_NAME)/chip/regs/inc

SOURCES = $(notdir $(wildcard $(MSRCPATH)/*.c))

include $(BUILD_STATIC_LIBRARY)

