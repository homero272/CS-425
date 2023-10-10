#! make

SOURCE = $(wildcard *.cpp)
TARGETS = $(SOURCE:.cpp=)
HEADERS = Number.h

STD = -std=c++20
OPTIMIZER = -O3
CXXFLAGS = $(STD) $(OPTIMIZER)

default: $(TARGETS)

clean clobber:
	$(RM) $(TARGETS)