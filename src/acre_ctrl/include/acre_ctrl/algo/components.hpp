#pragma once
#include <cstdint>
#include <optional> 
#include <type_traits> 

struct AlgorithmOutput {
    double linear_vel_x         = 0.0;   // m/s
    double linear_vel_y         = 0.0;   // m/s
    double angular_vel_z        = 0.0;   // rad/s
};

struct OdomData {
    double x, y, z;
    double heading;
    double linear_vel_x, linear_vel_y;
    double angular_vel_z;
    double dt;
};

struct GoalData {
    double goal_x, goal_y;
    double goal_heading;
    double heading_error;
    double distance;
};

struct PathData {
    struct Waypoint { double x, y, heading; };
    const Waypoint* waypoints;
    int             len;
    int             current_idx;
};

struct ComponentRegistry {
    std::optional<OdomData>  odom;
    std::optional<GoalData>  goal;
    std::optional<PathData>  path;
};