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

PipelineServer::PipelineServer(int port) : port_(port), stop_flag(false) {}

void PipelineServer::sendMessage(int fd, const std::string& msg) {
    send(fd, msg.c_str(), msg.size(), 0);
}

std::string PipelineServer::receiveLine(int fd) {
    std::string input;
    char ch;
    while (recv(fd, &ch, 1, 0) > 0 && ch != '\n') input += ch;
    return input;
}

Graph PipelineServer::receiveGraph(int client_fd, int& num_vertices) {
    sendMessage(client_fd, "Enter: <vertices> <edges> <seed>\n> ");
    std::string line = receiveLine(client_fd);
    int edges, seed;
    std::istringstream iss(line);
    if (!(iss >> num_vertices >> edges >> seed)) throw std::runtime_error("Invalid input");

    Graph g(num_vertices);
    std::mt19937 rng(seed);
    std::set<std::pair<int, int>> edge_set;
    std::uniform_int_distribution<int> dist(0, num_vertices - 1);

    while ((int)edge_set.size() < edges) {
        int u = dist(rng), v = dist(rng);
        if (u != v && edge_set.emplace(std::min(u,v), std::max(u,v)).second)
            g.addEdge(u, v);
    }

    return g;
}

void PipelineServer::handleClient(int client_fd) {
    try {
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

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        sendMessage(client_fd, "Finished all stages.\n");

    } catch (...) {
        sendMessage(client_fd, "Error or client disconnected.\n");
    }
    close(client_fd);
}

void PipelineServer::wakeUpServer() {
    int dummy = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(dummy, (sockaddr*)&addr, sizeof(addr));
    close(dummy);
}

void PipelineServer::start() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);
    std::cout << "Pipeline Server listening on port " << port_ << "...\n";

    while (!stop_flag.load()) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        if (client_fd >= 0) std::thread(&PipelineServer::handleClient, this, client_fd).detach();
    }
    close(server_fd);
}
