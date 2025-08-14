/*

@author Roy Maoded
@author Yarin Keshet
@date 2025-08-05

In this part, we implemented three versions of the CHArea function using different containers:
vector, deque, and list.
Each function calculates the area of the convex hull using the same algorithm but with 
a different internal data structure.
This setup allows us to compare the performance impact of using different STL containers.
We will profile all three versions to determine which is most efficient in our case.
*/

#pragma once
#include <vector>
#include <deque>
#include <list>
#include <utility>
#include <algorithm>
#include <cmath>

// Return the area of the Convex Hull of the points using different containers:
float CHArea_vec(const std::vector<std::pair<float, float>>& points);
float CHArea_deque(const std::vector<std::pair<float, float>>& points);
float CHArea_list(const std::vector<std::pair<float, float>>& points);
