CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra



all: server client

server: server_threads.cpp CH_vec.cpp
	$(CXX) $(CXXFLAGS) server_threads.cpp CH_vec.cpp -o server


client: client.cpp
	$(CXX) $(CXXFLAGS) client.cpp -o client


clean:
	rm -f server client
