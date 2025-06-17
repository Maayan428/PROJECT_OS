#ifndef SCC_ALGORITHM_HPP
#define SCC_ALGORITHM_HPP

#include "StrategyAlgorithm.hpp"
#include <vector>
#include <string>

class SCCAlgorithm : public StrategyAlgorithm {
public:
    std::string run(const Graph& graph) override;

private:
    void dfs(int v, const std::vector<std::vector<int>>& adj, std::vector<bool>& visited, std::vector<int>& component);
    std::vector<std::vector<int>> transpose(const std::vector<std::vector<int>>& adj);
};

#endif // SCC_ALGORITHM_HPP