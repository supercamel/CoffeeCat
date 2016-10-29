CC=g++
CFLAGS=-c -Wall -Wextra -std=c++14 -fwrapv -O3 -I../
LDFLAGS= -letk
COFFEECAT=$(wildcard *.cof)
SOURCES=$(patsubst %.cof,%.cpp,$(COFFEECAT)) $(wildcard ../stdlib/*.cpp)
OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))
EXECUTABLE=main

all: $(COFFEECAT) $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@ -lc

%.o:%.cpp
	$(CC) $(CFLAGS) $< -o $@

$(SOURCES): $(COFFEECAT)
	ccat -o ./ $(COFFEECAT)

clean:
	find . -name \*.o -execdir rm {} \;
	rm *.cpp
	rm *.h
	rm -f $(EXECUTABLE)
