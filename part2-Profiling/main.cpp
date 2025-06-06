#include <iostream>
#include <chrono>
#include "ConvexHull.hpp"
#include <vector>

using namespace std;
using namespace chrono;

int main() {
    vector<pair<float,float>> points = {
        {0, 0}, {0, 1}, {1, 2}, {2, 2}, {3, 1},
        {3, 0}, {2, -1}, {1, -1}, {0.5, 0.5}, {1.5, 1.5}
    };

    cout << "Points:\n";
    for (auto& p : points) {
        cout << "(" << p.first << "," << p.second << ") ";
    }
    cout << "\n\n";

    // Vector version
    auto start1 = high_resolution_clock::now();
    float area1 = CHArea_vec(points);
    auto end1 = high_resolution_clock::now();
    auto dur1 = duration_cast<nanoseconds>(end1 - start1).count();

    // Deque version
    auto start2 = high_resolution_clock::now();
    float area2 = CHArea_deque(points);
    auto end2 = high_resolution_clock::now();
    auto dur2 = duration_cast<nanoseconds>(end2 - start2).count();

    // List version
    auto start3 = high_resolution_clock::now();
    float area3 = CHArea_list(points);
    auto end3 = high_resolution_clock::now();
    auto dur3 = duration_cast<nanoseconds>(end3 - start3).count();

    cout << "Area (vector): " << area1 << " | Time: " << dur1 << " ns\n";
    cout << "Area (deque):  " << area2 << " | Time: " << dur2 << " ns\n";
    cout << "Area (list):   " << area3 << " | Time: " << dur3 << " ns\n";

    return 0;
}
