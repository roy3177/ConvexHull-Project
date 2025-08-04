CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -pthread

SERVER_OBJS = server_proactor.o Proactor.o CH_vec.o
CLIENT_OBJS = client.o

all: server client

server: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o server $(SERVER_OBJS)

client: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o client $(CLIENT_OBJS)

server_proactor.o: server_proactor.cpp Proactor.hpp ConvexHull.hpp
	$(CXX) $(CXXFLAGS) -c server_proactor.cpp

Proactor.o: Proactor.cpp Proactor.hpp
	$(CXX) $(CXXFLAGS) -c Proactor.cpp

CH_vec.o: CH_vec.cpp ConvexHull.hpp
	$(CXX) $(CXXFLAGS) -c CH_vec.cpp

client.o: client.cpp
	$(CXX) $(CXXFLAGS) -c client.cpp

clean:
	rm -f server client *.o
