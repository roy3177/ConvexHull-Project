

#pragma once

#include <thread>
#include <functional>
#include <unordered_map>


/*
This line create alias for a type of function
that get an integer as an argument and returns void.
*/

using ProactorFunc=std::function<void(int)>;


/*
Define method for starting a Proactor:
This method will take a socket file descriptor and a function that
will be run in a separate thread to handle events on that socket.
This method will return a thread object that can be used to manage the Proactor.
This method gives us to use new thread with the function that we want to run in the Proactor,
and the number of the socket that we want to handle events on.
*/

std::thread startProactor(int sockfd, ProactorFunc func) ;


/*
Define method that get reference to a thread and stops the Proactor.
using join(),that is a method of std::thread, that waits for a thread to
finish before allowing the program to conitnue.
*/

void stopProactor(std::thread& t);
    


