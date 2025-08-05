/*
@author Roy Maoded
@author Yarin Keshet
@date 2025-08-05

In this part, we built a multi-threaded Convex Hull server using the Proactor pattern.
The server handles multiple clients concurrently, allowing them to send commands to manage a personal set
of 2D points and calculate the Convex Hull area.
We also implemented an area watcher thread, which continuously monitors all clients’ convex hull areas.
If any area exceeds 100.0, a warning message is printed to the server console.
*/




#pragma once
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>
#include <pthread.h>

float CHArea_vec(const std::vector<std::pair<float, float>>& points);

// 🔒 Shared area state and synchronization tools (used by CHArea_vec and area_watcher thread):
extern double curr_area;
extern double last_report_area;
extern const double AREA_THRESHOLD;
extern pthread_mutex_t ch_mutex;
extern pthread_cond_t area_changed_cond;
