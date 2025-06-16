#include <iostream>
#include <unistd.h>   // for getopt
#include <cstdlib>    // for srand, rand, atoi
#include "Graph.hpp"
#include "EulerCircle.hpp"  // keep the custom header name

/**
 * @brief Prints usage instructions for the program.
 */
void printUsage(const char* progName) {
    std::cerr << "Usage: " << progName << " -v <number_of_vertices> -e <number_of_edges> -s <random_seed>\n";
}

int main(int argc, char* argv[]) {
    int number_of_vertices = -1;
    int number_of_edges = -1;
    int random_seed = 0;

    // Parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "v:e:s:")) != -1) {
        switch (opt) {
            case 'v':
                number_of_vertices = std::atoi(optarg);
                break;
            case 'e':
                number_of_edges = std::atoi(optarg);
                break;
            case 's':
                random_seed = std::atoi(optarg);
                break;
            default:
                printUsage(argv[0]);
                return 1;
        }
    }

    // Validate arguments
    if (number_of_vertices <= 0 || number_of_edges < 0) {
        printUsage(argv[0]);
        return 1;
    }

    // Check if the number of edges is valid for a simple undirected graph
    if (number_of_edges > number_of_vertices * (number_of_vertices - 1) / 2) {
        std::cerr << "Too many edges for a simple undirected graph.\n";
        return 1;
    }

    // Initialize random number generator
    srand(random_seed);

    // Create a graph and add random edges
    Graph graph(number_of_vertices);
    int edgesAdded = 0;
    while (edgesAdded < number_of_edges) {
        int u = rand() % number_of_vertices;
        int v = rand() % number_of_vertices;

        if (u != v && !graph.edgeExists(u, v)) {
            graph.addEdge(u, v);
            edgesAdded++;
        }
    }

    // Print the generated graph
    std::cout << "\nGenerated Graph:\n";
    graph.print();

    // Attempt to find an Eulerian circuit
    try {
        if (EulerCircle::isEulerian(graph)) {
            std::vector<int> circuit = EulerCircle::getEulerianCircuit(graph);
            std::cout << "\nEulerian circuit found:\n";
            for (int node : circuit) {
                std::cout << node << " ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "\nGraph is not Eulerian.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
    }

    return 0;
}