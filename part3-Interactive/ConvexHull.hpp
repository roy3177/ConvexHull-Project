/*

@author Roy Maoded
@author Yarin Keshet
@date 2025-08-05

In this part, we implement an interactive program that reads commands from stdin.
The user can create a new set of points, add or remove points, and request the convex hull area
using the command CH.
The supported commands are Newgraph n, Newpoint x,y, Removepoint x,y, and CH, with each ending in a line feed.
The program processes these inputs and outputs only the convex hull area when requested.
*/




#pragma once
#include <vector>
#include <deque>
#include <list>
#include <utility>
#include <algorithm>
#include <cmath>

float CHArea_vec(const std::vector<std::pair<float, float>>& points);

