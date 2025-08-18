#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cmath>

extern pthread_mutex_t ch_mutex;
extern pthread_cond_t area_changed_cond;

extern double curr_area;

const double AREA_THRESHOLD = 100.0;

//locks the mutex and waits for the condition variable to be signaled
// (meaning the area might have changed)
void* areaWatcherThread(void*) 
{
    bool was_above_threshold = false;

    while (true) 
    {

        // Wait for the condition variable to be signaled:
        pthread_mutex_lock(&ch_mutex);

        pthread_cond_wait(&area_changed_cond, &ch_mutex);

        // Check the new state of the area:
        if (curr_area >= AREA_THRESHOLD && !was_above_threshold) 
        {
            std::cout << "At Least 100 units belongs to CH" << std::endl;
            was_above_threshold = true;
        }

        // If the area goes below the threshold, notify the user:
        else if (curr_area < AREA_THRESHOLD && was_above_threshold) 
        {
            std::cout << "At Least 100 units no longer belongs to CH" << std::endl;
            was_above_threshold = false;
        }

        pthread_mutex_unlock(&ch_mutex);
    }

    return nullptr;
}
