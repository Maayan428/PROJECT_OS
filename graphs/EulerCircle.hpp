#ifndef EULER_CIRCLE_H
#define EULER_CIRCLE_H

#include "Graph.hpp"
#include <vector>

class EulerCircle {
public:
    static void dfs(const Graph& graph, int v, std::vector<bool>& visited);
    static bool isEulerian(const Graph& graph);  
    static std::vector<int> getEulerianCircuit(Graph graph); 

private:
    static bool isConnected(const Graph& graph);
};

#endif