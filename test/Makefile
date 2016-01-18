# Check if an installation path of PECFwk is provided
ifeq ($(PEC_FWK_INSTALL), )
  $(error Mandatory environment variable PEC_FWK_INSTALL is not set)
endif

# Make sure ROOT is available
ifeq ($(shell which root-config), )
  $(error ROOT installation is not found)
endif


# Set Boost variables
# A bit of special setup is required for lxplus
ifeq ($(shell hostname | grep -q lxplus; echo $$?), 0)
  ifeq ($(BOOST_ROOT), )
    # Set the path to Boost appropriate for lxplus
    BOOST_ROOT = /afs/cern.ch/sw/lcg/external/Boost/1.50.0_python2.7/x86_64-slc5-gcc46-opt
  endif
  
  BOOST_INCLUDE = $(BOOST_ROOT)/include/boost-1_50
  BOOST_LIB_POSTFIX = -gcc46-mt-1_50
else
  ifeq ($(BOOST_ROOT), )
    $(error Mandatory environment variable BOOST_ROOT is not set)
  endif
  
  BOOST_INCLUDE = $(BOOST_ROOT)/include
  BOOST_LIB_POSTFIX = 
endif

BOOST_LIB = $(BOOST_ROOT)/lib


# Flags to control compilation and linking
INCLUDE = -I$(PEC_FWK_INSTALL)/core/include -I$(PEC_FWK_INSTALL)/extensions/include \
 -I$(shell root-config --incdir)
OPFLAGS = -O2
CFLAGS = -Wall -Wextra -std=c++14 $(INCLUDE) $(OPFLAGS)
LDFLAGS = $(shell root-config --libs) -lTreePlayer -lHistPainter \
 -L$(BOOST_LIB) -lboost_filesystem$(BOOST_LIB_POSTFIX) $(PEC_FWK_INSTALL)/lib/libpecfwk.a \
 -Wl,-rpath=$(BOOST_LIB)


# Define the phony targets
.PHONY: clean


# The default rule
all: minimal multithread


minimal: minimal.cpp $(PEC_FWK_INSTALL)/lib/libpecfwk.a
	@ g++ $< $(CFLAGS) $(LDFLAGS) -o $@


multithread: multithread.cpp $(PEC_FWK_INSTALL)/lib/libpecfwk.a
	@ g++ $< $(CFLAGS) $(LDFLAGS) -o $@


clean:
	@ rm -f minimal multithread
