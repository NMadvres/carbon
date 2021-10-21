CXX = g++
CXXFLAGS = -g -Wall -Werror
LDFLAGS =
LDLIBS =

# systemc setup
CXXFLAGS += $(shell pkg-config --cflags          systemc)
LDFLAGS  += $(shell pkg-config --libs-only-L     systemc)
LDLIBS   += $(shell pkg-config --libs-only-other systemc)
LDLIBS   += $(shell pkg-config --libs-only-l     systemc)

CXXFLAGS += -I./src -I./tb/src

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
APPNAME = carbon

all: $(APPNAME)

format:
	clang-format -i ./src/*.h
	cflag-format -i ./tb/src/*.h
	clang-format -i $(SOURCES)

$(APPNAME): $(OBJECTS)
	$(CXX) -o $(APPNAME) $(OBJECTS) $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJECTS) $(APPNAME)

.PHONY: all clean
