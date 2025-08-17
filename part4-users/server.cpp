#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>
#include <utility>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "ConvexHull.hpp"
#include <iomanip>

using namespace std;

#define PORT "9034" // Our port number(beej)
#define BACKLOG 10  // The number of pending connections that the queue will hold

vector<pair<float, float>> points; // The common Graph of points

// Function for listening for new connections:
int setup_server_socket()
{
    struct addrinfo hints, *res;
    int listener; // The reuslt of the socket function
    int yes = 1;  // For reusing the port

    // Clear the hints structure:
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // To get matching address for the local host

    // Get the address info:
    if (getaddrinfo(NULL, PORT, &hints, &res) != 0)
    {
        perror("getaddrinfo");
        exit(1);
    }

    // Create a socket
    // The socket function creates a new socket and returns a file descriptor for it:
    listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (listener < 0)
    {
        perror("socket");
        exit(1);
    }

    // Set the socket options to allow reuse of the address:
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    // Bind the socket to the address and port:
    if (bind(listener, res->ai_addr, res->ai_addrlen) < 0)
    {
        perror("bind");
        close(listener);
        exit(1);
    }

    // Free the addrinfo structure:
    freeaddrinfo(res);

    // Set the socket to listen for incoming connections:
    // BACKLOG is the maximum number of pending connections that the queue will hold:
    if (listen(listener, BACKLOG) < 0)
    {
        perror("listen");
        exit(1);
    }

    return listener;
}

// Main function to set up the server and handle client connections:
string process_command(const string &cmd, int client_fd, fd_set &master_fds)
{

    string clean_cmd = cmd;
    // remove all newline ('\n') and carriage return ('\r') characters from the string clean_cmd.
    clean_cmd.erase(remove(clean_cmd.begin(), clean_cmd.end(), '\n'), clean_cmd.end());
    clean_cmd.erase(remove(clean_cmd.begin(), clean_cmd.end(), '\r'), clean_cmd.end());

    if (clean_cmd.empty())
    {
        return "ERROR: Empty command received\n";
    }

    if (clean_cmd == "exit")
    {
        close(client_fd);
        FD_CLR(client_fd, &master_fds);
        return "Goodbye!\n";
    }

    stringstream ss(cmd); // Use stringstream to parse the command
    string keyword;
    ss >> keyword;

    if (keyword == "Newgraph")
    {

        if (!points.empty())
        {
            return "ERROR: A graph already exists. Remove all points first to create a new one.\n";
        }

        int n;
        // (ss >> n) -> uses a stringstream (ss) to extract an integer value
        // from the command string and store it in the variable n.
        if (!(ss >> n) || n < 3)
        {
            return "ERROR: Invalid Newgraph format. Must be at least 3 points\n";
        }

        points.clear();
        string confirmation = "OK: send " + to_string(n) + " points, one per line in format x,y\n";
        send(client_fd, confirmation.c_str(), confirmation.size(), 0);

        int i = 0;
        while (i < n)
        {
            char point_buf[1024];
            memset(point_buf, 0, sizeof(point_buf));
            int bytes = recv(client_fd, point_buf, sizeof(point_buf), 0);
            if (bytes <= 0)
            {
                return "ERROR: Failed to receive point " + to_string(i + 1) + "\n";
            }

            float x, y;
            char comma;
            stringstream ps(point_buf);
            char extra;//if something was entered after y in the x,y format
            // !(ps >> x >> comma >> y) -> check to see if the input string can be successfully parsed into three values
            if (!(ps >> x >> comma >> y) || comma != ',' || ps >> extra)
            {
                string err = "ERROR: Invalid format at point " + to_string(i + 1) + " (expected x,y)\n";
                send(client_fd, err.c_str(), err.size(), 0);
                continue;
            }

            pair<float, float> new_point = {x, y};
            if (std::find(points.begin(), points.end(), new_point) != points.end())
            {
                string err = "ERROR: Duplicate point detected at point " + to_string(i + 1) + "\n";
                send(client_fd, err.c_str(), err.size(), 0);
                continue;
            }

            points.push_back(new_point);
            string ack = "Server: Point " + to_string(i + 1) + " received\n";
            send(client_fd, ack.c_str(), ack.size(), 0);
            ++i;
        }

        ostringstream oss;
        oss << fixed << setprecision(2);
        oss << "Graph initialized with " << points.size() << " points:\n";
        for (const auto &p : points)
        {
            oss << "(" << p.first << "," << p.second << ")\n";
        }
        return oss.str();
    }

    else if (keyword == "Newpoint")
    {
        float x, y;
        char comma;
        if (!(ss >> x >> comma >> y) || comma != ',')
        {
            return "ERROR: Usage: Newpoint x,y\n";
        }
        pair<float, float> new_point = {x, y};
        if (std::find(points.begin(), points.end(), new_point) != points.end())
        {
            return "ERROR: Point already exists\n";
        }
        points.push_back(new_point);

        ostringstream oss;
        oss << "Point added\n";
        oss << fixed << setprecision(2);
        for (const auto &p : points)
        {
            oss << "(" << p.first << "," << p.second << ")\n";
        }
        return oss.str();
    }

    else if (keyword == "Removepoint")
    {
        float x, y;
        char comma;
        if (!(ss >> x >> comma >> y) || comma != ',')
        {
            return "ERROR: Usage: Removepoint x,y\n";
        }

        auto it = std::find(points.begin(), points.end(), make_pair(x, y));
        if (it != points.end())
        {
            points.erase(it);
            ostringstream oss;
            oss << "Point removed\n";
            oss << fixed << setprecision(2);
            for (const auto &p : points)
            {
                oss << "(" << p.first << "," << p.second << ")\n";
            }
            return oss.str();
        }
        else
        {
            return "ERROR: Point not found\n";
        }
    }

    else if (keyword == "CH")
    {
        if (points.size() < 3)
            return "0\n";
        float area = CHArea_vec(points);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << area;
        return "Area: " + oss.str() + "\n";
    }

    else
    {
        return "ERROR: Unknown command\n";
    }
}

