# Standard definitions
include $(MENSURA_INSTALL)/Makefile.inc


# Paths to source and object files
SOURCE_DIR := src
OBJ_DIR := obj
OBJECTS := $(shell for f in `find $(SOURCE_DIR) -regex ".*\.cpp"`; \
	do echo $(OBJ_DIR)/`basename $$f .cpp`.o; done)
MODULE_NAME := $(shell basename `pwd`)
LIB_DIR := lib
LIB_PATH := $(LIB_DIR)/$(MODULE_NAME).a

vpath %.cpp $(SOURCE_DIR)


.PHONY: clean


# Building rules
all: $(LIB_PATH)

$(LIB_PATH): $(OBJECTS)
	@ mkdir -p $(LIB_DIR)
	@ rm -f $@
	@ ar -cq $@ $+

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@ mkdir -p $(OBJ_DIR)
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm -rf $(OBJ_DIR) $(LIB_DIR)
