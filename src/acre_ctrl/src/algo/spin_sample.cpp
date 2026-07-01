#include "algo/algorithm_base.hpp"
#include <cmath>

class SpinSample : public ControlAlgorithm {
public:
    void init() override {}

    ControlOutput compute(const void* raw) override {
        return { .linear_vel_x = 0.0, .linear_vel_y = 0.0, .angular_vel_z = 0.05 };
    }
};

REGISTER_ALGORITHM(SpinSample, PointInput)