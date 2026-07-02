// src/bindings.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "acre_ctrl/algorithm/algorithm_base.hpp"
#include "acre_ctrl/algorithm/component_registry.hpp"

namespace py = pybind11;

// Trampoline — lets Python subclass ControlSystem
class PyControlSystem : public ControlSystem {
public:
    using ControlSystem::ControlSystem;

    void init() override {
        PYBIND11_OVERRIDE(void, ControlSystem, init);
    }

    ControlOutput compute(const ComponentRegistry& world) override {
        PYBIND11_OVERRIDE_PURE(ControlOutput, ControlSystem, compute, world);
    }

    std::vector<std::string> required() const override {
        PYBIND11_OVERRIDE(std::vector<std::string>, ControlSystem, required);
    }
};

PYBIND11_MODULE(acre_ctrl_algorithm, m) {

    // Building blocks
    py::class_<OdomData>(m, "OdomData")
        .def(py::init<>())
        .def_readwrite("x",             &OdomData::x)
        .def_readwrite("y",             &OdomData::y)
        .def_readwrite("z",             &OdomData::z)
        .def_readwrite("heading",       &OdomData::heading)
        .def_readwrite("linear_vel_x",  &OdomData::linear_vel_x)
        .def_readwrite("linear_vel_y",  &OdomData::linear_vel_y)
        .def_readwrite("angular_vel_z", &OdomData::angular_vel_z)
        .def_readwrite("dt",            &OdomData::dt);

    py::class_<GoalData>(m, "GoalData")
        .def(py::init<>())
        .def_readwrite("goal_x",        &GoalData::goal_x)
        .def_readwrite("goal_y",        &GoalData::goal_y)
        .def_readwrite("goal_heading",  &GoalData::goal_heading)
        .def_readwrite("heading_error", &GoalData::heading_error)
        .def_readwrite("distance",      &GoalData::distance);

    py::class_<PathData::Waypoint>(m, "Waypoint")
        .def(py::init<>())
        .def_readwrite("x",       &PathData::Waypoint::x)
        .def_readwrite("y",       &PathData::Waypoint::y)
        .def_readwrite("heading", &PathData::Waypoint::heading);

    py::class_<PathData>(m, "PathData")
        .def(py::init<>())
        .def_readwrite("current_idx", &PathData::current_idx)
        .def_property_readonly("waypoints", [](const PathData& pd) {
            return std::vector<PathData::Waypoint>(
                pd.waypoints, pd.waypoints + pd.len);
        });

    py::class_<ComponentRegistry>(m, "ComponentRegistry")
        .def(py::init<>())
        .def_readwrite("odom",  &ComponentRegistry::odom)
        .def_readwrite("goal",  &ComponentRegistry::goal)
        .def_readwrite("path",  &ComponentRegistry::path)
        .def_readwrite("lidar", &ComponentRegistry::lidar);

    // ControlOutput
    py::class_<ControlOutput>(m, "ControlOutput")
        .def(py::init<>())
        .def(py::init<double, double>())
        .def_readwrite("linear_vel",  &ControlOutput::linear_vel)
        .def_readwrite("angular_vel", &ControlOutput::angular_vel);

    py::class_<Algorithm, PyAlgorithm>(m, "Algorithm")
        .def(py::init<>())
        .def("init",     &AlgorithmSystem::init)
        .def("compute",  &AlgorithmSystem::compute)
        .def("required", &AlgorithmSystem::required);
}