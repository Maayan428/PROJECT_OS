#include "MSTAlgorithm.hpp"
#include "../graphs/Graph.hpp"
#include <vector>
#include <limits>
#include <queue>
#include <sstream>

/**
 * @brief Executes Prim's algorithm to compute the Minimum Spanning Tree (MST) of the graph.
 *
 * The algorithm assumes that all edges have equal weight of 1, since the graph is unweighted.
 * It starts from vertex 0 and builds the MST greedily using a min-priority queue.
 *
 * @param graph The graph to compute the MST on.
 * @return A string describing the total weight of the MST.
 */
std::string MSTAlgorithm::run(const Graph& graph) {
    int n = graph.getSize();
    if (n == 0) return "Graph is empty.\n";

    std::vector<bool> inMST(n, false);  // Track which vertices are already included in MST
    std::vector<int> key(n, std::numeric_limits<int>::max()); // Store minimum edge weight to each vertex
    key[0] = 0; // Start from vertex 0

    using Pair = std::pair<int, int>; // (weight, vertex)
    std::priority_queue<Pair, std::vector<Pair>, std::greater<>> pq;
    pq.push({0, 0}); // Push starting vertex

    int totalWeight = 0;

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (inMST[u]) continue; // Skip if already processed
        inMST[u] = true;
        totalWeight += key[u]; // Accumulate MST weight

        // For each neighbor v of u
        for (int v = 0; v < n; ++v) {
            // If edge u-v exists, and v not in MST, and current weight to v is higher than 1
            if (graph.edgeExists(u, v) && !inMST[v] && key[v] > 1) {
                key[v] = 1; // Set weight to 1 (unweighted graph assumption)
                pq.push({key[v], v});
            }
        }
    }

    std::ostringstream out;
    out << "Total MST weight (Prim's Algorithm): " << totalWeight << "\n";
    return out.str();
}