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
