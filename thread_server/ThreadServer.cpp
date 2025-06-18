#include "ThreadServer.hpp"
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
#include <thread>
#include <csignal>

#define BUFFER_SIZE 2048

volatile sig_atomic_t stop_flag = 0;

void signal_handler(int signum) {
    stop_flag = 1;
}

/**
 * @brief Constructs the multithreaded server with a specified port.
 * @param port Port number the server should listen on.
 */
ThreadServer::ThreadServer(int port) : port_(port) {}

/**
 * @brief Sends a message to the client.
 * @param client_fd File descriptor of the client socket.
 * @param msg The message to send.
 */
void ThreadServer::sendMessage(int client_fd, const std::string& msg) {
    send(client_fd, msg.c_str(), msg.size(), 0);
}

/**
 * @brief Receives a single line of text from the client.
 * @param client_fd File descriptor of the client socket.
 * @return The received line as a string.
 */
std::string ThreadServer::receiveLine(int client_fd) {
    std::string input;
    char ch;

    while (true) {
        ssize_t bytes_read = recv(client_fd, &ch, 1, 0);
        if (bytes_read <= 0) break;
        if (ch == '\n') break;
        input += ch;
    }

    // Trim trailing whitespace
    input.erase(input.find_last_not_of(" \r\t") + 1);
    return input;
}

/**
 * @brief Receives graph parameters from client and builds a random graph.
 * @param client_fd File descriptor of the client socket.
 * @param num_vertices Output number of vertices.
 * @return The generated Graph object.
 * @throws std::runtime_error if input is invalid or on disconnect.
 */
Graph ThreadServer::receiveGraph(int client_fd, int& num_vertices) {
    sendMessage(client_fd,
        "Welcome to the Multithreaded Algorithm Server!\n"
        "----------------------------------------------\n"
        "Please enter a request in the following format:\n"
        "   <vertices> <edges> <random_seed>\n"
        "Example: 6 10 42\n"
        "- vertices: number of nodes (> 0)\n"
        "- edges: number of edges (>= 0)\n"
        "- random_seed: integer for reproducibility\n"
        "Type 'exit' or 'q' to quit.\n\n> ");

    std::string input = receiveLine(client_fd);
    if (input == "exit" || input == "q") {
        sendMessage(client_fd, "Goodbye! Disconnecting...\n");
        shutdown(client_fd, SHUT_RDWR);
        close(client_fd);
        std::cout << "[Thread] Client disconnected or requested shutdown.\n";
        throw std::runtime_error("Client exited");
    }

    int num_edges, seed;
    std::istringstream iss(input);
    if (!(iss >> num_vertices >> num_edges >> seed) || num_vertices <= 0 || num_edges < 0)
        throw std::runtime_error("Invalid input format. Expected: <vertices> <edges> <random_seed>");

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

    sendMessage(client_fd, "Graph created successfully! Running algorithms...\n");
    return g;
}

/**
 * @brief Handles a single client connection by running all algorithms and returning results.
 * @param client_fd File descriptor of the client socket.
 */
void ThreadServer::handleClient(int client_fd) {
    try {
        int num_vertices = -1;
        Graph g = receiveGraph(client_fd, num_vertices);

        std::vector<std::string> algos = {"mst", "hamilton", "scc", "maxclique"};
        for (const auto& algo : algos) {
            try {
                auto strategy = AlgorithmFactory::create(algo);
                std::string result = strategy->run(g);
                sendMessage(client_fd, "\n[" + algo + "] Result:\n" + result + "\n");
            } catch (const std::exception& e) {
                sendMessage(client_fd, "Error running " + algo + ": " + e.what() + "\n");
            }
        }

        sendMessage(client_fd, "\nThank you! Connection will now close.\n");
    } catch (...) {
        // Graceful disconnect
    }
    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
}

/**
 * @brief Starts the multithreaded server and listens for client connections.
 */
void ThreadServer::start() {
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

    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        return;
    }

    std::cout << "Multithreaded Algorithm Server listening on port " << port_ << "...\n";

    while (!stop_flag) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            if (stop_flag) break;
            perror("accept failed");
            continue;
        }

        std::thread t(&ThreadServer::handleClient, this, client_fd);
        t.detach(); // Don't wait for the thread to complete
    }

    close(server_fd);
    std::cout << "\nServer shut down gracefully.\n";
}
