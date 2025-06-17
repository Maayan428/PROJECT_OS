#ifndef MAX_CLIQUE_ALGORITHM_HPP
#define MAX_CLIQUE_ALGORITHM_HPP

#include "StrategyAlgorithm.hpp"
#include <vector>
#include <string>

class MaxCliqueAlgorithm : public StrategyAlgorithm {
public:
    std::string run(const Graph& graph) override;

private:
    bool isClique(const Graph& graph, const std::vector<int>& nodes);
    void findMaxClique(const Graph& graph, std::vector<int>& current, int start);
    
    std::vector<int> maxClique;
};

#endif // MAX_CLIQUE_ALGORITHM_HPP