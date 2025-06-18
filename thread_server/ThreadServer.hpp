#pragma once
#include <string>
#include "../graphs/Graph.hpp"

class ThreadServer {
public:
    ThreadServer(int port);
    void start();

private:
    int port_;
    void sendMessage(int client_fd, const std::string& msg);
    std::string receiveLine(int client_fd);
    Graph receiveGraph(int client_fd, int& num_vertices);
    void handleClient(int client_fd);

};