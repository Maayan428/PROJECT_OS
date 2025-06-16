#include "Server.hpp"

int main() {
    Server server(9090);  // Set the port
    server.run();         // Start the server loop
    return 0;
}