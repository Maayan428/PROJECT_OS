#include "EulerCircle.hpp"
#include <stack>
#include <stdexcept>
#include <algorithm>
#include <iostream> // for std::cout

/**
 * @brief Performs a depth-first search on the graph from a given vertex.
 *
 * @param graph The input graph.
 * @param v Current vertex to visit.
 * @param visited Vector tracking visited vertices.
 */
void EulerCircle::dfs(const Graph& graph, int v, std::vector<bool>& visited) {
    visited[v] = true;
    for (int u = 0; u < graph.getSize(); ++u) {
        if (graph.edgeExists(v, u) && !visited[u])
            dfs(graph, u, visited);
    }
}

/**
 * @brief Checks if the graph is connected (ignoring isolated vertices).
 *
 * @param graph The input graph.
 * @return true if connected, false otherwise.
 */
bool EulerCircle::isConnected(const Graph& graph) {
    int n = graph.getSize();
    std::vector<bool> visited(n, false);

    int start = -1;

    // Find a vertex with non-zero degree to start DFS
    for (int i = 0; i < n; ++i) {
        if (graph.degree(i) > 0) {
            start = i;
            break;
        }
    }

    if (start == -1)
        return true; // No edges in the graph – considered connected

    dfs(graph, start, visited);

    // Check if all non-isolated vertices are visited
    for (int i = 0; i < n; ++i) {
        if (graph.degree(i) > 0 && !visited[i])
            return false;
    }

    return true;
}

/**
 * @brief Checks whether the given graph has an Eulerian circuit.
 *
 * A graph has an Eulerian circuit if it is connected and all vertices have even degree.
 *
 * @param graph The input graph.
 * @return true if the graph has an Eulerian circuit, false otherwise.
 */
bool EulerCircle::isEulerian(const Graph& graph) {
    if (!isConnected(graph))
        return false;

    for (int i = 0; i < graph.getSize(); ++i) {
        if (graph.degree(i) % 2 != 0)
            return false;
    }

    return true;
}

/**
 * @brief Computes an Eulerian circuit in the given graph, if one exists.
 *
 * This function modifies a copy of the graph internally to remove edges as it constructs the circuit.
 * Prints the graph before computing for debugging purposes.
 *
 * @param graph A copy of the input graph (passed by value).
 * @return A vector of vertex indices representing the Eulerian circuit.
 * @throws std::logic_error if the graph is empty or does not have an Eulerian circuit.
 */
std::vector<int> EulerCircle::getEulerianCircuit(Graph graph) {
    if (graph.getSize() == 0)
        throw std::logic_error("Graph is empty");

    std::cout << "Graph before computing Eulerian Circuit:\n";
    graph.print();  // Optional: helpful for debugging

    if (!isEulerian(graph))
        throw std::logic_error("Graph is not Eulerian: no Eulerian circuit exists");

    std::stack<int> stack;
    std::vector<int> circuit;

    int curr = 0;

    // Find a vertex with non-zero degree to start
    for (int i = 0; i < graph.getSize(); ++i) {
        if (graph.degree(i) > 0) {
            curr = i;
            break;
        }
    }

    stack.push(curr);

    while (!stack.empty()) {
        int v = stack.top();

        // Look for a neighbor with an unused edge
        int u = -1;
        for (int i = 0; i < graph.getSize(); ++i) {
            if (graph.edgeExists(v, i)) {
                u = i;
                break;
            }
        }

        if (u == -1) {
            // No more edges from this vertex, add to circuit
            circuit.push_back(v);
            stack.pop();
        } else {
            // Follow the edge and remove it from the graph
            stack.push(u);
            graph.removeEdge(v, u);
        }
    }

    // Reverse to get the path from start to end
    std::reverse(circuit.begin(), circuit.end());

    return circuit;
}