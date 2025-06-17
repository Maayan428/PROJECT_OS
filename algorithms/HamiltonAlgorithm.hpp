#ifndef HAMILTON_ALGORITHM_HPP
#define HAMILTON_ALGORITHM_HPP

#include "StrategyAlgorithm.hpp"
#include <vector>
#include <string>

class HamiltonAlgorithm : public StrategyAlgorithm {
public:
    std::string run(const Graph& graph) override;

private:
    bool isHamiltonianUtil(const Graph& graph, std::vector<int>& path,
                           std::vector<bool>& visited, int pos);
};

#endif // HAMILTON_ALGORITHM_HPP