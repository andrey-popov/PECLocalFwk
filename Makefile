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
INCLUDE = -Icore/include -Iextensions/include -I./ -I$(shell root-config --incdir) -I$(BOOST_INCLUDE)
OPFLAGS = -O2
CFLAGS = -Wall -Wextra -Wno-unused-function -fPIC -std=c++14 $(INCLUDE) $(OPFLAGS)
#LDFLAGS = $(shell root-config --libs) -lTreePlayer -lHistPainter \
# -L$(BOOST_LIB) -lboost_filesystem$(BOOST_LIB_POSTFIX) $(PEC_FWK_INSTALL)/lib/libpecfwk.a \
# -Wl,-rpath=$(BOOST_LIB)


# Define where the object files should be located
MODULE_PATHS = core extensions $(shell ls -d external/*)
OBJPATH = obj
OBJECTS = $(shell for d in $(MODULE_PATHS); \
 do  for f in `ls $$d/src/ | grep .cpp`; do echo $(OBJPATH)/`basename $$f .cpp`.o; done; done)

vpath %.cpp $(addsuffix /src/,$(MODULE_PATHS))
vpath %.o $(OBJPATH)


# Define phony targets
.PHONY: clean


# The default rule
all: libpecfwk.a libpecfwk.so unpack


libpecfwk.a: $(OBJECTS)
	@ mkdir -p lib
	@ rm -f lib/$@
	@ ar -cq lib/$@ $+
# '$@' is expanded to the target, '$+' expanded to all the dependencies. See
# http://www.gnu.org/savannah-checkouts/gnu/make/manual/html_node/Automatic-Variables.html


libpecfwk.so: $(OBJECTS)
	@ mkdir -p lib
	@ rm -f lib/$@
	@ $(CC) -shared -Wl,-soname,$@.1 -o $@.1.0 $+
	@ mv $@.1.0 lib/
	@ ln -sf $@.1.0 lib/$@
	

$(OBJPATH)/%.o: %.cpp
	@ mkdir -p $(OBJPATH)
	@ $(CC) $(CFLAGS) -c $< -o $@
# '$<' is expanded to the first dependency


unpack:
	@ if [ `ls data/JERC/ | grep AK5PFchs.txt | wc -l` -eq 0 ]; \
	 then tar -xzf data/JERC/Summer13_V5_AK5PFchs.tar.gz -C data/JERC/; fi


clean:
	@ rm -f $(OBJPATH)/*
# '@' prevents the command from being printed to stdout
