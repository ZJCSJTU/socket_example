LINK.o = $(LINK.cc)
LDFLAGS=-std=c++11
CXXFLAGS=-g3 -O0 -Wall -Werror -pedantic -std=c++11

TARGETS=server client

.PHONY: all clean

all: $(TARGETS)

%: %.cpp

clean: 
	rm -f $(TARGETS) *.o

