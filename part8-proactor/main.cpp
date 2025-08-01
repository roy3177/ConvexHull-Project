

#include "Reactor.hpp" // for the Reactor class
#include <iostream>    // for std::cout, std::cin
#include <unistd.h>    // for STDIN_FILENO
#include "Proactor.hpp"
#include  <cstring>

// Function that will be called by the Proactor when there's input:
void handleInput(int fd){

    std::string input;
    std::getline(std::cin, input);
    std::cout << "Received input: " << input << std::endl;

    if (input == "exit"){
        std::cout << "Stopping proactor..." << std::endl;
        exit(0); //Ends the process, the proactor thread will close as well
    }
}

int main(){

    std::cout << "Proactor running. Type something (type 'exit' to quit):" << std::endl;
    

    //Start new thread(Proactor), that listenning for input from the user:
    std::thread proactorThread=startProactor(STDIN_FILENO,handleInput);

    /*
    This thread will run indefintely until user types 'exit'.
    */

    //join() tells the main program to wait until this thread finishes its work:
    
    proactorThread.join(); //Wait for proactor thread to finish.

    return 0;
}
