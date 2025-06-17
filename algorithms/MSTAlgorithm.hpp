#ifndef MST_ALGORITHM_HPP
#define MST_ALGORITHM_HPP

#include "StrategyAlgorithm.hpp"

class MSTAlgorithm : public StrategyAlgorithm {
public:
    std::string run(const Graph& graph) override;
};

#endif