# This is included by the top-level Makefile.
# It sets up standard variables based on the
# current configuration and platform, which
# are not specific to what is being built.

export CLEAR_VARS:= $(BUILD_SYSTEM)/ClearVar.mk
export BUILD_HOST_STATIC_LIBRARY:= $(BUILD_SYSTEM)/host_static_library.mk
export BUILD_HOST_SHARED_LIBRARY:= $(BUILD_SYSTEM)/host_shared_library.mk
export BUILD_STATIC_LIBRARY:= $(BUILD_SYSTEM)/static_library.mk
export BUILD_RAW_STATIC_LIBRARY := $(BUILD_SYSTEM)/raw_static_library.mk
export BUILD_SHARED_LIBRARY:= $(BUILD_SYSTEM)/shared_library.mk
export BUILD_EXECUTABLE:= $(BUILD_SYSTEM)/executable.mk
export BUILD_RAW_EXECUTABLE:= $(BUILD_SYSTEM)/raw_executable.mk
export BUILD_HOST_EXECUTABLE:= $(BUILD_SYSTEM)/host_executable.mk


GLOBLE_COMPILER_FLAGS :=
GCC_COMPILER_FLAGS :=

ifeq ($(strip $(TARGET_BUILD_TYPE)),debug)
GLOBLE_COMPILER_FLAGS += KING_DEBUG_VARIANT
OUT_DIR := $(TOPDIR)Build/$(PLATFORM_NAME)/$(DEVICE)/debug
else #TARGET_BUILD_TYPE
OUT_DIR := $(TOPDIR)Build/$(PLATFORM_NAME)/$(DEVICE)/release
endif #TARGET_BUILD_TYPE

TARGET_OUT_INTERMEDIATES := $(OUT_DIR)

export OUT_DIR TARGET_OUT_INTERMEDIATES GLOBLE_COMPILER_FLAGS GCC_COMPILER_FLAGS