int main()
{

    int listener = setup_server_socket(); // Create and bind the server socket
    fd_set master_fds, read_fds;          // Set of master file descriptors for select
    int fdmax = listener;                 // Maximum file descriptor number, initially set to the listener socket

    // Initialize the master file descriptor set:
    FD_ZERO(&master_fds);
    FD_SET(listener, &master_fds); // Add the listener socket to the master set

    cout << "Server listening on port " << PORT << "..." << endl;

    // Uses select to wait for activity on any socket (new connection or client message).
    while (true)
    {
        read_fds = master_fds; // Copy the master set to read_fds for select

        // Use select to wait for activity on the sockets:
        // select() blocks until one or more file descriptors are ready for reading
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(4);
        }

        // Check each file descriptor in the master set:
        for (int i = 0; i <= fdmax; i++)
        {

            // If the file descriptor is set in read_fds, it means it's ready for reading:
            // FD_ISSET checks if the file descriptor i is part of the read_fds set
            if (FD_ISSET(i, &read_fds))
            {
                if (i == listener)
                {

                    // New connection on the listener socket:
                    sockaddr_storage client_addr;
                    socklen_t addrlen = sizeof client_addr;

                    // Accept the new connection:
                    int newfd = accept(listener, (sockaddr *)&client_addr, &addrlen);

                    if (newfd == -1)
                    {
                        perror("accept");
                    }
                    else
                    {
                        FD_SET(newfd, &master_fds); // Add the new socket to the master set

                        if (newfd > fdmax)
                        {
                            fdmax = newfd;
                        }

                        cout << "New connection on socket " << newfd << endl;

                        //Send welcome message to the client:
                        string welcome =
                            "Welcome to the Convex Hull Server!\n"
                            "Available commands:\n"
                            "  Newgraph N       - Create a new graph with N points (each in x,y)\n"
                            "  Newpoint x,y     - Add a point to the graph\n"
                            "  Removepoint x,y  - Remove a point from the graph\n"
                            "  CH               - Compute the convex hull area\n"
                            "  exit             - Disconnect from the server\n";

                        send(newfd, welcome.c_str(), welcome.size(), 0);
                    }
                }

                // If the file descriptor is not the listener, it must be a client socket:
                else
                {
                    // Handle data from a client socket:
                    char buf[1024];
                    memset(buf, 0, sizeof(buf));
                    int nbytes = recv(i, buf, sizeof(buf), 0);

                    // Check if recv() was successful:
                    if (nbytes <= 0)
                    {

                        // If nbytes is 0, the connection was closed by the client:
                        if (nbytes == 0)
                        {
                            cout << "Socket " << i << " disconnected." << endl;
                        }

                        // If nbytes is -1, an error occurred:
                        else
                        {
                            perror("recv");
                        }

                        // Close the socket and remove it from the master set:
                        close(i);
                        FD_CLR(i, &master_fds);
                    }
                    else
                    {
                        buf[nbytes] = '\0'; // Ensure the string is null-terminated for printing
                        string response = process_command(string(buf), i, master_fds);

                        cout << "[Server] Received command: " << buf << endl;
                        cout << "[Server] Sending response: " << response << endl;

                        send(i, response.c_str(), response.size(), 0);
                    }
                }
            }
        }
    }

    return 0;
}
