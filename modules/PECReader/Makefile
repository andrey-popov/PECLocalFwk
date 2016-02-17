# Standard definitions
include $(PEC_FWK_INSTALL)/Makefile.inc


# Paths to source and object files
MODULE_NAME := $(shell basename `pwd`)

LIB_DIR := lib
LIB_NAME := lib$(MODULE_NAME).so
LIB_PATH := $(LIB_DIR)/$(LIB_NAME)

DICT_CPP_NAME := dictPEC
DICT_PCM_NAME := lib$(MODULE_NAME)_rdict.pcm
DICT_PATH := $(LIB_DIR)/$(DICT_PCM_NAME)

SOURCE_DIR := src
OBJ_DIR := obj
OBJECTS := $(shell for f in `find $(SOURCE_DIR) -regex ".*\.cpp"`; \
	do echo $(OBJ_DIR)/`basename $$f .cpp`.o; done)

vpath %.cpp $(SOURCE_DIR)


.PHONY: clean


# Building rules
all: $(LIB_PATH) $(DICT_PATH)

$(LIB_PATH): $(OBJECTS) $(OBJ_DIR)/$(DICT_CPP_NAME).o
	@ mkdir -p $(LIB_DIR)
	@ rm -f $@
	@ $(CC) -shared -Wl,-soname,$(LIB_NAME).3 -o $@.3.0 $^
    #^ It is important to use there $^ instead of $+ since $(DICT_CPP_NAME).o can occur twice among
    # the dependencies.
	@ ln -sf $(LIB_NAME).3.0 $@.3; ln -sf $(LIB_NAME).3 $@

$(DICT_PATH): $(SOURCE_DIR)/$(DICT_CPP_NAME).cpp
	@ mkdir -p $(LIB_DIR)
	@ mv $(SOURCE_DIR)/$(DICT_PCM_NAME) $@

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@ mkdir -p $(OBJ_DIR)
	@ $(CC) $(CFLAGS) -c $< -o $@

$(SOURCE_DIR)/$(DICT_CPP_NAME).cpp: $(SOURCE_DIR)/classes.h $(SOURCE_DIR)/classes_def.xml
	@ cd $(SOURCE_DIR); \
		rootcling -f $(DICT_CPP_NAME).cpp -s $(LIB_NAME) -I$(PEC_FWK_INSTALL)/include \
		classes.h classes_def.xml
	@ touch -r $(SOURCE_DIR)/$(DICT_CPP_NAME).cpp $(SOURCE_DIR)/$(DICT_PCM_NAME)
    #^ In the first command of the recipe, rootcling produces cpp and pcm files. The $(DICT_PATH)
    # target naturally depends on the pcm file, while $(DICT_NAME).o depends on the cpp file.
    # However, GNU make is not capable of supporting multiple targets properly [1], and because of
    # this $(DICT_PATH) is declared depending on the cpp file. The problem is that this file is
    # created after the pcm one. In order to avoid rerunning recipe for $(DICT_PATH) every time,
    # the time stamp of the pcm file is updated.
    # [1] http://stackoverflow.com/questions/2973445/gnu-makefile-rule-generating-a-few-targets-from-a-single-source-file

clean:
	@ rm -rf $(OBJ_DIR) $(LIB_DIR)
	@ rm -f $(SOURCE_DIR)/dictPEC.cpp $(wildcard $(SOURCE_DIR)/*_rdict.pcm)
