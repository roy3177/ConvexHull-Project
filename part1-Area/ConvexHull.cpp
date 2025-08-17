#include "ConvexHull.hpp"
#include <algorithm>
#include <cmath>

using namespace std;

//Computes the cross product of three points A, B, C:
static float cross(const pair<float,float>& A, const pair<float,float>& B,const pair<float,float>& C)
{
    // [ (A.x - C.x) * (B.y - C.y) ] - [ (A.y - C.y) * (B.x - C.x) ]
    // The 2D cross product of vectors CA and CB
    // If the result is > 0, the sequence C → A → B makes a left turn (counterclockwise).
    // If the result is < 0, it makes a right turn (clockwise).
    // If the result is 0, the points are collinear.
    return (A.first-C.first)*(B.second-C.second)-(A.second-C.second)*(B.first-C.first);
}

float CHArea(const vector<pair<float,float>>& points_vec)
{
    int n=points_vec.size(); // number of points in vector points_vec
    int k=0; // index
    
    //Need at least three points:
    if(n<3)
    {
        return 0.0;
    }

    vector<pair<float,float>> pts=points_vec; // copy points_vec to a new vector named pts
    sort(pts.begin(),pts.end()); //first by x, and after by y

    //Allocate enough space for the convex hull (at most 2n points):
    vector<pair<float,float>> hull(2*n);

    //Build the lower hull:
    //Iterate over all the sorted points.
    //If the new point creates a right turn or a straight line,
    //we remove the previous point:
    for(int i=0;i<n;i++)
    {   // hull[k-2] and hull[k-1] are the last two points currently in the hull.
        // pts[i] is the new candidate point we want to add.
        while(k>=2 && cross(hull[k-2],hull[k-1],pts[i])<=0)
        {
            k--; //hull[k-1] is not part of the convex hull and should be removed (k--)    
        }
        hull[k++]=pts[i];// the current point pts[i] is added to the hull at position k,
        //and then k is incremented.
    }

    //Build the upper hull:
    //Iterate over all the sorted points==>but now from the end to the beginning:
    for (int i = n-2, t = k+1; i >= 0; --i) 
    {
        while (k >= t && cross(hull[k-2], hull[k-1], pts[i]) <= 0)
            k--;
        hull[k++] = pts[i];
    }

    hull.resize(k-1); //The first snd the last point returns itself=>remove one of them.

    //Compute the area:
    float area=0.0; // Initialize the area
    
    for(size_t i=0;i<hull.size();i++)
    {
        auto& p1=hull[i]; //p1 is the current point in the hull
        auto& p2=hull[(i+1) % hull.size()]; //p2 is the next point in the hull
        //The modulo operator (% hull.size()) ensures that after the last point,
        //it wraps around to the first point, closing the polygon.
        
        //the shoelace formula for the area of a polygon:
        //For each edge from p1 to p2, it adds (x1*y2 - x2*y1) to the running total area.
        area=area+(p1.first*p2.second-p2.first*p1.second);
    }

    //Return the absolute value of the area divided by 2==>the geometric formula for polygon's area:
    return fabs(area)/2.0 ; 
}