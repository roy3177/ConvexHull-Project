/*

@author Roy Maoded
@author Yarin Keshet
@date 2025-08-05

In this part, we implemented a basic Reactor system using C++.
The reactor listens to multiple file descriptors and triggers a callback function whenever
an event (like input) occurs.
We used select() to monitor events.
When a file descriptor becomes ready (e.g., the user types something), the associated callback is called.
In main, we registered STDIN with a callback that prints the input.
Typing "exit" stops the reactor loop.
This structure is a foundation for building event-driven systems such as servers or async I/O handlers.
*/


#pragma once
#include <functional>
#include <unordered_map>
#include <atomic> 

class Reactor 
{
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
