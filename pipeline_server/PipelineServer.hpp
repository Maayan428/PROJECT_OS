#pragma once
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include "../graphs/Graph.hpp"

class PipelineServer {
public:
    PipelineServer(int port); // Constructs the server with the specified port.
    void start();

private:
    int port_;
    std::atomic<bool> stop_flag; // Flag to signal the server to stop
    void handleClient(int client_fd);  // Handles a single connected client in a dedicated thread.
    void sendMessage(int client_fd, const std::string& msg);
    std::string receiveLine(int client_fd);
    Graph receiveGraph(int client_fd, int& num_vertices);
    void wakeUpServer(); // Wakes up the server by making a dummy connection to itself.
};