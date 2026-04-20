# cs530 assignment 2 - lxe assembler
#Team: Alan Chavarin (cssc2513, redid 827690364)
# Amir Ali (cssc2503, redid 132395455)

EXEC = lxe
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -g

SOURCES = main.cpp parser.cpp optab.cpp symtab.cpp pass1.cpp pass2.cpp listing.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJECTS) $(EXEC)

.PHONY: all clean
