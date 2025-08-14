CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -pthread

SERVER_OBJS = server.o Proactor.o CH_vec.o area_watcher.o
CLIENT_OBJS = client.o

all: server client

server: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o server $(SERVER_OBJS)

client: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o client $(CLIENT_OBJS)

server_proactor.o: server.cpp Proactor.hpp ConvexHull.hpp area_watcher.cpp
	$(CXX) $(CXXFLAGS) -c server.cpp

area_watcher.o: area_watcher.cpp area_watcher.hpp
	$(CXX) $(CXXFLAGS) -c area_watcher.cpp

Proactor.o: Proactor.cpp Proactor.hpp
	$(CXX) $(CXXFLAGS) -c Proactor.cpp

CH_vec.o: CH_vec.cpp ConvexHull.hpp
	$(CXX) $(CXXFLAGS) -c CH_vec.cpp

client.o: client.cpp
	$(CXX) $(CXXFLAGS) -c client.cpp

clean:
	rm -f server client *.o
