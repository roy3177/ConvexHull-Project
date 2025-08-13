/*

@author Roy Maoded
@author Yarin Keshet
@date 2025-08-05

In this part, we implemented a multi-user TCP server that allows multiple clients to interact with a shared set
of 2D points.
Clients can connect concurrently and send textual commands such as Newgraph, Newpoint, Removepoint,
and CH to modify the graph or compute the convex hull area.
The server uses select() to handle multiple socket connections efficiently within a single-threaded environment.
All clients operate on the same shared graph, and any changes by one client are reflected for all others.
The convex hull area is computed using the CHArea_vec function implemented in previous parts.
*/

#pragma once
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>

float CHArea_vec(const std::vector<std::pair<float, float>>& points);

