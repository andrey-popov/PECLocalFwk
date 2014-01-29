BOOSTDIR = /afs/cern.ch/sw/lcg/external/Boost/1.50.0_python2.7/x86_64-slc5-gcc46-opt

INCLUDE = -I$(PEC_FWK_INSTALL)/core/include -I$(PEC_FWK_INSTALL)/extensions/include -I$(shell root-config --incdir)
OPFLAGS = -O2
CFLAGS = -Wall -Wextra -std=c++11 $(INCLUDE) $(OPFLAGS) $(MACROS)
LDFLAGS = $(shell root-config --libs) -lTreePlayer -lHistPainter \
 -L$(BOOSTDIR)/lib/ -lboost_filesystem-gcc46-mt-1_50 $(PEC_FWK_INSTALL)/lib/libpecfwk.a \
 -Wl,-rpath=$(BOOSTDIR)/lib/

all: minimal multithread

minimal: minimal.cpp $(PEC_FWK_INSTALL)/lib/libpecfwk.a
	@ g++ $< $(CFLAGS) $(LDFLAGS) -o $@

multithread: multithread.cpp $(PEC_FWK_INSTALL)/lib/libpecfwk.a
	@ g++ $< $(CFLAGS) $(LDFLAGS) -o $@
