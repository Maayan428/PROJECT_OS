#include "MaxCliqueAlgorithm.hpp"
#include <sstream>
#include <algorithm>

/**
 * @brief Executes the Max Clique algorithm on the given graph.
 *
 * This function initializes the search for the largest complete subgraph (clique)
 * by recursively checking all combinations of nodes and comparing their connectivity.
 *
 * @param graph The input graph to analyze.
 * @return A string describing the size and members of the maximum clique.
 */
std::string MaxCliqueAlgorithm::run(const Graph& graph) {
    int n = graph.getSize();
    maxClique.clear(); // Clear previous results
    std::vector<int> current; // Holds current subset being tested

    findMaxClique(graph, current, 0); // Start recursive search from node 0

    std::ostringstream out;
    out << "Maximum Clique Size: " << maxClique.size() << "\n";
    out << "Clique Members: ";
    for (int v : maxClique)
        out << v << " ";
    out << "\n";
    return out.str();
}

/**
 * @brief Checks if a given set of vertices forms a clique.
 *
 * A clique is a subset of vertices such that every two vertices are connected.
 *
 * @param graph The graph in which to check the subset.
 * @param nodes The subset of vertices to test.
 * @return true if the subset forms a clique, false otherwise.
 */
bool MaxCliqueAlgorithm::isClique(const Graph& graph, const std::vector<int>& nodes) {
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = i + 1; j < nodes.size(); ++j) {
            // If any two nodes are not connected, it's not a clique
            if (!graph.edgeExists(nodes[i], nodes[j]))
                return false;
        }
    }
    return true;
}

/**
 * @brief Recursively searches for the maximum clique in the graph using backtracking.
 *
 * This function explores all subsets of vertices starting from a given index,
 * and keeps track of the largest clique found.
 *
 * @param graph The input graph.
 * @param current The current subset being built.
 * @param start The index to start adding new vertices from.
 */
void MaxCliqueAlgorithm::findMaxClique(const Graph& graph, std::vector<int>& current, int start) {
    int n = graph.getSize();

    // Update maxClique if the current one is larger
    if (current.size() > maxClique.size())
        maxClique = current;

    for (int i = start; i < n; ++i) {
        current.push_back(i);
        if (isClique(graph, current)) {
            // Recurse to try to grow the clique
            findMaxClique(graph, current, i + 1);
        }
        current.pop_back(); // Backtrack
    }
}