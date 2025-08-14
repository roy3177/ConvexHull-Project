
#include "Proactor.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>

// Start a new thread for the socket and run the given callback function
std::thread startProactor(int sockfd, ProactorFunc func) {
    // Create and return a thread that monitors the socket for input readiness
    return std::thread([sockfd, func]() {
        while (true) {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);

            // Wait (block) until the socket is ready to read
            int activity = select(sockfd + 1, &readfds, nullptr, nullptr, nullptr);
            if (activity < 0) {
                std::cerr << "Select failed: " << strerror(errno) << std::endl;
                break;
            }

            if (FD_ISSET(sockfd, &readfds)) {
                // Handle the socket when data is ready
                func(sockfd);

                // Check if the socket was closed (from client or inside func)
                // recv with MSG_PEEK checks without removing data from buffer
                char temp;
                int check = recv(sockfd, &temp, 1, MSG_PEEK | MSG_DONTWAIT);

                if (check == 0) {
                    std::cout << "Socket " << sockfd << " disconnected.\n";
                    break; // socket closed
                } else if (check < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                    std::cerr << "Recv error on socket " << sockfd << ": " << strerror(errno) << std::endl;
                    break;
                }
            }
        }

        close(sockfd); // Cleanup after socket is done
        std::cout << "Socket " << sockfd << " closed.\n";
    });
}


//Stop the thread by joining it:
void stopProactor(std::thread& t){
    if(t.joinable()){
        t.join(); //Wait for the threaf to finish.
    }
}