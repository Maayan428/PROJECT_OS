#include "AlgorithmServer.hpp"
#include "AlgorithmFactory.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <random>
#include <set>
#include <csignal> // For signal handling

#define BUFFER_SIZE 2048

// === Signal handling ===
volatile sig_atomic_t stop_flag = 0;

void signal_handler(int signum) {
    stop_flag = 1;
}

/**
 * @brief Constructs the AlgorithmServer with a specified port.
 * @param port Port number the server should listen on.
 */
AlgorithmServer::AlgorithmServer(int port) : port_(port) {}

/**
 * @brief Sends a message to the client.
 * @param client_fd File descriptor of the client socket.
 * @param msg The message to send.
 */
void AlgorithmServer::sendMessage(int client_fd, const std::string& msg) {
    send(client_fd, msg.c_str(), msg.size(), 0);
}

std::string receiveLine(int client_fd) {
    std::string input;
    char ch;

    while (true) {
        ssize_t bytes_read = recv(client_fd, &ch, 1, 0);
        if (bytes_read <= 0) break;
        if (ch == '\n') break;
        input += ch;
    }

    // Trim end
    input.erase(input.find_last_not_of(" \r\t") + 1);
    return input;
}

/**
 * @brief Receives graph specifications from the client and generates a random graph accordingly.
 * @param client_fd File descriptor of the client socket.
 * @param num_vertices Output parameter for number of vertices.
 * @return The generated Graph object.
 * @throws std::runtime_error if input is invalid or connection fails.
 */
Graph AlgorithmServer::receiveGraph(int client_fd, int& num_vertices) {
    sendMessage(client_fd,
        "Welcome to the Algorithm Server!\n"
        "----------------------------------\n"
        "Please enter a request in the following format:\n"
        "   <vertices> <edges> <random_seed>\n"
        "Example: 6 10 42\n"
        "- vertices: number of nodes in the graph (must be > 0)\n"
        "- edges: number of edges (>= 0)\n"
        "- random_seed: any integer for reproducibility\n"
        "Type 'exit' or 'q' to quit.\n\n> ");

    std::string input = receiveLine(client_fd);
    if (input == "exit" || input == "q")
        throw std::runtime_error("Client exited");

    std::istringstream iss(input);
    int num_edges, seed;
    if (!(iss >> num_vertices >> num_edges >> seed) || num_vertices <= 0 || num_edges < 0)
        throw std::runtime_error("Invalid input format. Usage: <vertices> <edges> <random_seed>");

    Graph g(num_vertices);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, num_vertices - 1);
    std::set<std::pair<int, int>> edge_set;

    while ((int)edge_set.size() < num_edges) {
        int u = dist(rng);
        int v = dist(rng);
        if (u != v) {
            int a = std::min(u, v);
            int b = std::max(u, v);
            if (edge_set.insert({a, b}).second) {
                g.addEdge(a, b);
            }
        }
    }

    sendMessage(client_fd, "Graph created successfully!\n");
    return g;
}

std::string AlgorithmServer::receiveAlgorithmName(int client_fd) {
    sendMessage(client_fd,
        "\nNow enter the name of the algorithm you want to run (options: mst, hamilton, scc, maxclique):\n> ");
    std::string input = receiveLine(client_fd);
    return input;
}

/**
 * @brief Handles a single client connection, including graph creation and algorithm execution.
 * @param client_fd File descriptor of the client socket.
 */
void AlgorithmServer::handleClient(int client_fd) {
    try {
        sendMessage(client_fd, "Welcome to the Algorithm Server!\n===============================\n");

        while (!stop_flag) {
            int num_vertices = -1;
        
            Graph g;
            try {
                g = receiveGraph(client_fd, num_vertices);
            } catch (const std::exception& ex) {
                if (std::string(ex.what()) == "Client exited")
                    break;  // Client requested exit
                throw;
            }
        
            std::string algo = receiveAlgorithmName(client_fd);
            if (algo == "exit" || algo == "q")
                break;
        
            auto strategy = AlgorithmFactory::create(algo);
            std::string result = strategy->run(g);
            sendMessage(client_fd, "\nResult:\n" + result + "\n");
        
            sendMessage(client_fd,
                "\nYou may enter a new graph and algorithm now, or type 'exit' to quit.\n");
        }

    } catch (const std::exception& ex) {
        sendMessage(client_fd, std::string("Error: ") + ex.what() + "\n");
    }

    close(client_fd);
}

/**
 * @brief Starts the algorithm server, listens for connections, and handles clients sequentially.
 */
void AlgorithmServer::start() {
    // Register signal handler for graceful shutdown
    signal(SIGINT, signal_handler);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        return;
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        return;
    }

    std::cout << "Algorithm Server listening on port " << port_ << "...\n";

    while (!stop_flag) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            if (stop_flag) break;  // Graceful shutdown
            perror("accept failed");
            continue;
        }

        handleClient(client_fd);
    }

    close(server_fd);
    std::cout << "\nServer shut down gracefully.\n";
}