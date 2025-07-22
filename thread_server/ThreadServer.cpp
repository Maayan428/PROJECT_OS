#include "ThreadServer.hpp"
#include "../algorithms/AlgorithmFactory.hpp"
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
#include <mutex>
#include <atomic>

#define BUFFER_SIZE 2048

std::atomic<bool> stop_flag = false;
std::mutex client_counter_mutex;
std::mutex cout_mutex; // lock for console output
int client_counter = 0;

void signal_handler(int signum) {
    stop_flag.store(true, std::memory_order_seq_cst);
}

ThreadServer::ThreadServer(int port) : port_(port) {}

void ThreadServer::sendMessage(int client_fd, const std::string& msg) {
    send(client_fd, msg.c_str(), msg.size(), 0);
}

std::string ThreadServer::receiveLine(int client_fd) {
    std::string input;
    char ch;

    while (true) {
        ssize_t bytes_read = recv(client_fd, &ch, 1, 0);
        if (bytes_read <= 0) break;
        if (ch == '\n') break;
        input += ch;
    }

    input.erase(input.find_last_not_of(" \r\t") + 1);
    return input;
}

Graph ThreadServer::receiveGraph(int client_fd, int& num_vertices, int client_id) {
    while (true) {
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
        if (input == "exit") {
            sendMessage(client_fd, "Goodbye! Disconnecting...\n");
            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "[Client " << client_id << "] Disconnected by request.\n";
            }
            throw std::runtime_error("Client exited");
        } else if (input == "q") {
            sendMessage(client_fd, "Shutdown signal received. Server will terminate...\n");
            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "[Client " << client_id << "] Requested server shutdown.\n";
            }
            stop_flag.store(true);
            wakeUpServer(port_);
            throw std::runtime_error("Shutdown requested");
        }

        int num_edges, seed;
        std::istringstream iss(input);
        if (!(iss >> num_vertices >> num_edges >> seed) || num_vertices <= 0 || num_edges < 0) {
            sendMessage(client_fd, "Invalid input. Try again or type 'exit' to quit.\n\n");
            continue;
        }

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

        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "[Client " << client_id << "] Graph received with " << num_vertices << " vertices and " << num_edges << " edges.\n";
        }

        sendMessage(client_fd, "Graph created successfully! Running algorithms...\n");
        return g;
    }
}

void ThreadServer::handleClient(int client_fd) {
    int client_id;
    {
        std::lock_guard<std::mutex> lock1(client_counter_mutex);
        client_id = ++client_counter;
    }

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "[Client " << client_id << "] Connected.\n";
    }

    try {
        int num_vertices = -1;
        Graph g = receiveGraph(client_fd, num_vertices, client_id);

        std::vector<std::string> algos = {"mst", "hamilton", "scc", "maxclique"};
        for (const auto& algo : algos) {
            try {
                auto strategy = AlgorithmFactory::create(algo);
                std::string result = strategy->run(g);
                sendMessage(client_fd, "\n[" + algo + "] Result:\n" + result + "\n");
            } catch (const std::exception& e) {
                if (std::string(e.what()) == "Shutdown requested") {
                    std::lock_guard<std::mutex> lock(cout_mutex);
                    std::cout << "[Client " << client_id << "] Shutdown request handled.\n";
                }
                sendMessage(client_fd, "Error running " + algo + ": " + e.what() + "\n");
            }
        }

        sendMessage(client_fd, "\nThank you! Connection will now close.\n");

        {
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "[Client " << client_id << "] Processing complete.\n";
        }

    } catch (...) {
        // Already handled above
    }

    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "[Client " << client_id << "] Connection closed.\n";
    }
}

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

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Multithreaded Algorithm Server listening on port " << port_ << "...\n";
    }

    while (!stop_flag.load(std::memory_order_seq_cst)) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            if (stop_flag) break;
            perror("accept failed");
            continue;
        }

        if (stop_flag) {
            close(client_fd);
            break;
        }

        std::thread t(&ThreadServer::handleClient, this, client_fd);
        t.detach();
    }

    close(server_fd);

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "\nServer shut down gracefully.\n";
    }
}

void ThreadServer::wakeUpServer(int port) {
    int dummy = socket(AF_INET, SOCK_STREAM, 0);
    if (dummy < 0) return;

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(dummy, (sockaddr*)&server_addr, sizeof(server_addr));
    close(dummy);
}