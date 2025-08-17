#include <iostream>
#include <vector>
#include <utility>
#include "ConvexHull.hpp"

using namespace std;

int main(){

    int n; //Number of the points
    cout << "Enter the number of points: ";
    cin >> n;
    cin.ignore() ; //Read and ignore '/n' after the number

    if (n < 3) 
    {
        cerr << "Error: At least 3 points are required to form a convex hull." << endl;
        return 1;
    }

    vector<pair<float,float>> points;
    for(int i=0;i<n;i++)
    {
        float x;
        float y;
        char comma;
        bool unique = false;
        while (!unique) 
        {
            cout << "Point " << i + 1 << ": ";
            if (!(cin >> x >> comma >> y) || comma != ',') // Check for valid input format
            {
                cerr << "Error: Invalid input format. Expected format x,y (e.g. 0,1)." << endl;
                return 1;
            }
            // Check for duplicate points
            unique = true;
            for (const auto& pt : points) 
            {
                if (pt.first == x && pt.second == y) 
                {
                    cerr << "Error: Point already entered. Please enter a unique point." << endl;
                    unique = false;
                    break;
                }
            }
        }
        points.emplace_back(x,y); //Adding it to the vector of the points
    }


    float area=CHArea(points); //Commpute the area of the Convex Hull
    cout << "The area of the Convex Hull is : " << area << endl;


    return 0;
}