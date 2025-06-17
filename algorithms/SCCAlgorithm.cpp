#include "SCCAlgorithm.hpp"
#include "../graphs/Graph.hpp"
#include <stack>
#include <sstream>
#include <functional>

/**
 * @brief Depth-First Search to collect all vertices reachable from a given vertex.
 *
 * Used in the second phase of Kosaraju’s algorithm to collect all nodes in a strongly connected component.
 *
 * @param v The current vertex.
 * @param adj The adjacency list of the graph.
 * @param visited A boolean vector indicating whether each vertex has been visited.
 * @param component The current strongly connected component being constructed.
 */
void SCCAlgorithm::dfs(int v, const std::vector<std::vector<int>>& adj, std::vector<bool>& visited, std::vector<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int u : adj[v]) {
        if (!visited[u]) {
            dfs(u, adj, visited, component);
        }
    }
}

/**
 * @brief Transposes the given adjacency list (i.e., reverses all edges).
 *
 * @param adj The original adjacency list.
 * @return A new adjacency list representing the transposed graph.
 */
std::vector<std::vector<int>> SCCAlgorithm::transpose(const std::vector<std::vector<int>>& adj) {
    int n = adj.size();
    std::vector<std::vector<int>> trans(n);
    for (int u = 0; u < n; ++u) {
        for (int v : adj[u]) {
            trans[v].push_back(u);  // Reverse the edge u -> v to v -> u
        }
    }
    return trans;
}

/**
 * @brief Finds all Strongly Connected Components (SCCs) in the input graph using Kosaraju's algorithm.
 *
 * Steps:
 * 1. Perform DFS to fill a stack based on finish times.
 * 2. Transpose the graph.
 * 3. Perform DFS on the transposed graph in the order of the stack to identify SCCs.
 *
 * @param graph The graph on which to perform the algorithm.
 * @return A string listing all SCCs found.
 */
std::string SCCAlgorithm::run(const Graph& graph) {
    int n = graph.getSize();
    if (n == 0)
        return "Graph is empty.\n";

    // Step 1: Build adjacency list from the graph's adjacency matrix
    std::vector<std::vector<int>> adj(n);
    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (graph.edgeExists(u, v)) {
                adj[u].push_back(v);
            }
        }
    }

    // Step 2: First DFS to populate the finish stack
    std::vector<bool> visited(n, false);
    std::stack<int> finishStack;
    std::function<void(int)> fillOrder = [&](int v) {
        visited[v] = true;
        for (int u : adj[v]) {
            if (!visited[u])
                fillOrder(u);
        }
        finishStack.push(v);  // Add vertex to stack after visiting all its neighbors
    };

    for (int i = 0; i < n; ++i) {
        if (!visited[i])
            fillOrder(i);
    }

    // Step 3: Transpose the graph
    std::vector<std::vector<int>> adjT = transpose(adj);

    // Step 4: DFS on the transposed graph using the stack order
    visited.assign(n, false);
    std::ostringstream out;
    int count = 0;

    while (!finishStack.empty()) {
        int v = finishStack.top();
        finishStack.pop();

        if (!visited[v]) {
            std::vector<int> component;
            dfs(v, adjT, visited, component);  // Collect all reachable vertices in this SCC

            out << "SCC #" << (++count) << ": ";
            for (int u : component) {
                out << u << " ";
            }
            out << "\n";
        }
    }

    return out.str();
}