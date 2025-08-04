#include "Proactor.hpp"
#include "ConvexHull.hpp"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iomanip>


using namespace std;

/*
client_points : every client identified via socket fd , and has vector
with points.
points_mutex : using lock_guard when we call/write to client_points.
*/

unordered_map<int, vector<pair<float,float>>> client_points;
mutex points_mutex; 

//Maps each client socket to the number of remaining points to receive after a Newgraph command:
unordered_map<int, int> expected_points;


/*
The method process_command receives a command from the 
client (cmd) and its connection identifier (client_fd), 
and returns the appropriate response as a string.
*/


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


/*
This method is the callback method that get sent to the startProactor.
Every time the select identify data in the socket of 
any client-->this method will be called.
Reads the command from the client,Processes 
it (for example: Newgraph, Newpoint, CH...)
Sends a response. 
If the command is 'exit' – closes the connection
*/

void handle_client(int client_fd) {

    //Temporarily allocates an array of 1024 characters for input from the client:
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));


    //recv try to read data from the socket of the client
    //If the client sent command-->we get that here:
    int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes <= 0) {
        close(client_fd);
        return;
    }

    /*
    We will use this variable to store the response we return to
    the client.
    A block protected by lock_guard for mutex:
    Ensures that only one thread (or in our case: one callback) can
    modify client_points at any given moment.process_command(...):
    Processes the command (for example Newgraph, CH...)Returns the response as a string.
    */
    string response;

    {
        lock_guard<mutex> lock(points_mutex);
        response = process_command(string(buffer, bytes), client_fd);
    }

    //Send the response to the client via the same socket:
    send(client_fd, response.c_str(), response.size(), 0);

    if (response.find("Goodbye") != string::npos) {
        lock_guard<mutex> lock(points_mutex);
        client_points.erase(client_fd);
        close(client_fd);
    }
}


/*
There is a method, that receives the socket that
listens for new connections (listener_fd). 
It will be called by the Proactor every time a new client 
attempts to connect.
*/

void accept_connection(int listener_fd, std::vector<std::thread>& client_threads){

    //For get the IP address of the client:
    sockaddr_in client_addr{};
    socklen_t addr_size=sizeof(client_addr);

    //The accept get the connection from the client, and get the client_fd:
    int client_fd=accept(listener_fd, (sockaddr*)&client_addr, &addr_size);
    if(client_fd<0){
        perror("accept");
        return;
    }

     cout << "New client connected: socket " << client_fd << endl;

    string welcome = 
        "Welcome to the Convex Hull Server (Proactor version)!\n"
        "Commands:\n"
        "  Newgraph N\n"
        "  Newpoint x,y\n"
        "  Removepoint x,y\n"
        "  CH\n"
        "  exit\n";

    //Send the opening message to the client after they have logged in:
    send(client_fd, welcome.c_str(), welcome.length(), 0);

    //Operates Proactor on the new client:
    client_threads.push_back(startProactor(client_fd, handle_client));
}

int main(){

    std::vector<std::thread> client_threads; //store threads to join later


    //Create a socket for listening:
    int listener_fd=socket(AF_INET,SOCK_STREAM,0);
    if(listener_fd<0){
        perror("socket");
        return 1;
    }

    // Set socket options to reuse the port immediately after the server closes:
    int opt = 1;
    setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //Prepare the address structure:
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;       // Listen on all interfaces
    server_addr.sin_port = htons(9034);             // Beej’s magic port

    // Bind the socket to the address and port
    if (bind(listener_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(listener_fd);
        return 1;
    }

    if (listen(listener_fd, SOMAXCONN) < 0) {
        perror("listen");
        close(listener_fd);
        return 1;
    }

    cout << "Proactor server is running on port 9034..." << endl;

    // Loop forever and accept new clients using select:
while (true) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(listener_fd, &readfds);

    int activity = select(listener_fd + 1, &readfds, nullptr, nullptr, nullptr);
    if (activity < 0) {
        perror("select");
        break;
    }

    if (FD_ISSET(listener_fd, &readfds)) {
        accept_connection(listener_fd, client_threads);
    }
}

for (auto& t : client_threads) {
    if (t.joinable()) t.join();
}


    return 0;

}