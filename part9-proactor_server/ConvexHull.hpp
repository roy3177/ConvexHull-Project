/*
@author Roy Maoded
@author Yarin Keshet
@date 2025-08-05

In this part, we implemented a multi-threaded Convex Hull server using the Proactor pattern.
Each client connection is handled by a dedicated thread using select() to detect input readiness.
The server supports multiple clients simultaneously and allows each client to manage their 
own graph (set of 2D points).
Supported commands:
Newgraph N – start a new graph with N points.
Newpoint x,y – add a point.
Removepoint x,y – remove a point.
CH – compute the convex hull area.
exit – disconnect from the server.
Thread-safe access to shared data is ensured using a mutex.
*/


#pragma once
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>

float CHArea_vec(const std::vector<std::pair<float, float>>& points);
