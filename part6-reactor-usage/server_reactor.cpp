#include "Reactor.hpp"     
#include "ConvexHull.hpp"      
#include <iostream>       
#include <unistd.h>        // close()
#include <netinet/in.h>    // sockaddr_in, htons
#include <sys/socket.h>    // socket(), bind(), listen(), accept()
#include <cstring>         // memset
#include <map>             
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>         // std::setprecision

Reactor reactor;
std::vector<std::pair<float, float>> shared_graph; // The common Graph of clients.

/**
* 
Function to handle client connections and process commands:
This function will be called by the reactor when a new client connects or
when data is available to read from a client.
It will read the command from the client, process it, and send a response back
to the client. 
* 
*/

void handleClient(int client_fd) {

    char buf[1024]; // Buffer to hold incoming data that is received from the client.
    memset(buf, 0, sizeof(buf));

    int nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);



    // Check if the recv() call was successful:
    if (nbytes <= 0) {
        if (nbytes == 0) {
            std::cout << "Client " << client_fd << " disconnected.\n";
        } else {
            perror("recv");
        }

        // Close the client socket and remove it from the reactor:
        reactor.removeFd(client_fd);
        close(client_fd);
        return;
    }

    // Process the received command:
    std::string cmd(buf);
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\n'), cmd.end());
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\r'), cmd.end());

    //If the client sends "exit", close the connection:
    if (cmd == "exit") {
        std::cout << "Client " << client_fd << " has disconnected.\n";  
        std::string msg = "Goodbye!\n";
        if (send(client_fd, msg.c_str(), msg.size(), 0) == -1) {
            perror("send");
        }
        close(client_fd);
        reactor.removeFd(client_fd);
        return;
    }

    //Extract the command keyword:
    //(Newgraph, Newpoint, Removepoint, CH):
    std::stringstream ss(cmd);
    std::string keyword;
    ss >> keyword;

    if (keyword == "Newgraph") {
        if (!shared_graph.empty()) {
            std::string err = "ERROR: A graph already exists. Remove all points first to create a new one.\n";
            if(send(client_fd, err.c_str(), err.size(), 0)==-1) {
                perror("send");
            }
            return;
        }

        int n;
        if (!(ss >> n) || n < 3) {
            std::string err = "ERROR: Invalid Newgraph format. Must be at least 3 points\n";
            if(send(client_fd, err.c_str(), err.size(), 0)==-1) {
                perror("send");
            }
            return;
        }

        shared_graph.clear(); // Clear the existing graph if any.
        std::string confirmation = "OK: send " + std::to_string(n) + " points, one per line in format x,y\n";
        if(send(client_fd, confirmation.c_str(), confirmation.size(), 0)==-1) {
            perror("send");
        }

        int i = 0;
        while (i < n) {
            char point_buf[1024];
            memset(point_buf, 0, sizeof(point_buf));
            int bytes = recv(client_fd, point_buf, sizeof(point_buf) - 1, 0);
            if (bytes <= 0) {
                std::string err = "ERROR: Failed to receive point " + std::to_string(i + 1) + "\n";
                if(send(client_fd, err.c_str(), err.size(), 0)==-1) {
                    perror("send");
                }
                return;
            }

            float x, y;
            char comma, extra;
            std::stringstream ps(point_buf);
            if (!(ps >> x >> comma >> y) || comma != ',' || ps >> extra) {
                std::string err = "ERROR: Invalid format at point " + std::to_string(i + 1) + " (expected x,y)\n";
                if(send(client_fd, err.c_str(), err.size(), 0)==-1) {
                    perror("send");
                }
                continue;
            }

            std::pair<float, float> p = {x, y};
            if (std::find(shared_graph.begin(), shared_graph.end(), p) != shared_graph.end()) {
                std::string err = "ERROR: Duplicate point detected at point " + std::to_string(i + 1) + "\n";
                if(send(client_fd, err.c_str(), err.size(), 0)==-1) {
                    perror("send");
                }
                continue;
            }

            shared_graph.push_back(p);
            std::string ack = "Server: Point " + std::to_string(i + 1) + " received\n";
            if(send(client_fd, ack.c_str(), ack.size(), 0)==-1) {
                perror("send");
            }
            ++i;
        }

        // After receiving all points, send a summary of the graph:
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "Graph initialized with " << shared_graph.size() << " points:\n";
        for (const auto& p : shared_graph) {
            oss << "(" << p.first << "," << p.second << ")\n";
        }
        std::string summary = oss.str();
        if(send(client_fd, summary.c_str(), summary.size(), 0)==-1) {
            perror("send");
        }
    }

    else if (keyword == "Newpoint") 
    {
        float x, y;
        char comma;
        if (!(ss >> x >> comma >> y) || comma != ',') {
            std::string err = "ERROR: Usage: Newpoint x,y\n";
            if(send(client_fd, err.c_str(), err.size(), 0)==-1) {
                perror("send");
            }
            return;
        }

        std::pair<float, float> p = {x, y};
        if (std::find(shared_graph.begin(), shared_graph.end(), p) != shared_graph.end()) {
            std::string err = "ERROR: Point already exists\n";
            if(send(client_fd, err.c_str(), err.size(), 0)==-1) {
                perror("send");
            }
            return;
        }

        shared_graph.push_back(p);

        std::ostringstream oss;
        oss << "Point added\n";
        oss << std::fixed << std::setprecision(2);
        for (const auto& pt : shared_graph) {
            oss << "(" << pt.first << "," << pt.second << ")\n";
        }
        std::string summary = oss.str();
        if(send(client_fd, summary.c_str(), summary.size(), 0)==-1) {
            perror("send");
        }
    }

    else if (keyword == "Removepoint") {
        float x, y;
        char comma;
        if (!(ss >> x >> comma >> y) || comma != ',') {
            std::string err = "ERROR: Usage: Removepoint x,y\n";
            if(send(client_fd, err.c_str(), err.size(), 0)==-1) {
                perror("send");
            }
            return;
        }

        auto it = std::find(shared_graph.begin(), shared_graph.end(), std::make_pair(x, y));
        if (it != shared_graph.end()) {
            shared_graph.erase(it);
            std::ostringstream oss;
            oss << "Point removed\n";
            oss << std::fixed << std::setprecision(2);
            for (const auto& pt : shared_graph) {
                oss << "(" << pt.first << "," << pt.second << ")\n";
            }
            std::string summary = oss.str();
            if(send(client_fd, summary.c_str(), summary.size(), 0)==-1) {
                perror("send");
            }
        } else {
            std::string err = "ERROR: Point not found\n";
            if(send(client_fd, err.c_str(), err.size(), 0)==-1) {
                perror("send");
            }
        }
    }

    else if (keyword == "CH") {
        if (shared_graph.size() < 3) {
            std::string msg = "0\n";
            if(send(client_fd, msg.c_str(), msg.size(), 0)==-1) {
                perror("send");
            }
            return;
        }

        float area = CHArea_vec(shared_graph);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "Area: " << area << "\n";
        std::string res = oss.str();
        if(send(client_fd, res.c_str(), res.size(), 0)==-1) {
            perror("send");
        }
    }

    else {
        std::string err = "ERROR: Unknown command\n";
        if(send(client_fd, err.c_str(), err.size(), 0)==-1) {
            perror("send");
        }
    }

}

