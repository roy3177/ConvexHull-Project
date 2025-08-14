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
#include <thread>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include "Proactor.hpp"



using namespace std;

#define PORT "9034" // Our port number(beej)
#define BACKLOG 10  // The number of pending connections that the queue will hold

// Global vector to hold points for all clients:
unordered_map<int, vector<pair<float, float>>> client_points;

atomic<int> thread_counter(1); //Counter for the number of threads via order

// Function for listening for new connections:
int setup_server_socket(){
    
    struct addrinfo hints, *res;
    int listener; // The reuslt of the socket function
    int yes = 1;  // For reusing the port

    // Clear the hints structure:
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // To get matching address for the local host

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
string process_command(const string &cmd, int client_fd){

    string clean_cmd = cmd;
    clean_cmd.erase(remove(clean_cmd.begin(), clean_cmd.end(), '\n'), clean_cmd.end());
    clean_cmd.erase(remove(clean_cmd.begin(), clean_cmd.end(), '\r'), clean_cmd.end());

    if (clean_cmd.empty())
    {
        return "ERROR: Empty command received\n";
    }

    if (clean_cmd == "exit")
    {
        return "Goodbye!\n";
    }

    stringstream ss(cmd); // Use stringstream to parse the command
    string keyword;
    ss >> keyword;

    if (keyword == "Newgraph")
    {

        if (!client_points[client_fd].empty())
        {
            return "ERROR: A graph already exists. Remove all points first to create a new one.\n";
        }

        int n;
        if (!(ss >> n) || n < 3)
        {
            return "ERROR: Invalid Newgraph format. Must be at least 3 points\n";
        }

        client_points[client_fd].clear();
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
            char extra;
            if (!(ps >> x >> comma >> y) || comma != ',' || ps >> extra)
            {
                string err = "ERROR: Invalid format at point " + to_string(i + 1) + " (expected x,y)\n";
                send(client_fd, err.c_str(), err.size(), 0);
                continue;
            }

            pair<float, float> new_point = {x, y};
            if (std::find(client_points[client_fd].begin(), client_points[client_fd].end(), new_point) != client_points[client_fd].end())
            {
                string err = "ERROR: Duplicate point detected at point " + to_string(i + 1) + "\n";
                send(client_fd, err.c_str(), err.size(), 0);
                continue;
            }

            client_points[client_fd].push_back(new_point);
            string ack = "Server: Point " + to_string(i + 1) + " received\n";
            send(client_fd, ack.c_str(), ack.size(), 0);
            ++i;
        }

        ostringstream oss;
        oss << fixed << setprecision(2);
        oss << "Graph initialized with " << client_points[client_fd].size() << " points:\n";
        for (const auto &p : client_points[client_fd])
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
        if (std::find(client_points[client_fd].begin(), client_points[client_fd].end(), new_point) != client_points[client_fd].end())
        {
            return "ERROR: Point already exists\n";
        }
        client_points[client_fd].push_back(new_point);

        ostringstream oss;
        oss << "Point added\n";
        oss << fixed << setprecision(2);
        for (const auto &p : client_points[client_fd])
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

        auto it = std::find(client_points[client_fd].begin(), client_points[client_fd].end(), make_pair(x, y));
        if (it != client_points[client_fd].end())
        {
            client_points[client_fd].erase(it);
            ostringstream oss;
            oss << "Point removed\n";
            oss << fixed << setprecision(2);
            for (const auto &p : client_points[client_fd])
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
        if (client_points[client_fd].size() < 3)
            return "0\n";
        float area = CHArea_vec(client_points[client_fd]);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << area;
        return "Area: " + oss.str() + "\n";
    }

    else
    {
        return "ERROR: Unknown command\n";
    }
}




/**
 * Using a mutex to protect the access to the shared points vector:
 * for preventing crushing of the points vector by multiple threads
 * When couple of clients are sending commands at the same time, we 
 * need to ensure that everyone does not change the points vector at the same time.
 */

mutex points_mutex;

void handle_client_event(int client_fd) {
    static thread_local int thread_number = thread_counter++;



    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int nbytes = recv(client_fd, buffer, sizeof(buffer), 0);

    if (nbytes <= 0) {
        if (nbytes == 0) {
            cout << "Socket " << client_fd << " disconnected" << endl;
        } else {
            perror("recv");
        }

        close(client_fd);
        lock_guard<mutex> lock(points_mutex);
        client_points.erase(client_fd);
        return;
    }

    cout << "[Thread " << thread_number << "] Received command on socket " << client_fd << ": " << buffer << endl;

    string response;
    {
        lock_guard<mutex> lock(points_mutex);
        response = process_command(string(buffer, nbytes), client_fd);
    }

    cout << "[Thread " << thread_number << "] Sending response: " << response << endl;
    send(client_fd, response.c_str(), response.size(), 0);

    string cmd_str(buffer, nbytes);
    if (cmd_str.find("exit") != string::npos) {
        close(client_fd);
        lock_guard<mutex> lock(points_mutex);
        client_points.erase(client_fd);
    }
}


int main(){

    int listener = setup_server_socket(); // Create the server socket

    cout << "Server listening on port " << PORT << "..." << endl;


    while (true){

        int thread_number=thread_counter++;
        
        sockaddr_storage client_addr; // Structure to hold client address
        socklen_t addrlen=sizeof client_addr;

        int new_fd = accept(listener, (sockaddr*)&client_addr, &addrlen); // Accept a new connection
        if(new_fd==-1){
            perror("accept");
            continue; // If accept fails, continue to the next iteration
        }

        cout<<"New connection on socket "<<new_fd<<endl;

        string welcome =
            "Welcome to the Convex Hull Server (Proactor version)!\n"
            "Available commands:\n"
            "  Newgraph N       - Create a new graph with N points (each in x,y)\n"
            "  Newpoint x,y     - Add a point to the graph\n"
            "  Removepoint x,y  - Remove a point from the graph\n"
            "  CH               - Compute the convex hull area\n"
            "  exit             - Disconnect from the server\n";
        send(new_fd, welcome.c_str(), welcome.size(), 0);

        
        thread t = startProactor(new_fd, handle_client_event);
        t.detach();

    }
    return 0;
}
