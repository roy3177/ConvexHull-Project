CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
OBJS = main.o Reactor.o

all: main

main: $(OBJS)
	$(CXX) $(CXXFLAGS) -o main $(OBJS)

main.o: main.cpp Reactor.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

Reactor.o: Reactor.cpp Reactor.hpp
	$(CXX) $(CXXFLAGS) -c Reactor.cpp

clean:
	rm -f main *.o
