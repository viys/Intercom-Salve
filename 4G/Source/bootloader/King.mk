#########################################################################
#
# Xiamen CHEERZING IOT Tech KingSDK Build System
# All Rights Reserved 2019
# Author: LuXiao Nestor.Zhang
#########################################################################

include $(CLEAR_VARS)

LOCAL_MODULE_CLASS := lib
LOCAL_MODULE := bootloader
LOCAL_PATH := $(CURDIR)/$(PLATFORM_NAME)
LOCAL_IS_HOST_MODULE :=


#定义源代码搜索路径
LOCAL_SRC_DIR := $(PLATFORM_NAME)/src
MSRCPATH = ./Source/bootloader/$(LOCAL_SRC_DIR)

#定义头文件搜索路径
LOCAL_C_INCLUDES := ./Source/bootloader/$(PLATFORM_NAME)/chip,./Source/bootloader/$(PLATFORM_NAME)/inc

LOCAL_COMPILER_VARIABLES := DUMMY_MACRO
LOCAL_COMPILER_VARIABLES+=TEST_MACRO11
LOCAL_COMPILER_VARIABLES+=TEST_MACRO12

LOCAL_C_INCLUDES_I = -I ./Source/bootloader/$(PLATFORM_NAME)/inc \
                     -I ./Source/bootloader/$(PLATFORM_NAME)/inc/hal \
                     -I ./Source/bootloader/$(PLATFORM_NAME)/chip/defs/inc \
                     -I ./Source/bootloader/$(PLATFORM_NAME)/chip/pmd/inc \
                     -I ./Source/bootloader/$(PLATFORM_NAME)/chip/regs/inc \
                     -I ./Source/bootloader/$(PLATFORM_NAME)/chip/hal/inc 
					 
LOCAL_C_INCLUDES_I += -I ./Source/std/inc 

LOCAL_C_INCLUDES_I += -I ./Source/bootApp/$(PLATFORM_NAME)/inc 

SOURCES = $(notdir $(wildcard $(MSRCPATH)/*.c))
SOURCES += $(notdir $(wildcard $(MSRCPATH)/*.S))

include $(BUILD_STATIC_LIBRARY)

