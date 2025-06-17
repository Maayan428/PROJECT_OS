#ifndef ALGORITHM_FACTORY_HPP
#define ALGORITHM_FACTORY_HPP

#include "../algorithms/StrategyAlgorithm.hpp"
#include <memory>
#include <string>

class AlgorithmFactory {
public:
    static std::unique_ptr<StrategyAlgorithm> create(const std::string& name);
};

#endif