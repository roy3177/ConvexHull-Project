/*
@author Roy Maoded
@author Yarin Keshet
@date 2025-08-05
In this part, we define the interface for the function CHArea, which computes the area of the
Convex Hull for a given set of 2D points.
The input is a std::vector of (float, float) pairs representing points on the plane.
The output is a float value representing the area of the convex hull surrounding the given points.
*/


#pragma once
#include <vector>
#include <utility>


//Return the area of the Convex Hull of the points:
float CHArea(const std::vector<std::pair<float,float>>& points);