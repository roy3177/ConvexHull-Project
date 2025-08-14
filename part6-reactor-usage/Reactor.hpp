#pragma once
#include <functional>
#include <unordered_map>
#include <atomic>

class Reactor {
public:

    /*
     Define a callback type that takes an integer (file descriptor) as an argument:
     This callback will be called when an event occurs on the file descriptor.
     This is a functor that can be used to handle events such as read, write, or error on a file descriptor.
    */
    using Callback = std::function<void(int)>; 


    Reactor(); // Constructor
    ~Reactor(); // Destructor 

    void addFd(int fd, Callback cb); //Adding fd, the callback will be called when an event occurs on the file descriptor

    void removeFd(int fd); //Removing fd, the callback will not be called anymore

    void start(); // Start the reactor loop, which will run in a separate thread and handle events

    void stop(); // Stop the reactor loop, which will terminate the thread and stop handling events              

private:

    std::unordered_map<int, Callback> callbacks;

    // A thread that will run the reactor loop:
    std::atomic<bool> running;

    // The reactor loop function that will be run in a separate thread:
    void loop(); 
};
