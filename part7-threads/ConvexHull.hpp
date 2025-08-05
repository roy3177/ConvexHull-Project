/*

@author Roy Maoded
@author Yarin Keshet
@date 2025-08-05

In Part 7, we upgraded the Convex Hull server to support multiple clients simultaneously using threads.
Each time a new client connects, the server spawns a new thread to handle that client’s session independently.
To ensure thread-safe access to shared data (like the unordered_map holding client points), we used a mutex lock.
This prevents race conditions when multiple threads access or modify the shared structure concurrently.
This part demonstrates:
Efficient use of POSIX threads
Safe concurrent programming with mutex protection
A scalable architecture where each client is isolated in its own thread
Now, the server can compute convex hulls for multiple clients in parallel, with correct and
synchronized communication.
*/



#pragma once
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>

float CHArea_vec(const std::vector<std::pair<float, float>>& points);
