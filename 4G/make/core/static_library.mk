

###########################################################
## Common object handling.
###########################################################

include $(BUILD_SYSTEM)/Makefile_Macros.mk

ifeq ($(strip $(COMPILER)),RVCT)
TARGE_FILE:=$(LOCAL_MODULE).a
else
TARGE_FILE:=lib$(LOCAL_MODULE).a
endif

asm_sources_s := $(filter %.s,$(SOURCES))
asm_objects_s := $(addprefix $(intermediates)/,$(asm_sources_s:.s=.o))

asm_sources_S := $(filter %.S,$(SOURCES))
asm_objects_S := $(addprefix $(intermediates)/,$(asm_sources_S:.S=.o))

c_sources := $(filter %.c,$(SOURCES))
c_objects := $(addprefix $(intermediates)/,$(c_sources:.c=.o))

cpp_sources := $(filter %.cpp,$(SOURCES))
cpp_objects := $(addprefix $(intermediates)/,$(cpp_sources:.cpp=.o))

# some rules depend on asm_objects being first.  If your code depends on
# being first, it's reasonable to require it to be assembly
asm_objects := $(asm_objects_S) $(asm_objects_s)
c_objects	 := $(c_objects)
cpp_objects	   := $(cpp_objects)

all_objects := \
    $(asm_objects) \
    $(c_objects) \
    $(cpp_objects)



$(TARGE_FILE): $(all_objects) $(intermediateLibs) $(intermediates)
	@$(ECHO) $(all_objects) > $(intermediates)/obj_list.txt
ifeq ($(strip $(COMPILER)),RVCT) 
	@$(LIB_PACKER) $(COMMON_LIB_PACKER_FLAGS) $(intermediateLibs)/$(TARGE_FILE) --via $(intermediates)/obj_list.txt
else
	@$(LIB_PACKER) $(COMMON_LIB_PACKER_FLAGS) $(intermediateLibs)/$(TARGE_FILE) $(all_objects)
endif

ifneq ($(strip $(all_objects)),)
$(intermediates): DESTINATION_DIR:=$(subst /,\,$(intermediates))
$(intermediates): 
	@if not exist $(DESTINATION_DIR) mkdir $(DESTINATION_DIR)
	@if not exist $(DESTINATION_DIR)\macros mkdir $(DESTINATION_DIR)\macros
	@$(foreach filepath,$(SOURCES), \
		$(if $(findstring /,$(filepath)), \
			$(shell if not exist $(subst /,\,$(DESTINATION_DIR)/$(dir $(filepath))) \
			mkdir $(subst /,\,$(DESTINATION_DIR)/$(dir $(filepath))))\
		) \
	)
ifneq ($(strip $(LOCAL_COMPILER_VARIABLES)), $(EMPTY))
	@$(ECHO) $(addprefix -D,$(LOCAL_COMPILER_VARIABLES))>$(DESTINATION_DIR)\macros\c-macros.txt
else
	@type nul >$(DESTINATION_DIR)\macros\c-macros.txt
endif
	@$(ECHO) $(addprefix -D,$(GLOBLE_COMPILER_FLAGS))>>$(DESTINATION_DIR)\macros\c-macros.txt
	@$(ECHO) -I $(LOCAL_C_INCLUDES) > $(DESTINATION_DIR)\macros\includes.txt
endif

$(intermediateLibs):LIB_DESTINATION_DIR:=$(subst /,\,$(intermediateLibs))
$(intermediateLibs):
	@if not exist $(LIB_DESTINATION_DIR) mkdir $(LIB_DESTINATION_DIR)

ifneq ($(strip $(asm_objects_S)),)
$(asm_objects_S): $(intermediates)/%.o: $(TOPDIR)Source/$(LOCAL_MODULE)/$(LOCAL_SRC_DIR)/%.S $(intermediates)
	@$(ECHO) Compiling ASM $<...
	$(transform-s-to-o)
-include $(asm_objects_S:%.o=%.P)
endif

ifneq ($(strip $(asm_objects_s)),)
$(asm_objects_s): $(intermediates)/%.o: $(TOPDIR)Source/$(LOCAL_MODULE)/$(LOCAL_SRC_DIR)/%.s $(intermediates)
	@$(ECHO) Compiling ASM $<...
	$(transform-s-to-o)
-include $(asm_objects_S:%.o=%.P)
endif

ifneq ($(strip $(c_objects)),)
$(c_objects): $(intermediates)/%.o: $(TOPDIR)Source/$(LOCAL_MODULE)/$(LOCAL_SRC_DIR)/%.c $(intermediates)
	@$(ECHO) Compiling C $<...
	$(transform-c-to-o)
-include $(c_objects:%.o=%.P)
endif

ifneq ($(strip $(cpp_objects)),)
$(cpp_objects): $(intermediates)/%.o: $(TOPDIR)Source/$(LOCAL_MODULE)/$(LOCAL_SRC_DIR)/%.cpp $(intermediates)
	@$(ECHO) Compiling CPP $<...
	$(transform-c-to-o)
-include $(cpp_objects:%.o=%.P)
endif


