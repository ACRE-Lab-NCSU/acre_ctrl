#pragma once
#include <cstdint>
#include <type_traits> 
#include <vector>
#include <string>

class Algorithm {
public:
    virtual ~Algorithm() = default;
    virtual void          init()    {}
    virtual AlgorithmOutput compute(const ComponentRegistry& world) = 0;
    virtual std::vector<std::string> required() const {
        return { "odom", "goal" };
    }
};                                                                             