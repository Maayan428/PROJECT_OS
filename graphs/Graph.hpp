#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <ostream>

class Graph {
public:
    Graph(int size);
    void addEdge(int u, int v);
    void removeEdge(int u, int v);
    bool edgeExists(int u, int v) const;
    int getSize() const;
    int degree(int v) const;
    const std::vector<std::vector<int>>& getAdjMatrix() const;
    void print() const;
    void generateRandomUndirected(int numEdges, int seed);
    void printAdjacencyMatrix(std::ostream& out) const;

private:
    std::vector<std::vector<int>> adjMatrix;
    int size;
};

#endif