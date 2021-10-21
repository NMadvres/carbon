CXX = g++
CXXFLAGS = -g -Wall -Werror
LDFLAGS =
LDLIBS =

# systemc setup
CXXFLAGS += $(shell pkg-config --cflags          systemc)
LDFLAGS  += $(shell pkg-config --libs-only-L     systemc)
LDLIBS   += $(shell pkg-config --libs-only-other systemc)
LDLIBS   += $(shell pkg-config --libs-only-l     systemc)

# add include path
CXXFLAGS += -I./src -I./tb/src

# generate depends rule
CXXFLAGS += -MD

SOURCES = ./src/comm_def.cpp \
		  ./src/main.cpp \
		  ./src/mod_egr.cpp \
		  ./src/mod_ing.cpp \
		  ./src/mod_pe.cpp \
		  ./src/mod_sch.cpp \
		  ./tb/src/mod_stat.cpp \
		  ./tb/src/mod_stim.cpp \
		  ./tb/src/mod_testctrl.cpp

OBJECTS = $(SOURCES:.cpp=.o)
DEPENDS = $(SOURCES:.cpp=.d)
APPNAME = carbon

-include $(DEPENDS)

all: $(APPNAME)

format:
	clang-format -i ./src/*.h
	clang-format -i ./tb/src/*.h
	clang-format -i $(SOURCES)

$(APPNAME): $(OBJECTS)
	$(CXX) -o $(APPNAME) $(OBJECTS) $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJECTS)
	$(RM) $(DEPENDS)
	$(RM) $(APPNAME)

.PHONY: all clean
