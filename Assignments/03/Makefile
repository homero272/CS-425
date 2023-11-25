
SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)
TARGETS = $(SOURCES:.cpp=)

STD = -std=c++20
OPTIMIZER ?= -ofast
CXXINCS =
CXXFLAGS = $(STD) $(OPTIMIZER) $(CXXINCS)

default all: $(TARGETS)

%: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -o $@

clean clobber:
	$(RM) $(TARGETS)

.SUFFIXES:
.SUFFIXES: .h .cpp
.PHONY: Makefile default all clean clobber
