# Flags to control compilation and linking
CC = g++
INCLUDE = -I$(PEC_FWK_INSTALL)/include -I$(PEC_FWK_INSTALL) -I$(shell root-config --incdir) -I$(BOOST_ROOT)/include
OPFLAGS = -O2
CFLAGS = -Wall -Wextra -Wno-unused-function -fPIC -std=c++14 $(INCLUDE) $(OPFLAGS)
