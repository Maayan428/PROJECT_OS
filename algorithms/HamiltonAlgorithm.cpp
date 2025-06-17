#include "HamiltonAlgorithm.hpp"
#include <vector>
#include <sstream>

/**
 * @brief Recursive utility function to check if a Hamiltonian cycle exists in the graph.
 *
 * This function attempts to construct a Hamiltonian cycle by exploring
 * valid paths recursively using backtracking.
 *
 * @param graph The graph to search in.
 * @param path The current path being built.
 * @param visited A boolean array indicating which vertices have been visited.
 * @param pos The current position in the path to fill.
 * @return true if a Hamiltonian cycle is found, false otherwise.
 */
bool HamiltonAlgorithm::isHamiltonianUtil(const Graph& graph, std::vector<int>& path, std::vector<bool>& visited, int pos) {
    int n = graph.getSize();
    
    // Base case: all vertices are in the path
    if (pos == n) {
        // Check if the last vertex connects to the starting vertex
        return graph.edgeExists(path[pos - 1], path[0]);
    }

    for (int v = 1; v < n; ++v) { // Try all vertices except the starting one (0)
        // If vertex v is connected to the last vertex in path and not yet visited
        if (graph.edgeExists(path[pos - 1], v) && !visited[v]) {
            path[pos] = v;
            visited[v] = true;

            // Recurse to build the next position in path
            if (isHamiltonianUtil(graph, path, visited, pos + 1))
                return true;

            // Backtrack
            visited[v] = false;
        }
    }

    return false;
}

/**
 * @brief Executes the Hamiltonian cycle algorithm on the given graph.
 *
 * Tries to determine whether the graph contains a Hamiltonian cycle
 * and returns a description of the result as a string.
 *
 * @param graph The input graph.
 * @return A string indicating whether a Hamiltonian circuit exists, and if so, the path.
 */
std::string HamiltonAlgorithm::run(const Graph& graph) {
    int n = graph.getSize();
    if (n == 0)
        return "Graph is empty.\n";

    std::vector<int> path(n, -1);      // Path to store Hamiltonian cycle
    std::vector<bool> visited(n, false); // Track visited vertices
    path[0] = 0; // Start from vertex 0
    visited[0] = true;

    std::ostringstream out;
    if (isHamiltonianUtil(graph, path, visited, 1)) {
        out << "Hamiltonian Circuit Found:\n";
        for (int i = 0; i < n; ++i)
            out << path[i] << " ";
        out << path[0] << "\n"; // Close the cycle
    } else {
        out << "No Hamiltonian Circuit exists.\n";
    }

    return out.str();
}