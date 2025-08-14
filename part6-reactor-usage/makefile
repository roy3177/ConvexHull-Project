CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: server client

server: server_reactor.cpp CH_vec.cpp Reactor.cpp
	$(CXX) $(CXXFLAGS) server_reactor.cpp CH_vec.cpp Reactor.cpp -o server

client: client.cpp
	$(CXX) $(CXXFLAGS) client.cpp -o client

clean:
	rm -f server client
