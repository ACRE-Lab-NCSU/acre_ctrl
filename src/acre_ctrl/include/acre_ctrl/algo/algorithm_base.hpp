#pragma once
#include <cstdint>
#include <type_traits> 

struct ControlOutput {
    double linear_vel_x         = 0.0;   // m/s
    double linear_vel_y         = 0.0;   // m/s
    double angular_vel_z        = 0.0;   // rad/s
};


enum class AlgorithmInputType : uint32_t {
    PointInput      = 1,
    PathInput       = 2,
    SensorInput     = 3,
    PathSensorInput = 4,
};

struct OdomData {
    double x, y, z;
    double heading;
    double linear_vel_x, linear_vel_y, linear_vel_z;
    double angular_vel_x, angular_vel_y, angular_vel_z;
    double dt;
};

struct GoalData {
    double goal_x, goal_y, goal_z;
    double goal_heading;
    double heading_error;
    double distance;
};

struct PathData {
    struct Waypoint { double x, y, z, heading; };
    const Waypoint* waypoints;
    int             len;
    int             current_idx;
};

struct LidarData {
    const float* ranges;
    int          len;
    float        angle_min;
    float        angle_increment;
};

struct PointInput {
    OdomData odom;
    GoalData goal;
};

struct PathInput {
    OdomData odom;
    GoalData goal;
    PathData path;
};

struct SensorInput {
    OdomData  odom;
    GoalData  goal;
    LidarData lidar;
};

struct PathSensorInput {
    OdomData  odom;
    GoalData  goal;
    PathData  path;
    LidarData lidar;
};

class ControlAlgorithm {
public:
    virtual ~ControlAlgorithm() = default;
    virtual void init() {}
    virtual ControlOutput compute(const void* input) = 0;
};

#define REGISTER_ALGORITHM(ClassName, InputType)                                  \
static_assert(std::is_base_of<ControlAlgorithm, ClassName>::value,                \
              #ClassName " must inherit from ControlAlgorithm");                  \
extern "C" {                                                                      \
    void* algo_create()  { return new ClassName(); }                              \
    void  algo_destroy(void* p) {                                                 \
        delete static_cast<ClassName*>(p); }                                      \
    void  algo_init(void* p) {                                                    \
        static_cast<ClassName*>(p)->init(); }                                     \
    ControlOutput algo_compute(void* p, const void* input) {                      \
        return static_cast<ClassName*>(p)->compute(input); }                      \
    uint32_t algo_input_type() {                                                  \
        return static_cast<uint32_t>(AlgorithmInputType::InputType); }            \
}