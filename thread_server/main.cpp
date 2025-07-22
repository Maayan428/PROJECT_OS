#include "ThreadServer.hpp"
#include <iostream>

int main() {
    int port = 9090; 
    ThreadServer server(port);
    server.start();
    return 0;
}