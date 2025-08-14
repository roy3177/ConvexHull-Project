CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
OBJS = main.o Proactor.o

all: main

main: $(OBJS)
	$(CXX) $(CXXFLAGS) -o main $(OBJS)

main.o: main.cpp Proactor.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

Proactor.o: Proactor.cpp Proactor.hpp
	$(CXX) $(CXXFLAGS) -c Proactor.cpp

clean:
	rm -f main *.o
