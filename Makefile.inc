# Flags to control compilation and linking
CC = g++
INCLUDE = -I$(MENSURA_INSTALL)/include -I$(MENSURA_INSTALL) -I$(shell root-config --incdir) -I$(BOOST_ROOT)/include
OPFLAGS = -O2
CFLAGS = -Wall -Wextra -Wno-unused-function -fPIC -std=c++17 $(INCLUDE) $(OPFLAGS)
