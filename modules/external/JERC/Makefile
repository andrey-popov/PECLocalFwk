# Standard definitions
include $(PEC_FWK_INSTALL)/Makefile.inc


# Paths to source and object files
SOURCE_PATH = src
OBJ_PATH = obj
OBJECTS = $(shell for f in `find $(SOURCE_PATH) -regex ".*\.cpp"`; \
	do echo $(OBJ_PATH)/`basename $$f .cpp`.o; done)
MODULE_NAME = $(shell basename `pwd`)
LIB_PATH = lib


# Building rules
all: $(LIB_PATH)/$(MODULE_NAME).a

$(LIB_PATH)/$(MODULE_NAME).a: $(OBJECTS)
	@ mkdir -p $(LIB_PATH)
	@ rm -f $@
	@ ar -cq $@ $+

$(OBJ_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	@ mkdir -p $(OBJ_PATH)
	@ $(CC) $(CFLAGS) -c $< -o $@
