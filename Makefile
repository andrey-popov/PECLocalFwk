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


# Information about individual modules in the project
MODULES_PATH := modules
MODULES := core extensions PECReader
MODULE_LIBS := $(shell for m in $(MODULES); do echo $(MODULES_PATH)/$$m/lib/$$m.a; done)

MODULES += external/JERC
MODULE_LIBS += $(MODULES_PATH)/external/JERC/lib/JERC.a


# Define phony targets
.PHONY: clean $(MODULES)


# Building rules
all: libpecfwk.so unpack

libpecfwk.so: $(MODULES)
	@ mkdir -p lib
	@ rm -f lib/$@
	@ $(CC) -shared -Wl,-soname,$@.4 -o $@.4.0 \
		-Wl,--whole-archive $(MODULE_LIBS) -Wl,--no-whole-archive
	@ mv $@.4.0 lib/
	@ ln -sf $@.4.0 lib/$@.4; ln -sf $@.4 lib/$@

$(MODULES):
	@ +make -s -C $(MODULES_PATH)/$@

unpack:
	@ if [ `ls data/JERC/ | grep AK5PFchs.txt | wc -l` -eq 0 ]; \
	 then tar -xzf data/JERC/Summer13_V5_AK5PFchs.tar.gz -C data/JERC/; fi

clean:
	@ for m in $(MODULES); do make -s -C $(MODULES_PATH)/$$m clean; done
