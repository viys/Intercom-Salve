export ECHO:=Echo
export TOP :=.
export TOPDIR :=./
export empty:=
export space:=$(empty) $(empty)
export comma:=,

export SIGN := KingSign

export BUILD_SYSTEM := $(TOPDIR)make/core
export BUILD_ENV    := $(TOPDIR)env

ifeq ($(PLATFORM_NAME),)
$(error Please run SetEnv.bat Once!)
endif

#Base config rules
include $(BUILD_SYSTEM)/config.mk

include $(BUILD_SYSTEM)/envCfg_$(PLATFORM_NAME).mk

#Initialize local variables
include $(BUILD_SYSTEM)/ClearVar.mk

#Initialize Project Variables
include $(TOP)/Devices/$(DEVICE)/Project.mk

#Initialize Modules
include $(TOP)/Devices/$(DEVICE)/Modules.mk

LIB_LIST:=$(addsuffix .a,$(MODULES))
DOS_OUTPUT_DIRECTORY:=$(subst /,\,$(OUT_DIR))
ALL_LIBS:=$(addprefix   $(OUT_DIR)/lib/,$(LIB_LIST))

%.a:
	@make -f $(TOPDIR)Source/$(subst .a,,$@)/King.mk

all:$(LIB_LIST)

include $(BUILD_SYSTEM)/genImg_${PLATFORM_NAME}.mk

.PHONY: clean
clean:
	@echo Cleaning...
	@if exist $(DOS_OUTPUT_DIRECTORY) rd $(subst . ,.\,$(wordlist 1,2, $(subst \, ,$(DOS_OUTPUT_DIRECTORY)))) /s/q
	@echo "Entire build directory removed."

help:
	@$(ECHO) Xiamen CHEERZING IOT Technology KingSDK Build System V1.0:
	@$(ECHO) SetEnv.bat: Setup RVCT and make_cmd Path and BUILD_OPERATOR(for Xiamen CHEERZING IOT Internal use)
	@$(ECHO) SetEnv.bat should be run only once!
	@$(ECHO) Build_App.bat: to Build Release Sample device codes
	@$(ECHO) Build_AppDebug.bat to Build Debug Sample device codes
	@$(ECHO) Build_App.bat clean: remove all builds of release variant
	@$(ECHO) Build_AppDebug.bat clean: remove all builds of debug variant

