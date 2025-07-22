#include "PipelineServer.hpp"
#include <iostream>

int main() {
    int port = 8081;
    PipelineServer server(port);
    server.start();
    return 0;
}