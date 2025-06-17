#include "../server_client/Server.hpp"
#include "../graphs/Graph.hpp"
#include "../graphs/EulerCircle.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <arpa/inet.h>

#define BACKLOG 5
#define BUFFER_SIZE 1024

/**
 * @brief Constructor initializing server with a specific port.
 * 
 * @param port The port number to bind the server.
 */
Server::Server(int port) : port(port), server_fd(-1) {
    memset(&address, 0, sizeof(address));
}

/**
 * @brief Destructor to close the socket if open.
 */
Server::~Server() {
    if (server_fd != -1)
        close(server_fd);
}

/**
 * @brief Sets up the TCP socket and binds it to the specified port.
 * 
 * @throws std::runtime_error on socket or bind failure.
 */
void Server::setupSocket() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        throw std::runtime_error("Failed to create socket");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
        throw std::runtime_error("Failed to bind socket");

    if (listen(server_fd, BACKLOG) < 0)
        throw std::runtime_error("Failed to listen on socket");
}

/**
 * @brief Parses the request string and extracts graph parameters.
 * 
 * @param request The string received from the client.
 * @param vertices Output: number of vertices.
 * @param edges Output: number of edges.
 * @param seed Output: seed for random generator.
 * @throws std::invalid_argument if parsing fails or values are invalid.
 */
void Server::parseRequest(const std::string& request, int& vertices, int& edges, int& seed) {
    std::istringstream iss(request);
    if (!(iss >> vertices >> edges >> seed))
        throw std::invalid_argument("Invalid input format. Usage: <vertices> <edges> <seed>");

    if (vertices <= 0 || edges < 0)
        throw std::invalid_argument("Vertices must be > 0 and edges >= 0");
}

/**
 * @brief Handles a client connection: receives request, generates graph,
 * computes Eulerian circuit (if any), and sends response.
 * 
 * @param client_fd The client's socket descriptor.
 */
void Server::handleClient(int client_fd) {
    const char* welcomeMsg =
        "Welcome to the Euler Graph Server!\n"
        "----------------------------------\n"
        "Please enter a request in the following format:\n"
        "   <vertices> <edges> <random_seed>\n"
        "Example: 6 10 42\n"
        "- vertices: number of nodes in the graph (must be > 0)\n"
        "- edges: number of edges (>= 0)\n"
        "- random_seed: any integer for reproducibility\n"
        "Type 'exit' or 'q' to quit.\n\n";

    send(client_fd, welcomeMsg, strlen(welcomeMsg), 0);

    char buffer[BUFFER_SIZE];

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);

        if (bytes_read == 0) {
            std::cout << "Client disconnected gracefully.\n";
            break;
        } else if (bytes_read < 0) {
            std::cerr << "Client disconnected unexpectedly (error during read).\n";
            break;
        }

        std::string input(buffer);
        input.erase(input.find_last_not_of(" \n\r\t") + 1); // Trim

        if (input == "exit" || input == "q") {
            std::cout << "Client requested exit.\n";
            send(client_fd, "Goodbye!\n", 9, 0);
            break;
        }

        std::ostringstream response;
        int vertices, edges, seed;

        try {
            parseRequest(input, vertices, edges, seed);

            Graph g(vertices);
            g.generateRandomUndirected(edges, seed);

            response << "Generated Graph:\n";
            g.printAdjacencyMatrix(response);

            if (EulerCircle::isEulerian(g)) {
                std::vector<int> circuit = EulerCircle::getEulerianCircuit(g);
                response << "\nEulerian Circuit Found:\n";
                for (int v : circuit) response << v << " ";
                response << "\n";
            } else {
                response << "\nGraph is not Eulerian.\n";
            }
        } catch (const std::exception& ex) {
            response << "Error: " << ex.what() << "\n";
        }

        std::string out = response.str();
        send(client_fd, out.c_str(), out.size(), 0);
    }

    close(client_fd);
    std::cout << "Closed connection with client.\n";
}

/**
 * @brief Runs the main server loop to accept and handle client connections.
 */
void Server::run() {
    setupSocket();
    std::cout << "Server listening on port " << port << "...\n";

    while (true) {
        socklen_t addrlen = sizeof(address);
        int client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client_fd < 0) {
            std::cerr << "Error accepting connection.\n";
            continue;
        }

        handleClient(client_fd);
    }
}