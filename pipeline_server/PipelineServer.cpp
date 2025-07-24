#include "PipelineServer.hpp"
#include "../algorithms/AlgorithmFactory.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <set>
#include <random>
#include <thread>
#include <chrono>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "PipelineStage.hpp"

/**
 * @brief Constructor for the PipelineServer.
 * @param port The port on which the server will listen.
 */
PipelineServer::PipelineServer(int port)
    : port_(port), stop_flag(false) {}

/**
 * @brief Sends a message over a socket.
 * @param fd The socket file descriptor.
 * @param msg The message string to send.
 */
void PipelineServer::sendMessage(int fd, const std::string& msg) {
    send(fd, msg.c_str(), msg.size(), 0);
}

/**
 * @brief Receives a line of text from the socket, ending at newline.
 * @param fd The socket file descriptor.
 * @return A string containing the received line.
 */
std::string PipelineServer::receiveLine(int fd) {
    std::string input;
    char ch;
    while (recv(fd, &ch, 1, 0) > 0 && ch != '\n') {
        input += ch;
    }
    return input;
}

/**
 * @brief Interacts with the client to receive parameters for a random graph.
 * @param client_fd The client's socket file descriptor.
 * @param num_vertices Reference to store the number of vertices.
 * @return A Graph object created with the given parameters.
 * @throws std::runtime_error if client requests disconnection or shutdown.
 */
Graph PipelineServer::receiveGraph(int client_fd, int& num_vertices) {
    while (true) {
        sendMessage(client_fd,
            "Enter: <vertices> <edges> <seed>\n"
            "(Type 'exit' to disconnect, 'q' to shutdown)\n> ");
        
        std::string line = receiveLine(client_fd);

        if (line == "exit") {
            sendMessage(client_fd, "Goodbye! Disconnecting...\n");
            std::cout << "[Client] Disconnected by request." << std::endl;
            throw std::runtime_error("Client exited");
        }

        if (line == "q") {
            sendMessage(client_fd, "Shutdown signal received. Server will terminate...\n");
            std::cout << "[Client] Requested server shutdown." << std::endl;
            stop_flag.store(true);
            wakeUpServer();
            throw std::runtime_error("Shutdown requested");
        }

        int edges, seed;
        std::istringstream iss(line);
        if (!(iss >> num_vertices >> edges >> seed)) {
            sendMessage(client_fd, "Invalid input. Please try again.\n");
            continue;
        }

        Graph g(num_vertices);
        std::mt19937 rng(seed);
        std::set<std::pair<int, int>> edge_set;
        std::uniform_int_distribution<int> dist(0, num_vertices - 1);

        // Randomly generate unique edges
        while ((int)edge_set.size() < edges) {
            int u = dist(rng), v = dist(rng);
            // Ensure u ≠ v and that the edge is unique
            if (u != v && edge_set.emplace(std::min(u, v), std::max(u, v)).second)
                g.addEdge(u, v);
        }

        std::cout << "[Client] Graph received: " << num_vertices << " vertices, " << edges << " edges." << std::endl;
        return g;
    }
}

/**
 * @brief Handles communication with a single connected client.
 *        Receives graph input and runs it through 4 algorithms in parallel stages.
 * @param client_fd The client's socket file descriptor.
 */
void PipelineServer::handleClient(int client_fd) {
    try {
        while (true) {
            int num_vertices;
            Graph g = receiveGraph(client_fd, num_vertices);

            std::vector<std::string> names = {"mst", "hamilton", "scc", "maxclique"};
            std::vector<std::unique_ptr<PipelineStage>> stages;
            for (int i = 0; i < 4; ++i)
                stages.push_back(std::make_unique<PipelineStage>());

            for (int i = 0; i < 4; ++i) {
                stages[i]->submit([&, i]() {
                    try {
                        auto strategy = AlgorithmFactory::create(names[i]);
                        std::string result = strategy->run(g);
                        sendMessage(client_fd, "[" + names[i] + "] Result:\n" + result + "\n");
                    } catch (...) {
                        sendMessage(client_fd, "[" + names[i] + "] Error.\n");
                    }
                });
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Allow time for all threads to finish
            sendMessage(client_fd, "Finished all stages.\n\n");
        }
    } catch (const std::exception& e) {
        std::cout << "[Client] Exception: " << e.what() << std::endl;
        sendMessage(client_fd, "Session ended.\n");
    }

    close(client_fd);
    std::cout << "[Client] Connection closed.\n";
}

/**
 * @brief Wakes up the server by connecting to itself, used for clean shutdown.
 */
void PipelineServer::wakeUpServer() {
    int dummy = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(dummy, (sockaddr*)&addr, sizeof(addr));
    close(dummy);
}

/**
 * @brief Starts the server, listens for client connections and delegates handling to threads.
 */
void PipelineServer::start() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        return;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        return;
    }

    std::cout << "Pipeline Server listening on port " << port_ << "...\n";

    while (!stop_flag.load()) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);

        if (client_fd < 0) {
            if (stop_flag.load()) break;  // Graceful exit
            perror("accept failed");
            continue;
        }

        std::thread(&PipelineServer::handleClient, this, client_fd).detach();  // Handle client in separate thread
    }

    close(server_fd);
    std::cout << "Server shut down gracefully.\n";
}