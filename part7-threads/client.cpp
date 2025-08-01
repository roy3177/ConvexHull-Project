#include <iostream>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 9034 

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <server_ip>\n";
        return 1;
    }

    const char* server_ip = argv[1];

    //Creating a TCP socket:
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    //Defining the server address structure:
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);  // תמיד 9034
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        cerr << "Invalid IP address\n";
        return 1;
    }

    //Connecting to the server:
    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        return 1;
    }

    cout << "Connected to server at " << server_ip << " with the beej port : " << PORT << endl;

   

   
    string command;
    char buffer[4096];


     memset(buffer, 0, sizeof(buffer));
    int welcome_bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (welcome_bytes > 0) {
        cout << buffer;
    }

    while (true) {
        cout << "\nEnter command (or 'exit'): ";
        if (!getline(cin, command)) break;

        if (command == "exit") {
            send(sock, command.c_str(), command.length(), 0);

            memset(buffer, 0, sizeof(buffer));
            int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (bytes > 0) {
                cout << "Server response: " << buffer << endl;
            }

            break;
        }

        send(sock, command.c_str(), command.length(), 0);

        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            cout << "Server disconnected.\n";
            break;
        }

        string response(buffer);
        cout << "Server response: " << response;

        if (command.find("Newgraph") == 0 && response.find("ERROR") == string::npos) {
            int n;
            if (sscanf(command.c_str(), "Newgraph %d", &n) == 1) {
                int i = 1;
                while (i <= n) {
                    cout << "Enter point " << i << " (x,y): ";
                    string point;
                    if (!getline(cin, point)) break;

                    send(sock, point.c_str(), point.length(), 0);

                    memset(buffer, 0, sizeof(buffer));
                    int r = recv(sock, buffer, sizeof(buffer) - 1, 0);
                    if (r <= 0) {
                        cout << "Server disconnected.\n";
                        break;
                    }

                    string server_response(buffer);
                    cout << "Server: " << server_response;

                    if (server_response.find("ERROR") == string::npos) {
                        ++i; 
                    }
                  
                }


                memset(buffer, 0, sizeof(buffer));
                int r = recv(sock, buffer, sizeof(buffer) - 1, 0);
                if (r > 0) {
                    cout << "Server summary: " << buffer;
                }
            }
        }

    }

    close(sock);
    cout << "Disconnected.\n";
    return 0;
}
