CXX=g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fprofile-arcs -ftest-coverage
LDFLAGS = -lgcov


all: main

main: main.cpp ConvexHull.cpp
	$(CXX) $(CXXFLAGS)  -o main  main.cpp ConvexHull.cpp -o main $(LDFLAGS)
clean:
	rm -f main *.gcno *.gcda *.gcov
