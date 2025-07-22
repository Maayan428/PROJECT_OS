#include "Graph.hpp"
#include <stdexcept>
#include <random>
#include <iostream>
#include <sstream>
#include <set>


/**
 * @brief Constructs a graph with the given number of vertices.
 * Initializes an adjacency matrix with all values set to 0.
 *
 * @param size Number of vertices in the graph.
 * @throws std::invalid_argument if size is non-positive.
 */
Graph::Graph(int size) : size(size), adjMatrix(size, std::vector<int>(size, 0)) {
    if (size <= 0)
        throw std::invalid_argument("Graph size must be positive");
}

/**
 * @brief Adds an undirected edge between vertices u and v.
 *
 * @param u First vertex.
 * @param v Second vertex.
 * @throws std::out_of_range if u or v are out of range.
 * @throws std::invalid_argument if u == v (self-loop).
 * @throws std::logic_error if the edge already exists.
 */
void Graph::addEdge(int u, int v) {
    if (u < 0 || v < 0 || u >= size || v >= size)
        throw std::out_of_range("Vertex index out of range");

    if (u == v)
        throw std::invalid_argument("Self-loops are not allowed");

    if (edgeExists(u, v))
        throw std::logic_error("Edge already exists");

    // Set both directions since this is an undirected graph
    adjMatrix[u][v] = 1;
    adjMatrix[v][u] = 1;
}

/**
 * @brief Removes the undirected edge between vertices u and v.
 *
 * @param u First vertex.
 * @param v Second vertex.
 * @throws std::out_of_range if u or v are out of range.
 * @throws std::logic_error if the edge does not exist.
 */
void Graph::removeEdge(int u, int v) {
    if (u < 0 || v < 0 || u >= size || v >= size)
        throw std::out_of_range("Vertex index out of range");

    if (!edgeExists(u, v))
        throw std::logic_error("Edge does not exist");

    // Remove the edge in both directions
    adjMatrix[u][v] = 0;
    adjMatrix[v][u] = 0;
}

/**
 * @brief Checks whether an edge exists between vertices u and v.
 *
 * @param u First vertex.
 * @param v Second vertex.
 * @return true if edge exists, false otherwise.
 */
bool Graph::edgeExists(int u, int v) const {
    if (u < 0 || v < 0 || u >= size || v >= size)
        return false;

    return adjMatrix[u][v] == 1;
}

/**
 * @brief Returns the number of vertices in the graph.
 *
 * @return Number of vertices.
 */
int Graph::getSize() const {
    return size;
}

/**
 * @brief Calculates the degree of a vertex (number of edges connected to it).
 *
 * @param v The vertex.
 * @return Degree of vertex v.
 * @throws std::out_of_range if vertex index is invalid.
 */
int Graph::degree(int v) const {
    if (v < 0 || v >= size)
        throw std::out_of_range("Vertex index out of range");

    int deg = 0;
    for (int i = 0; i < size; ++i)
        deg += adjMatrix[v][i];  // Count the number of connections (1s)
    return deg;
}

/**
 * @brief Returns a const reference to the adjacency matrix.
 *
 * @return Const reference to the 2D vector representing the adjacency matrix.
 */
const std::vector<std::vector<int>>& Graph::getAdjMatrix() const {
    return adjMatrix;
}


/**
 * @brief Randomly generates an undirected graph with a given number of edges and a seed.
 *
 * Ensures no duplicate edges or self-loops are created.
 *
 * @param numEdges The number of edges to generate.
 * @param seed Seed for the random number generator (to make results reproducible).
 * @throws std::logic_error if numEdges is too large for the number of vertices.
 */
void Graph::generateRandomUndirected(int numEdges, int seed) {
    int maxEdges = (size * (size - 1)) / 2;
    if (numEdges > maxEdges)
        throw std::logic_error("Too many edges requested for the graph size.");

    std::mt19937 rng(seed);  // Mersenne Twister random number generator
    std::uniform_int_distribution<int> dist(0, size - 1);

    std::set<std::pair<int, int>> usedEdges;

    int edgesAdded = 0;
    while (edgesAdded < numEdges) {
        int u = dist(rng);
        int v = dist(rng);
        if (u == v) continue;

        // Order the pair so (2,3) and (3,2) are treated the same
        std::pair<int, int> edge = (u < v) ? std::make_pair(u, v) : std::make_pair(v, u);

        if (usedEdges.find(edge) == usedEdges.end()) {
            addEdge(u, v);                // Use existing addEdge() with validations
            usedEdges.insert(edge);
            edgesAdded++;
        }
    }
}

/**
 * @brief Prints the adjacency matrix to the given output stream.
 *
 * Used by server to send graph structure to clients.
 *
 * @param out Output stream to write to (e.g., std::stringstream).
 */
void Graph::printAdjacencyMatrix(std::ostream& out) const {
    out << "Adjacency Matrix (" << size << " vertices):\n   ";
    for (int i = 0; i < size; ++i) {
        out << i << " ";
    }
    out << "\n";
    for (int i = 0; i < size; ++i) {
        out << i << ": ";
        for (int j = 0; j < size; ++j) {
            out << adjMatrix[i][j] << " ";
        }
        out << "\n";
    }
}

void Graph::print() const {
    std::cout << "Adjacency Matrix:\n";
    for (const auto& row : adjMatrix) {
        for (int val : row) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }
}