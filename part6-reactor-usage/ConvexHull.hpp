
/*

@author Roy Maoded
@author Yarin Keshet
@date 2025-08-05

In this stage, the server uses a Reactor to handle multiple clients concurrently without creating a
new thread for each client. Instead of blocking I/O, the reactor uses a single event loop to monitor
file descriptors (sockets) and trigger appropriate callbacks when they are ready. 
This is more efficient and scalable than spawning threads for every connection. 
The main server socket is monitored for new connections, and when a client connects, its socket is added 
to the reactor to handle commands asynchronously.
*/




#pragma once
#include <vector>
#include <deque>
#include <list>
#include <utility>
#include <algorithm>
#include <cmath>

float CHArea_vec(const std::vector<std::pair<float, float>>& points);
