

#include "Reactor.hpp" // for the Reactor class
#include <iostream>    // for std::cout, std::cin
#include <unistd.h>    // for STDIN_FILENO

// Function to handle input from the terminal
// fd is not used here, but it is required by the Reactor's callback signature
void handleInput([[maybe_unused]] int fd)
{

    std::string input;
    std::getline(std::cin, input);
    std::cout << "Received input: " << input << std::endl;
    if (input == "exit")
    {
        std::cout << "Stopping reactor..." << std::endl;
        // Not ideal, but here we assume global reactor object to stop it
        exit(0); // Alternatively use a signal or shared flag
    }
}

int main()
{
    Reactor reactor;
    reactor.addFd(STDIN_FILENO, handleInput); // Listen to input from terminal
    std::cout << "Reactor running. Type something (type 'exit' to quit):" << std::endl;
    reactor.start();
    return 0;
}
