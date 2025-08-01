
#include "Proactor.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>

// Start a new thread for the socket and run the given callback function
std::thread startProactor(int sockfd, ProactorFunc func ){

    //Create a new thread, that running in the loop and checking when the socket is ready for reading:
    return std::thread([sockfd, func](){
        // In the thread: wait for input on the socket and call the function when ready:
        while(true){
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(sockfd,&readfds);


            //Calling to select() to check if there is information to read from the socket:
            int activity=select(sockfd+1,&readfds,nullptr,nullptr,nullptr);
            if(activity<0){
                std::cerr<<"Select failed: " << strerror(errno)<<std::endl;
                break;
            }

            //If came information to the socket-->calling the func to handle the socket:
            if(FD_ISSET(sockfd, &readfds)){
                func(sockfd);
            }
        }

        close(sockfd); //Closing the socket when we done
        std::cout<<"Socket " <<sockfd << "closed.\n";

    });
}

//Stop the thread by joining it:
void stopProactor(std::thread& t){
    if(t.joinable()){
        t.join(); //Wait for the threaf to finish.
    }
}