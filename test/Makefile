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


# Flags to control compilation and linking
INCLUDE = -I$(PEC_FWK_INSTALL)/include -I$(shell root-config --incdir)
OPFLAGS = -O2
CFLAGS = -Wall -Wextra -std=c++14 $(INCLUDE) $(OPFLAGS)
LDFLAGS = $(shell root-config --libs) -lTreePlayer -lHistPainter \
 -L$(PEC_FWK_INSTALL)/lib -lPECFwk -Wl,-rpath=$(PEC_FWK_INSTALL)/lib \
 -L$(BOOST_ROOT)/lib -lboost_filesystem -Wl,-rpath=$(BOOST_ROOT)/lib


# Define the phony targets
.PHONY: clean


# The default rule
all: minimal multithread


minimal: minimal.cpp
	@ g++ $< $(CFLAGS) $(LDFLAGS) -o $@


multithread: multithread.cpp
	@ g++ $< $(CFLAGS) $(LDFLAGS) -o $@


clean:
	@ rm -f minimal multithread
