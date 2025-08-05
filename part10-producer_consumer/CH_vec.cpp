#include "ConvexHull.hpp"
#include <pthread.h>

//Our threshold for the area of the convex hull(that we want to warn the user):
const double AREA_THRESHOLD = 100.0;

//Global variables for saving the current area and the last reported area:
double curr_area = 0.0;
double last_report_area = -1.0;

//Synchronization primitives for the area watcher thread:
pthread_mutex_t ch_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t area_changed_cond = PTHREAD_COND_INITIALIZER;


using namespace std;

//Computes the cross product of three points A, B, C:
static float cross(const pair<float,float>& A, const pair<float,float>& B,const pair<float,float>& C){
    return (A.first-C.first)*(B.second-C.second)-(A.second-C.second)*(B.first-C.first);
}

float CHArea_vec(const vector<pair<float,float>>& points)
{
    int n=points.size();
    int k=0;
    
    //Need at least three points:
    if(n<3){
        return 0.0;
    }

    vector<pair<float,float>> pts=points;
    sort(pts.begin(),pts.end()); //first by x, and after by y

    vector<pair<float,float>>hull(2*n); 

    //Iterate over all the sorted points.
    //If the new point creates a right turn or a straight line, we remove the previous point:
    for(int i=0;i<n;i++){
        while(k>=2 && cross(hull[k-2],hull[k-1],pts[i])<=0){
            k--;            
        }
        hull[k++]=pts[i];
    }

    //Iterate over all the sorted points==>but now from the end to the beginning:
    for (int i = n-2, t = k+1; i >= 0; --i) {
        while (k >= t && cross(hull[k-2], hull[k-1], pts[i]) <= 0)
            k--;
        hull[k++] = pts[i];
    }

    hull.resize(k-1); //The first snd the last point returns itself=>remove one of them.

    //Compute the area:
    float area=0.0;
    for(size_t i=0;i<hull.size();i++){
        auto& p1=hull[i];
        auto& p2=hull[(i+1) % hull.size()];
        area=area+(p1.first*p2.second-p2.first*p1.second);
    }

   area = fabs(area)/2.0;

    // 🔒 Synchronization: safely update curr_area and notify watcher if changed
    pthread_mutex_lock(&ch_mutex);
    curr_area = area;

    if (fabs(curr_area - last_report_area) >= 1e-3) {
        last_report_area = curr_area;
        pthread_cond_signal(&area_changed_cond); // Notify the waiting thread
    }

    pthread_mutex_unlock(&ch_mutex);

    return area; 
}