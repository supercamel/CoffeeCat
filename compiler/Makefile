CC=g++-4.9
CFLAGS=-c -Wall -std=c++14 -O3 -I./include
LDFLAGS=
SOURCES=$(wildcard *.cpp) $(wildcard src/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ccat

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	find . -name \*.o -execdir rm {} \;
	rm -f $(EXECUTABLE)

install: $(EXECUTABLE)
	sudo cp $(EXECUTABLE) /usr/bin
