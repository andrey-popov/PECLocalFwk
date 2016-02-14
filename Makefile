# Check if an installation path of PECFwk is provided
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


# Setup variables to use Boost libraries
BOOST_INCLUDE = $(BOOST_ROOT)/include
BOOST_LIB = $(BOOST_ROOT)/lib
BOOST_LIB_POSTFIX = 


# Flags to control compilation and linking
CC = g++
INCLUDE = -Iinclude -I. -I$(shell root-config --incdir) -I$(BOOST_INCLUDE)
OPFLAGS = -O2
CFLAGS = -Wall -Wextra -Wno-unused-function -fPIC -std=c++14 $(INCLUDE) $(OPFLAGS)


# Define where the object files should be located
SOURCE_PATHS = src/core src/extensions $(shell for p in `ls -d external/*`; do echo $$p/src; done)
OBJPATH = obj
OBJECTS = $(shell for d in $(SOURCE_PATHS); \
 do  for f in `ls $$d/ | grep .cpp`; do echo $(OBJPATH)/`basename $$f .cpp`.o; done; done)

vpath %.cpp $(SOURCE_PATHS)
vpath %.o $(OBJPATH)


# Define phony targets
.PHONY: clean


# The default rule
all: libpecfwk.so unpack


libpecfwk.so: $(OBJECTS)
	@ mkdir -p lib
	@ rm -f lib/$@
	@ $(CC) -shared -Wl,-soname,$@.4 -o $@.4.0 $+
	@ mv $@.4.0 lib/
	@ ln -sf $@.4.0 lib/$@.4; ln -sf $@.4 lib/$@
	

$(OBJPATH)/%.o: %.cpp
	@ mkdir -p $(OBJPATH)
	@ $(CC) $(CFLAGS) -c $< -o $@


unpack:
	@ if [ `ls data/JERC/ | grep AK5PFchs.txt | wc -l` -eq 0 ]; \
	 then tar -xzf data/JERC/Summer13_V5_AK5PFchs.tar.gz -C data/JERC/; fi


clean:
	@ rm -f $(OBJPATH)/*
