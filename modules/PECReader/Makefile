# Standard definitions
include $(PEC_FWK_INSTALL)/Makefile.inc


# Paths to source and object files
SOURCE_DIR := src
OBJ_DIR := obj
OBJECTS := $(shell for f in `find $(SOURCE_DIR) -regex ".*\.cpp"`; \
	do echo $(OBJ_DIR)/`basename $$f .cpp`.o; done)
MODULE_NAME := $(shell basename `pwd`)
LIB_DIR := lib
LIB_NAME := lib$(MODULE_NAME).so
LIB_PATH := $(LIB_DIR)/$(LIB_NAME)

vpath %.cpp $(SOURCE_DIR)


.PHONY: clean


# Building rules
all: $(LIB_PATH)

$(LIB_PATH): $(OBJECTS)
	@ mkdir -p $(LIB_DIR)
	@ rm -f $@
	@ $(CC) -shared -Wl,-soname,$(LIB_NAME).3 -o $@.3.0 $+
	@ ln -sf $(LIB_NAME).3.0 $@.3; ln -sf $(LIB_NAME).3 $@

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@ mkdir -p $(OBJ_DIR)
	@ $(CC) $(CFLAGS) -c $< -o $@

$(SOURCE_DIR)/classes_rflx.cpp: $(SOURCE_DIR)/classes.h $(SOURCE_DIR)/classes_def.xml
	@ genreflex classes.h -s classes_def.xml -I$(PEC_FWK_INSTALL)/include/

clean:
	@ rm -rf $(OBJ_DIR) $(LIB_DIR)
