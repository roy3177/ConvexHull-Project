#include <iostream>
#include <vector>
#include "ConvexHull.hpp"
#include <string>
#include <sstream>
#include <algorithm>
#include <utility>

using namespace std;

// Helper function to print all points
void print_points(const vector<pair<float, float>> &pts)
{
    if (pts.empty())
    {
        cout << "No points in the graph.\n";
    }
    else
    {
        cout << "Current points:\n";
        for (const auto &p : pts)
        {
            cout << "(" << p.first << "," << p.second << ") ";
        }
        cout << endl;
    }
}

int main()
{
    vector<pair<float, float>> points;
    string line;

    cout << "Convex Hull Interactive Program\n";
    cout << "Commands:\n";
    cout << "  Newgraph N         - Start a new graph with N points (enter each as x,y on a new line)\n";
    cout << "  Newpoint x,y       - Add a new point\n";
    cout << "  Removepoint x,y    - Remove a point\n";
    cout << "  CH                 - Compute and display convex hull area\n";
    cout << "  (Ctrl+D to exit)\n\n";

    while (true)
    {
        cout << "\nEnter command: ";
        if (!getline(cin, line))
            break;
        istringstream iss(line);
        string command;
        iss >> command;

        if (command == "Newgraph")
        {
            int n;
            string extra;
            if (!(iss >> n) || n <= 0 || (iss >> extra))
            {
                cerr << "Invalid format. Please enter as: Newgraph N (e.g., Newgraph 5)\n";
                continue;
            }
            points.clear();

            cout << "Enter " << n << " points (format: x,y):\n";
            int added = 0;
            while (added < n)
            {
                cout << "Point " << (added + 1) << ": ";
                string point_line;
                if (!getline(cin, point_line))
                    break;
                float x, y;
                char comma;
                istringstream point_stream(point_line);
                if (!(point_stream >> x >> comma >> y) || comma != ',')
                {
                    cerr << "Invalid point format. Please enter as: x,y (e.g., 1.5,2.5)\n";
                    continue;
                }
                points.emplace_back(x, y);
                ++added;
            }
            cout << n << " points added.\n";
            print_points(points);
        }

        else if (command == "Newpoint")
        {
            float x, y;
            char comma;
            while (!(iss >> x >> comma >> y) || comma != ',')
            {
                cerr << "Invalid point format. Please enter as: Newpoint x,y (e.g., Newpoint 1.5,2.5)\n";
                cout << "Try again: ";
                string retry_line;
                if (!getline(cin, retry_line))
                    break;
                istringstream retry_iss(retry_line);
                string retry_command;
                retry_iss >> retry_command; // skip command
                iss = move(retry_iss);
            }
            points.emplace_back(x, y);
            cout << "Point (" << x << "," << y << ") added.\n";
            print_points(points);
        }

        else if (command == "CH")
        {
            float area = CHArea_vec(points);
            cout << "Area: " << area << endl;
        }

        else if (command == "Removepoint")
        {
            float x, y;
            char comma;
            while (!(iss >> x >> comma >> y) || comma != ',')
            {
                cerr << "Invalid point format. Please enter as: Removepoint x,y (e.g., Removepoint 1.5,2.5)\n";
                cout << "Try again: ";
                string retry_line;
                if (!getline(cin, retry_line))
                    break;
                istringstream retry_iss(retry_line);
                string retry_command;
                retry_iss >> retry_command; // skip command
                iss = move(retry_iss);
            }

            auto it = find(points.begin(), points.end(), make_pair(x, y));
            if (it != points.end())
            {
                points.erase(it);
                cout << "Point (" << x << "," << y << ") removed.\n";
            }
            else
            {
                cout << "Point (" << x << "," << y << ") not found.\n";
            }
            print_points(points);
        }

        else
        {
            cerr << "Unknown command: " << command << endl;
        }
    }

    cout << "Exiting program.\n";
    return 0;
}