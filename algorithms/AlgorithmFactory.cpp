#include "AlgorithmFactory.hpp"
#include "../algorithms/MSTAlgorithm.hpp"
#include "../algorithms/HamiltonAlgorithm.hpp"
#include "../algorithms/SCCAlgorithm.hpp"
#include "../algorithms/MaxCliqueAlgorithm.hpp"
#include <algorithm>

/**
 * @brief Creates an instance of a specific algorithm based on the given name.
 *
 * Supported algorithm names (case-insensitive):
 * - "mst"         : Minimum Spanning Tree algorithm
 * - "hamilton"    : Hamiltonian path/cycle algorithm
 * - "scc"         : Strongly Connected Components algorithm
 * - "maxclique" / "clique" : Maximum Clique algorithm
 *
 * @param name The name of the algorithm to create.
 * @return A unique_ptr to a StrategyAlgorithm implementation.
 * @throws std::invalid_argument if the algorithm name is not recognized.
 */
std::unique_ptr<StrategyAlgorithm> AlgorithmFactory::create(const std::string& name) {
    std::string lower_name = name;
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);

    if (lower_name == "mst") {
        return std::make_unique<MSTAlgorithm>();
    } else if (lower_name == "hamilton") {
        return std::make_unique<HamiltonAlgorithm>();
    } else if (lower_name == "scc") {
        return std::make_unique<SCCAlgorithm>();
    } else if (lower_name == "maxclique" || lower_name == "clique") {
        return std::make_unique<MaxCliqueAlgorithm>();
    }

    throw std::invalid_argument("Unknown algorithm: " + name);
}