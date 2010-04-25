CXX = g++
CXXFLAGS = -Wall -g -fpic
#CXXFLAGS = -Wall -g -O2 -fpic
INCLUDES = -I../include
LDFLAGS = -shared
LIBS = 

.SUFFIXES: .cpp .o

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean

all: StringPlugin.so EnvRestore.so

StringPlugin.so: StringPlugin.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ StringPlugin.o $(LDFLAGS) $(LIBS)

EnvRestore.so: EnvRestore.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ EnvRestore.o $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o *~ *.so
