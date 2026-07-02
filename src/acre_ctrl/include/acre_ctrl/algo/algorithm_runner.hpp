// algorithm_runner.hpp
#pragma once
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <nav_msgs/msg/odometry.hpp>
#include <nav_msgs/msg/path.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <cmath>

namespace py = pybind11;
namespace fs = std::filesystem;

// ControlOutput mirrors the Python dataclass
struct ControlOutput {
    double linear_vel  = 0.0;
    double angular_vel = 0.0;
};

class AlgorithmRunner {
public:
    explicit AlgorithmRunner(const std::string& path)
    {
        if (!path.ends_with(".py"))
            throw std::runtime_error("Only .py algorithms supported");
        load(path);
    }

    ~AlgorithmRunner() {
        py::gil_scoped_acquire gil;
        instance_.reset();
        registry_cls_.reset();
        odom_cls_.reset();
        goal_cls_.reset();
        path_cls_.reset();
        lidar_cls_.reset();
        waypoint_cls_.reset();
        output_cls_.reset();
    }

    void init() {
        py::gil_scoped_acquire gil;
        instance_->attr("init")();
    }

    std::vector<std::string> required() {
        py::gil_scoped_acquire gil;
        return instance_->attr("required")()
                   .cast<std::vector<std::string>>();
    }

    // Build Python objects and call compute — one GIL acquisition per tick
    ControlOutput compute(
        const nav_msgs::msg::Odometry&         odom_msg,
        const geometry_msgs::msg::PoseStamped& goal_msg,
        double dt,
        const nav_msgs::msg::Path*             path_msg  = nullptr,
        const sensor_msgs::msg::LaserScan*     lidar_msg = nullptr)
    {
        py::gil_scoped_acquire gil;

        // Build odom
        double heading = headingFromQuat(odom_msg.pose.pose.orientation);
        py::object odom = (*odom_cls_)(
            odom_msg.pose.pose.position.x,
            odom_msg.pose.pose.position.y,
            odom_msg.pose.pose.position.z,
            heading,
            odom_msg.twist.twist.linear.x,
            odom_msg.twist.twist.linear.y,
            odom_msg.twist.twist.angular.z,
            dt);

        // Build goal
        double gx = goal_msg.pose.position.x;
        double gy = goal_msg.pose.position.y;
        double x  = odom_msg.pose.pose.position.x;
        double y  = odom_msg.pose.pose.position.y;
        double gh = std::atan2(gy - y, gx - x);
        py::object goal = (*goal_cls_)(
            gx, gy, gh,
            gh - heading,
            std::hypot(gx - x, gy - y));

        // Build registry
        py::object registry = (*registry_cls_)();
        registry.attr("odom") = odom;
        registry.attr("goal") = goal;

        // Optionally add path
        if (path_msg) {
            py::list waypoints;
            for (auto& pose : path_msg->poses) {
                double wh = headingFromQuat(pose.pose.orientation);
                waypoints.append((*waypoint_cls_)(
                    pose.pose.position.x,
                    pose.pose.position.y,
                    wh));
            }
            registry.attr("path") = (*path_cls_)(waypoints, 0);
        }

        // Optionally add lidar
        if (lidar_msg) {
            py::list ranges;
            for (float r : lidar_msg->ranges)
                ranges.append(r);
            registry.attr("lidar") = (*lidar_cls_)(
                ranges,
                lidar_msg->angle_min,
                lidar_msg->angle_increment);
        }

        // Call compute — single Python call
        py::object result = instance_->attr("compute")(registry);

        return {
            result.attr("linear_vel").cast<double>(),
            result.attr("angular_vel").cast<double>()
        };
    }

private:
    inline static std::unique_ptr<py::scoped_interpreter> interp_;

    std::unique_ptr<py::object> instance_;
    std::unique_ptr<py::object> registry_cls_;
    std::unique_ptr<py::object> odom_cls_;
    std::unique_ptr<py::object> goal_cls_;
    std::unique_ptr<py::object> path_cls_;
    std::unique_ptr<py::object> lidar_cls_;
    std::unique_ptr<py::object> waypoint_cls_;
    std::unique_ptr<py::object> output_cls_;

    void load(const std::string& path)
    {
        if (!interp_)
            interp_ = std::make_unique<py::scoped_interpreter>();

        py::gil_scoped_acquire gil;

        py::module_::import("sys")
            .attr("path")
            .attr("insert")(0, fs::path(path).parent_path().string());

        // Import the acre_ctrl.algorithm package — gets all classes
        py::module_ pkg = py::module_::import("acre_ctrl.algorithm");

        // Cache class references — construct objects cheaply each tick
        registry_cls_ = std::make_unique<py::object>(
            pkg.attr("ComponentRegistry"));
        odom_cls_     = std::make_unique<py::object>(pkg.attr("OdomData"));
        goal_cls_     = std::make_unique<py::object>(pkg.attr("GoalData"));
        path_cls_     = std::make_unique<py::object>(pkg.attr("PathData"));
        lidar_cls_    = std::make_unique<py::object>(pkg.attr("LidarData"));
        waypoint_cls_ = std::make_unique<py::object>(pkg.attr("Waypoint"));
        output_cls_   = std::make_unique<py::object>(
            pkg.attr("ControlOutput"));

        // Import and find the algorithm class
        py::module_ mod = py::module_::import(
            fs::path(path).stem().string().c_str());

        py::object base     = pkg.attr("ControlSystem");
        py::object found;

        for (auto item : mod.attr("__dict__").cast<py::dict>()) {
            py::object val = py::reinterpret_borrow<py::object>(item.second);
            if (!py::isinstance<py::type>(val))             continue;
            if (val.is(base))                               continue;
            if (!PyObject_IsSubclass(val.ptr(), base.ptr())) continue;
            if (!found.is_none())
                throw std::runtime_error(
                    "Multiple ControlSystem subclasses in: " + path);
            found = val;
        }

        if (found.is_none())
            throw std::runtime_error(
                "No ControlSystem subclass found in: " + path);

        instance_ = std::make_unique<py::object>(found());
    }

    static double headingFromQuat(
        const geometry_msgs::msg::Quaternion& q)
    {
        return std::atan2(2.0*(q.w*q.z + q.x*q.y),
                          1.0 - 2.0*(q.y*q.y + q.z*q.z));
    }
};