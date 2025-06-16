#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <string>
#include <netinet/in.h>

/**
 * @brief Server handles socket communication to receive graph parameters
 *        from clients, generate a graph, and respond with the adjacency matrix
 *        and whether an Eulerian circuit exists.
 */
class Server {
private:
    int port;
    int server_fd;
    sockaddr_in address;

    void setupSocket();

    void handleClient(int client_fd);

    void parseRequest(const std::string& request, int& vertices, int& edges, int& seed);

public:
   
    explicit Server(int port);

    void run();

    ~Server();
};

#endif // TCPSERVER_HPP