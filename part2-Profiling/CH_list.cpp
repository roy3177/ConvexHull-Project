#include "ConvexHull.hpp"

using namespace std;

// Computes the cross product of three points A, B, C:
static float cross(const pair<float, float> &A, const pair<float, float> &B, const pair<float, float> &C)
{
    return (A.first - C.first) * (B.second - C.second) - (A.second - C.second) * (B.first - C.first);
}

float CHArea_list(const vector<pair<float, float>> &points)
{
    int n = points.size();

    // Need at least three points:
    if (n < 3)
    {
        return 0.0;
    }

    vector<pair<float, float>> pts = points;
    sort(pts.begin(), pts.end()); // first by x, and after by y

    list<pair<float, float>> hull;

    // Lower hull:
    for (int i = 0; i < n; i++)
    {
        while(hull.size()>=2)
        {
            auto last = prev(hull.end());
            auto second_last = prev(last);
            if (cross(*second_last, *last, pts[i]) <= 0)
            {
                hull.pop_back(); // Remove the last point if it creates a right turn or a straight line
            }
            else
            {
                break; // No more points to remove
            }
        }
        hull.push_back(pts[i]);
    }

    // Upper hull:
    size_t lower_size = hull.size();
    for (int i = n - 2; i >= 0; --i)
    {
        while (hull.size() > lower_size)
        {
            auto last = prev(hull.end());
            auto second_last = prev(last);
            if (cross(*second_last, *last, pts[i]) <= 0)
            {
                hull.pop_back(); // Remove the last point if it creates a right turn or a straight line
            }
            else
            {
                break; // No more points to remove
            }
        }
        hull.push_back(pts[i]);
    }

    hull.pop_back(); // Remove duplicate last point

    // Compute the area:
    float area = 0.0;
    auto it1 = hull.begin();
    auto it2 = next(it1);
    for(; it2 != hull.end(); ++it1, ++it2)
    {
        area += (it1->first * it2->second - it2->first * it1->second);
    }
    area += (hull.back().first * hull.front().second - hull.front().first * hull.back().second);

    // Return the absolute value of the area divide 2==>the geometric formula for polygon's area:
    return fabs(area) / 2.0;
}