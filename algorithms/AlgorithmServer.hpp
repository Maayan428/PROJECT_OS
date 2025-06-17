#ifndef ALGORITHM_SERVER_HPP
#define ALGORITHM_SERVER_HPP

#include "../graphs/Graph.hpp"
#include <string>

/**
 * @brief Class for the algorithm server that reads a graph and an algorithm name from a client,
 * applies the requested algorithm using the Strategy and Factory patterns, and returns the result.
 */
class AlgorithmServer {
public:
    explicit AlgorithmServer(int port = 9091);  // Default port can be changed
    void start();  // Starts the server loop

private:
    int port_;

    void handleClient(int client_fd);  // Logic for handling a single client
    Graph receiveGraph(int client_fd, int& num_vertices); // Reads graph definition from client
    std::string receiveAlgorithmName(int client_fd); // Reads algorithm name from client
    void sendMessage(int client_fd, const std::string& msg); // Sends message to client
};

#endif // ALGORITHM_SERVER_HPP