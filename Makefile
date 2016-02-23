# Check if the installation path of PECFwk is provided
ifeq ($(PEC_FWK_INSTALL), )
  $(error Mandatory environment variable PEC_FWK_INSTALL is not set)
endif

# Make sure ROOT is available
ifeq ($(shell which root-config), )
  $(error ROOT installation is not found)
endif

# Make sure Boost is available
ifeq ($(BOOST_ROOT), )
  $(error Mandatory environment variable BOOST_ROOT is not set)
endif


# Include standard definitions
include Makefile.inc


# Information about individual modules in the project. Some of them are built into static libraries
# that are then combined in a shared library. But some are built into independent shared libraries.
MODULES_DIR := modules

MODULES_STATIC := core extensions
MODULE_STATIC_LIBS := $(shell for m in $(MODULES_STATIC); \
	do echo $(MODULES_DIR)/$$m/lib/$$m.a; done)
MODULES_STATIC += external/JERC external/BTagCalibration
MODULE_STATIC_LIBS += $(MODULES_DIR)/external/JERC/lib/JERC.a
MODULE_STATIC_LIBS += $(MODULES_DIR)/external/BTagCalibration/lib/BTagCalibration.a

MODULES_SHARED := PECReader
MODULE_SHARED_LIBS := $(shell for m in $(MODULES_SHARED); \
	do echo $(MODULES_DIR)/$$m/lib/lib$$m.so; done)

MODULES := $(MODULES_STATIC) $(MODULES_SHARED)

LIB_DIR := lib
# Library built from modules that do not produce shared libraries on their own
MAIN_LIB_NAME := libPECFwk.so
MAIN_LIB_PATH := $(LIB_DIR)/$(MAIN_LIB_NAME)


# Define phony targets
.PHONY: $(MODULES) link-libs unpack clean


# Building rules
all: $(MAIN_LIB_PATH) link-libs unpack

$(MAIN_LIB_PATH): $(MODULES_STATIC)
	@ mkdir -p $(LIB_DIR)
	@ rm -f $@
	@ $(CC) -shared -Wl,-soname,$(MAIN_LIB_NAME).4 -o $@.4.0 \
		-Wl,--whole-archive $(MODULE_STATIC_LIBS) -Wl,--no-whole-archive
	@ ln -sf $(MAIN_LIB_NAME).4.0 $@.4; ln -sf $(MAIN_LIB_NAME).4 $@

link-libs: $(MODULES_SHARED)
	@ mkdir -p $(LIB_DIR)
	@ cd $(LIB_DIR); for m in $(MODULES_SHARED); \
		do for f in `find $(PEC_FWK_INSTALL)/$(MODULES_DIR)/$$m/lib/ -regex ".*/lib.*\.so.*$$"`; \
			do ln -sf $$f .; done \
		done
	@ cd $(LIB_DIR); ln -sf $(wildcard $(PEC_FWK_INSTALL)/$(MODULES_DIR)/PECReader/lib/*.pcm) .

$(MODULES):
	@ +make -s -C $(MODULES_DIR)/$@

unpack:
	@ if [ `ls data/JERC/ | grep AK5PFchs.txt | wc -l` -eq 0 ]; \
	 then tar -xzf data/JERC/Summer13_V5_AK5PFchs.tar.gz -C data/JERC/; fi

clean:
	@ for m in $(MODULES); do make -s -C $(MODULES_DIR)/$$m clean; done