int main() {

    //Create the TCP server socket:
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(9034); // Beej's port number
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address and port:
    if (bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    // Listen for incoming connections:
    //The 10 here is the backlog size, which is the maximum number of pending connections that can be queued:
    if (listen(server_fd, 10) == -1) {
        perror("listen");
        close(server_fd);
        return 1;
    }

    std::cout << "Server is listening on port 9034...\n";

    // Add the server socket to the reactor:
    // The reactor will call the handleClient function when a new client connects:
    //This is a lambda function that will be called when a new client connects:
    reactor.addFd(server_fd, [&](int fd) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(fd, (sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("accept");
            return;
        }

        std::cout << "New client connected: " << client_fd << "\n";

        std::string welcome_msg =
        "Welcome to the Convex Hull Server!\n"
        "Available commands:\n"
        "  Newgraph N       - Create a new graph with N points (each in x,y)\n"
        "  Newpoint x,y     - Add a point to the graph\n"
        "  Removepoint x,y  - Remove a point from the graph\n"
        "  CH               - Compute the convex hull area\n"
        "  exit             - Disconnect from the server\n";

        if(send(client_fd, welcome_msg.c_str(), welcome_msg.size(), 0)==-1) {
            perror("send");
        }

        reactor.addFd(client_fd, handleClient); // Add the client socket to the reactor
    });

    // Start the reactor loop in a separate thread:
    reactor.start();

    return 0;
}
