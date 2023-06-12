define intermediates-dir-for
$(strip \
    $(eval _idfClass := $(strip $(1))) \
    $(if $(_idfClass),, \
        $(error $(LOCAL_PATH): Class not defined in call to intermediates-dir-for)) \
    $(eval _idfName := $(strip $(2))) \
    $(if $(_idfName),, \
        $(error $(LOCAL_PATH): Name not defined in call to intermediates-dir-for)) \
    $(eval _idfPrefix := $(if $(strip $(3)),HOST,TARGET)) \
    $(eval _idfIntBase := $($(_idfPrefix)_OUT_INTERMEDIATES)) \
    $(_idfIntBase)/$(_idfClass)/$(_idfName)_obj \
)
endef

define local-intermediates-dir
$(strip \
    $(if $(strip $(LOCAL_MODULE_CLASS)),, \
        $(error $(LOCAL_PATH): LOCAL_MODULE_CLASS not defined before call to local-intermediates-dir)) \
    $(if $(strip $(LOCAL_MODULE)),, \
        $(error $(LOCAL_PATH): LOCAL_MODULE not defined before call to local-intermediates-dir)) \
    $(call intermediates-dir-for,$(LOCAL_MODULE_CLASS),$(LOCAL_MODULE),$(LOCAL_IS_HOST_MODULE)) \
)
endef

intermediates := $(call local-intermediates-dir, $(1))

define intermediateLibs-dir-for
$(strip \
    $(eval _idfClass := $(strip $(1))) \
    $(if $(_idfClass),, \
        $(error $(LOCAL_PATH): Class not defined in call to intermediates-dir-for)) \
    $(eval _idfName := $(strip $(2))) \
    $(if $(_idfName),, \
        $(error $(LOCAL_PATH): Name not defined in call to intermediates-dir-for)) \
    $(eval _idfPrefix := $(if $(strip $(3)),HOST,TARGET)) \
    $(eval _idfIntBase := $($(_idfPrefix)_OUT_INTERMEDIATES)) \
    $(_idfIntBase)/$(_idfClass)\
)
endef

define local-intermediateLibs-dir
$(strip \
    $(if $(strip $(LOCAL_MODULE_CLASS)),, \
        $(error $(LOCAL_PATH): LOCAL_MODULE_CLASS not defined before call to local-intermediates-dir)) \
    $(if $(strip $(LOCAL_MODULE)),, \
        $(error $(LOCAL_PATH): LOCAL_MODULE not defined before call to local-intermediates-dir)) \
    $(call intermediateLibs-dir-for,$(LOCAL_MODULE_CLASS),$(LOCAL_MODULE),$(LOCAL_IS_HOST_MODULE)) \
)
endef

intermediateLibs := $(call local-intermediateLibs-dir, $(1))

###########################################################
## Retrieve the directory of the current makefile
###########################################################

# Figure out where we are.
define my-dir
$(strip \
  $(eval LOCAL_MODULE_MAKEFILE := $$(lastword $$(MAKEFILE_LIST))) \
  $(if $(filter $(CLEAR_VARS),$(LOCAL_MODULE_MAKEFILE)), \
    $(error LOCAL_PATH must be set before including $$(CLEAR_VARS)) \
   , \
    $(patsubst %/,%,$(dir $(LOCAL_MODULE_MAKEFILE))) \
   ) \
 )
endef

###########################################################
## Retrieve a list of all makefiles immediately below some directory
###########################################################

define all-makefiles-under
$(wildcard $(1)/*/King.mk)
endef

###########################################################
## Commands for running armcc to compile a C file
###########################################################
# $(1): extra flags

REALPATH=$<

ifeq ($(strip $(COMPILER)),RVCT) 
define transform-c-to-o
@$(CPP_COMPILER) $(COMMON_GLOBAL_CFLAGS) --via $(intermediates)/macros/c-macros.txt --via $(intermediates)/macros/includes.txt $< -c -o $@ 
endef
else ifeq ($(strip $(COMPILER)), CSDTK)
define transform-c-to-o
	@${ECHO} "CPP_COMPILER                $*.c"
	@${ECHO} $(C_SPECIFIC_CFLAGS) $(LOCAL_C_INCLUDES_I) $(CFLAGS) $(CT_MIPS16_CFLAGS) $(MYCFLAGS) $(CPPFLAGS) > ${intermediates}/$*.opt
	$(CPP_COMPILER) -MT $@ -MD -MP @${intermediates}/$*.opt -o $@ $(REALPATH)
endef
else
define transform-c-to-o
@$(CPP_COMPILER) $(COMMON_GLOBAL_CFLAGS) $(LOCAL_C_INCLUDES_I) $< -c -o $@ 
endef
endif
###########################################################
## Commands for running armasm to compile an ASM file
###########################################################
ifeq ($(strip $(COMPILER)),RVCT)
define transform-s-to-o
@$(ASM_COMPILER) $(COMMON_GLOBAL_AFLAGS) $< -o $@ 
endef
else ifeq ($(strip $(COMPILER)), CSDTK)
define transform-s-to-o
	@${ECHO} "CPP               $*.S"
	$(CPP_COMPILER) -E $(CPPFLAGS) $(ASCPPFLAGS) -MT $@ -MD -MP -o ${intermediates}/$*.asm $(REALPATH)
	@${ECHO} "AS                $*.asm"
	$(ASM_COMPILER) $(ASFLAGS) -o $@ ${intermediates}/$*.asm
endef
else
define transform-s-to-o
@$(ASM_COMPILER) $(COMMON_GLOBAL_AFLAGS) $(LOCAL_C_INCLUDES_I) $< -o $@ 
endef
endif


