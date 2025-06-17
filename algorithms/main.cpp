#include "AlgorithmServer.hpp"

int main() {
    AlgorithmServer server(9091);
    server.start();
    return 0;
}