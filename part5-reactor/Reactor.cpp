#include "Reactor.hpp"  // for the Reactor class
#include <sys/select.h> // for using select()
#include <unistd.h>     // for using close()
#include <iostream>     // for using std::cerr for printing errors

/*
Initializer constructor for the Reactor class
Sets the running state to false
*/
Reactor::Reactor() : running(false) {}

Reactor::~Reactor()
{
    stop(); //  Make sure to stop the reactor before destruction
}

// Adding a file descriptor (fd) with its associated callback function (cb)
void Reactor::addFd(int fd, Callback cb)
{
    callbacks[fd] = cb;
}

// Removing a file descriptor (fd) from the reactor
void Reactor::removeFd(int fd)
{
    callbacks.erase(fd);
}

// The start function begins the reactor loop and handles events in the current thread:
void Reactor::start()
{
    running = true;
    loop();
}

// The stop function sets the running state to false, which will terminate the reactor loop
void Reactor::stop()
{
    running = false;
}

// The loop function is the core of the reactor, it waits for events on file descriptors and calls the associated callbacks
void Reactor::loop()
{
    while (running)
    {
        fd_set readfds;    // A structure to hold the set of file descriptors that we want to check for reading.
        FD_ZERO(&readfds); // Clear the set of file descriptors

        int max_fd = -1;

        // Iterate over all fds being tracked
        for (const auto &[fd, _] : callbacks)
        {
            FD_SET(fd, &readfds); // Builds the set of file descriptors to monitor(not from scratch)
            if (fd > max_fd) // Keeps track of the maximum file descriptor
                max_fd = fd;
        }

        // If there are no file descriptors to monitor, we can skip the select call:
        if (max_fd < 0)
        {
            std::cerr << "No file descriptors to monitor.\n";
            break;
        }

        /*
        Use select to wait for activity on the file descriptors
        select() blocks until one or more file descriptors are ready for reading
        */

        int activity = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);

        // Check if select() was successful
        if (activity < 0)
        {
            perror("select failed");
            break;
        }

        for (const auto &[fd, cb] : callbacks)
        {
            // Check if the file descriptor is ready for reading
            // If it returns true, it means there is data to read or an event occurred
            if (FD_ISSET(fd, &readfds))
            {
                cb(fd); // Call the callback function associated with the file descriptor
            }
        }
    }
}
