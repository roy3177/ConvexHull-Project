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
