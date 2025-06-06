CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = 

all: main

main: main.cpp CH_vec.cpp CH_deque.cpp CH_list.cpp
	$(CXX) $(CXXFLAGS) main.cpp CH_vec.cpp CH_deque.cpp CH_list.cpp -o main $(LDFLAGS)

clean:
	rm -f main *.o *.gcno *.gcda *.gcov
	
.PHONY: all clean