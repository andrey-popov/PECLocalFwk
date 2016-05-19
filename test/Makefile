# Check if an installation path of PECFwk is provided
ifeq ($(MENSURA_INSTALL), )
  $(error Mandatory environment variable MENSURA_INSTALL is not set)
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
INCLUDE := -I$(MENSURA_INSTALL)/include -I$(shell root-config --incdir)
OPFLAGS := -O2
CFLAGS := -Wall -Wextra -std=c++14 $(INCLUDE) $(OPFLAGS)
LDFLAGS := $(shell root-config --libs) -lTreePlayer -lHistPainter \
 -L$(MENSURA_INSTALL)/lib -lmensura -lPECReader -Wl,-rpath=$(MENSURA_INSTALL)/lib \
 -L$(BOOST_ROOT)/lib -lboost_filesystem -Wl,-rpath=$(BOOST_ROOT)/lib


# Define example and test programs
EXAMPLES := minimal multithread
TESTS := btag-scale-factors btag-efficiencies lepton-scale-factors jet-met-systematics \
  jet-corrections


# Targets. By default only examples are cleaned
examples: $(EXAMPLES)
tests: $(TESTS)
.PHONY: clean


$(EXAMPLES) $(TESTS): %: %.cpp
	g++ $^ $(CFLAGS) $(LDFLAGS) -o $@


clean:
	rm -f $(EXAMPLES)
