INCLUDE = -I$(PEC_FWK_INSTALL)/core/include -I$(PEC_FWK_INSTALL)/extensions/include -I$(shell root-config --incdir)
OPFLAGS = -O2
CFLAGS = -Wall -Wextra -std=c++11 $(INCLUDE) $(OPFLAGS) $(MACROS)
LDFLAGS = $(shell root-config --libs) -lTreePlayer -lHistPainter $(PEC_FWK_INSTALL)/lib/libpecfwk.a

all: minimal multithread

minimal: minimal.cpp $(PEC_FWK_INSTALL)/lib/libpecfwk.a
	@ g++ $< $(CFLAGS) $(LDFLAGS) -o $@

multithread: multithread.cpp $(PEC_FWK_INSTALL)/lib/libpecfwk.a
	@ g++ $< $(CFLAGS) $(LDFLAGS) -o $@
