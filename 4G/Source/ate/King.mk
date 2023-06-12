#########################################################################
# Xiamen CHEERZING IOT Tech KingSDK Build System
# All Rights Reserved 2019
# Author: Nestor Zhang
#########################################################################

include $(CLEAR_VARS)


LOCAL_MODULE_CLASS := lib
LOCAL_MODULE := ate
LOCAL_PATH := $(CURDIR)
LOCAL_IS_HOST_MODULE := 

#定义源代码搜索路径
LOCAL_SRC_DIR := src
MSRCPATH = ./Source/ate/$(LOCAL_SRC_DIR)

#定义头文件搜索路径
LOCAL_C_INCLUDES := ./inc \
                    ./Source/ate/inc \
                    ./Source \
                    ./Source/ModemApp\inc

LOCAL_COMPILER_VARIABLES := DUMMY_MACRO

LOCAL_C_INCLUDES_I := $(addprefix -I,$(subst $(comma),$(space),$(LOCAL_C_INCLUDES)))

SOURCES = $(notdir $(wildcard $(MSRCPATH)/*.c))

MY_SRC_DIR := $(strip $(subst ./,,$(LOCAL_PATH)/$(dir $(MSRCPATH))))
AT_CMD_TABLE_INC_PATH = $(MY_SRC_DIR)inc

COMMON_GLOBAL_CFLAGS += -DYY_MEM_ONLY -DYY_NO_ERRNO

ifneq (,$(findstring tts,${MODULES}))
COMMON_GLOBAL_CFLAGS += -DKING_SDK_TTS_SUPPORT
endif

ifneq (,$(findstring ftm,${MODULES}))
COMMON_GLOBAL_CFLAGS += -DKING_SDK_FTM_SUPPORT
endif


#$(intermediates)::headergen
#libate.a : headergen

include $(BUILD_STATIC_LIBRARY)

#$(intermediates):|headergen

#$(intermediates):DESTINATION_DIR:=$(subst /,\,$(intermediates)) headergen
