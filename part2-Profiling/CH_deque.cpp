#include "ConvexHull.hpp"


using namespace std;

//Computes the cross product of three points A, B, C:
static float cross(const pair<float,float>& A, const pair<float,float>& B,const pair<float,float>& C){
    return (A.first-C.first)*(B.second-C.second)-(A.second-C.second)*(B.first-C.first);
}

float CHArea_deque(const vector<pair<float,float>>& points){
    int n=points.size();
    
    //Need at least three points:
    if(n<3){
        return 0.0;
    }

    vector<pair<float,float>> pts=points;
    sort(pts.begin(),pts.end()); //first by x, and after by y

    deque<pair<float,float>>hull; 

    //Lower shell:
    for(int i=0;i<n;i++){
        while(hull.size()>=2 && cross(hull[hull.size()-2],hull[hull.size()-1],pts[i])<=0){
            hull.pop_back();            
        }
        hull.push_back(pts[i]);
    }

    //Upper shell:
    size_t lower_size = hull.size() ; //t is the size of the lower hull
    for (int i = n-2; i >= 0; --i) {
        while (hull.size()> lower_size && cross(hull[hull.size()-2],hull[hull.size()-1], pts[i]) <= 0){
           hull.pop_back();
        }
        hull.push_back(pts[i]);
    }

    hull.pop_back(); //The first snd the last point returns itself=>remove one of them.

    //Compute the area:
    float area=0.0;
    for(size_t i=0;i<hull.size();i++){
        auto& p1=hull[i];
        auto& p2=hull[(i+1) % hull.size()];
        area=area+(p1.first*p2.second-p2.first*p1.second);
    }

    //Returnn the absolute value of the area divide 2==>the geometric formula for polygon's area:
    return fabs(area)/2.0 ; 
}