CC=g++
CFLAGS=-c -Wall -std=c++14 -O3 -I./include
LDFLAGS=
SOURCES=$(wildcard *.cpp) $(wildcard src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ccat

all: .depend $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

include .depend

depend: .depend

.depend: $(SOURCES)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^>>./.depend;

clean:
	find . -name \*.o -execdir rm {} \;
	rm -f .depend
	rm -f $(EXECUTABLE)

install: $(EXECUTABLE)
	sudo cp $(EXECUTABLE) /usr/bin
