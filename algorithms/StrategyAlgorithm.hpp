#ifndef STRATEGY_ALGORITHM_HPP
#define STRATEGY_ALGORITHM_HPP

#include "../graphs/Graph.hpp"
#include <string>

/**
 * @brief Abstract base class for graph algorithms using the Strategy pattern.
 *
 * This interface allows different algorithm implementations (e.g., MST, SCC, Hamiltonian)
 * to be interchangeable at runtime. Each subclass must implement the `run()` method.
 */
class StrategyAlgorithm {
public:
    /**
     * @brief Runs the algorithm on the provided graph and returns the result as a string.
     *
     * @param graph The graph on which the algorithm will be executed.
     * @return A string containing the result of the algorithm.
     */
    virtual std::string run(const Graph& graph) = 0;

    /**
     * @brief Virtual destructor to allow proper cleanup of derived classes.
     */
    virtual ~StrategyAlgorithm() = default;
};

#endif // STRATEGY_ALGORITHM_HPP