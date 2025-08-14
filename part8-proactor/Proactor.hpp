/*
@author Roy Maoded
@author Yarin Keshet
@date 2025-08-05

In this part, we implemented the Proactor design pattern in C++.
The Proactor module allows us to handle asynchronous events using threads.
We defined a startProactor function that starts a new thread.
The thread uses select() to wait for input readiness on a given socket or file descriptor.
When input is ready, a user-provided callback function is executed.
We also implemented stopProactor to cleanly wait for the thread to finish.
To test the implementation, we created a simple main.cpp that listens to user input from the keyboard (STDIN), 
and echoes it back.
When the user types "exit", the program stops gracefully.
This part sets the foundation for handling multiple clients in the next stages using Proactor logic.


*/

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
    


