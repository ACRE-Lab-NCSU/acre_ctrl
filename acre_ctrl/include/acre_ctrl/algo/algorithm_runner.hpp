#pragma once
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <rclcpp/serialization.hpp>
#include <rclcpp/serialized_message.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <nav_msgs/msg/path.hpp>
#include <grid_map_msgs/msg/grid_map.hpp>
#include <grid_map_ros/GridMapRosConverter.hpp>
#include <grid_map_core/GridMap.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <trajectory_msgs/msg/joint_trajectory.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <cstring>

namespace py = pybind11;
namespace fs = std::filesystem;

struct RequiredComponents {
    bool dt          = false;
    bool pose        = false;
    bool odom        = false;
    bool goal        = false;
    bool path        = false;
    bool traj        = false;
    bool point_cloud = false;
    bool map         = false;
    bool imu         = false;
};

// This class is for converting the pointcloud message only
template <typename MsgT>
py::bytes serialize_msg(const MsgT& msg) {
    rclcpp::Serialization<MsgT> serializer;
    rclcpp::SerializedMessage serialized;
    serializer.serialize_message(&msg, &serialized);
    const auto& rcl_msg = serialized.get_rcl_serialized_message();
    return py::bytes(reinterpret_cast<const char*>(rcl_msg.buffer),
                      rcl_msg.buffer_length);
}

class AlgorithmRunner {
public:
    explicit AlgorithmRunner(const std::string& path) {
        if (!path.ends_with(".py"))
            throw std::runtime_error("Only .py algorithms supported");
        load(path);
    }

    ~AlgorithmRunner() {
        py::gil_scoped_acquire gil;
        // Clear every held python reference while the GIL is held
        instance_ = py::object();
        input_    = py::object();
        cdr_      = py::object();
        pose_cls_ = quaternion_cls_ = vector3_cls_ = point_cls_ = py::object();
        odom_cls_ = path_cls_ = pose_stamped_cls_ = py::object();
        traj_cls_ = traj_point_cls_ = occ_cls_ = imu_cls_ = py::object();
    }

    void init() {
        py::gil_scoped_acquire gil;
        instance_.attr("init")();
    }

    const RequiredComponents& components() const { return required_; }

    void set_dt(const double dt) {
        if (!required_.dt) return;
        py::gil_scoped_acquire gil;
        input_.attr("dt") = dt;
    }

    void set_pose(const geometry_msgs::msg::Pose& pose) {
        if (!required_.pose) return;
        py::gil_scoped_acquire gil;
        input_.attr("pose") = to_py(pose);
    }

    void set_odom(const nav_msgs::msg::Odometry& odom) {
        if (!required_.odom) return;
        py::gil_scoped_acquire gil;
        input_.attr("odom") = to_py(odom);
    }

    void set_goal(const geometry_msgs::msg::Pose& goal) {
        if (!required_.goal) return;
        py::gil_scoped_acquire gil;
        input_.attr("goal") = to_py(goal);
    }

    void set_path(const nav_msgs::msg::Path& path) {
        if (!required_.path) return;
        py::gil_scoped_acquire gil;
        input_.attr("path") = to_py(path);
    }

    void set_traj(const trajectory_msgs::msg::JointTrajectory& traj) {
        if (!required_.traj) return;
        py::gil_scoped_acquire gil;
        input_.attr("traj") = to_py(traj);
    }

    void set_point_cloud(const sensor_msgs::msg::PointCloud2& pc) {
        if (!required_.point_cloud) return;
        py::gil_scoped_acquire gil;
        input_.attr("point_cloud") = cdr_.attr("from_cdr")(
            serialize_msg(pc), "sensor_msgs/msg/PointCloud2");
    }

    void set_map(const grid_map_msgs::msg::GridMap& msg) {
        if (!required_.map) return;
        py::gil_scoped_acquire gil;

        grid_map::GridMap gm;
        grid_map::GridMapRosConverter::fromMessage(msg, gm);

        input_.attr("map") = py::cast(gm);   // relies on grid_map_python's type caster being registered
    }

    void set_imu(const sensor_msgs::msg::Imu& imu) {
        if (!required_.imu) return;
        py::gil_scoped_acquire gil;
        input_.attr("imu") = to_py(imu);
    }

    geometry_msgs::msg::Twist compute() {
        py::gil_scoped_acquire gil;
        py::object result = instance_.attr("compute")(input_);

        geometry_msgs::msg::Twist out;
        out.linear.x  = result.attr("linear").attr("x").cast<double>();
        out.linear.y  = result.attr("linear").attr("y").cast<double>();
        out.linear.z  = result.attr("linear").attr("z").cast<double>();
        out.angular.x = result.attr("angular").attr("x").cast<double>();
        out.angular.y = result.attr("angular").attr("y").cast<double>();
        out.angular.z = result.attr("angular").attr("z").cast<double>();
        return out;
    }

private:
    inline static std::unique_ptr<py::scoped_interpreter> interp_;

    py::object instance_;
    py::object input_;     // persistent ComponentRegistry instance, mutated in place
    py::object cdr_;       // acre_ctrl.algorithm.cdr module, PointCloud2 only

    // Cached python message classes, resolved once at load time.
    py::object pose_cls_, quaternion_cls_, vector3_cls_, point_cls_;
    py::object odom_cls_, path_cls_, pose_stamped_cls_;
    py::object traj_cls_, traj_point_cls_;
    py::object occ_cls_, imu_cls_;

    RequiredComponents required_;

    void load(const std::string& path) {
        if (!interp_)
            interp_ = std::make_unique<py::scoped_interpreter>();

        py::gil_scoped_acquire gil;

        py::module_::import("sys")
            .attr("path")
            .attr("insert")(0, fs::path(path).parent_path().string());

        py::module_ pkg = py::module_::import("acre_ctrl.algorithm");
        cdr_ = py::module_::import("acre_ctrl.common.cdr");

        py::module_ geom = py::module_::import("geometry_msgs.msg");
        py::module_ navm = py::module_::import("nav_msgs.msg");
        py::module_ traj = py::module_::import("trajectory_msgs.msg");
        py::module_ sens = py::module_::import("sensor_msgs.msg");
        py::module_::import("grid_map_python");

        pose_cls_        = geom.attr("Pose");
        quaternion_cls_  = geom.attr("Quaternion");
        vector3_cls_     = geom.attr("Vector3");
        point_cls_       = geom.attr("Point");
        pose_stamped_cls_= geom.attr("PoseStamped");
        odom_cls_        = navm.attr("Odometry");
        path_cls_        = navm.attr("Path");
        occ_cls_         = navm.attr("OccupancyGrid");
        traj_cls_        = traj.attr("JointTrajectory");
        traj_point_cls_  = traj.attr("JointTrajectoryPoint");
        imu_cls_         = sens.attr("Imu");

        py::object registry_cls = pkg.attr("ComponentRegistry");
        input_ = registry_cls();

        py::module_ mod = py::module_::import(
            fs::path(path).stem().string().c_str());

        py::object base = pkg.attr("ControlAlgorithm");
        py::object found = py::none();

        for (auto item : mod.attr("__dict__").cast<py::dict>()) {
            py::object val = py::reinterpret_borrow<py::object>(item.second);
            if (!py::isinstance<py::type>(val))              continue;
            if (val.is(base))                                continue;
            if (!PyObject_IsSubclass(val.ptr(), base.ptr()))  continue;
            if (!found.is_none())
                throw std::runtime_error(
                    "Multiple ControlAlgorithm subclasses in: " + path);
            found = val;
        }

        if (found.is_none())
            throw std::runtime_error(
                "No ControlAlgorithm subclass found in: " + path);

                required_ = read_required_components(found);
                instance_ = found();
            }

    static RequiredComponents read_required_components(py::object cls) {
        RequiredComponents rc;
        std::vector<std::string> names;

        if (py::hasattr(cls, "_components")) {
            names = cls.attr("_components").cast<std::vector<std::string>>();
        } else {
            py::object tmp = cls();
            names = tmp.attr("components")().cast<std::vector<std::string>>();
        }

        for (const auto& n : names) {
            if      (n == "pose")        rc.pose = true;
            else if (n == "dt")          rc.dt = true;
            else if (n == "odom")        rc.odom = true;
            else if (n == "goal")        rc.goal = true;
            else if (n == "path")        rc.path = true;
            else if (n == "traj")        rc.traj = true;
            else if (n == "point_cloud") rc.point_cloud = true;
            else if (n == "map")         rc.map = true;
            else if (n == "imu")         rc.imu = true;
            else throw std::runtime_error(
                "Algorithm declared unknown required component: " + n);
        }
        return rc;

    }

    py::object to_py(const geometry_msgs::msg::Quaternion& q) {
        py::object obj = quaternion_cls_();
        obj.attr("x") = q.x; obj.attr("y") = q.y;
        obj.attr("z") = q.z; obj.attr("w") = q.w;
        return obj;
    }

    py::object to_py(const geometry_msgs::msg::Vector3& v) {
        py::object obj = vector3_cls_();
        obj.attr("x") = v.x; obj.attr("y") = v.y; obj.attr("z") = v.z;
        return obj;
    }

    py::object to_py(const geometry_msgs::msg::Point& p) {
        py::object obj = point_cls_();
        obj.attr("x") = p.x; obj.attr("y") = p.y; obj.attr("z") = p.z;
        return obj;
    }

    py::object to_py(const geometry_msgs::msg::Pose& p) {
        py::object obj = pose_cls_();
        obj.attr("position")    = to_py(p.position);
        obj.attr("orientation") = to_py(p.orientation);
        return obj;
    }

    py::object to_py(const geometry_msgs::msg::PoseStamped& ps) {
        py::object obj = pose_stamped_cls_();
        obj.attr("header").attr("frame_id") = ps.header.frame_id;
        obj.attr("pose") = to_py(ps.pose);
        return obj;
    }

    py::object to_py(const nav_msgs::msg::Odometry& msg) {
        py::object obj = odom_cls_();
        obj.attr("header").attr("frame_id") = msg.header.frame_id;
        obj.attr("child_frame_id") = msg.child_frame_id;

        obj.attr("pose").attr("pose") = to_py(msg.pose.pose);
        obj.attr("pose").attr("covariance") =
            py::cast(std::vector<double>(msg.pose.covariance.begin(),
                                          msg.pose.covariance.end()));

        obj.attr("twist").attr("twist").attr("linear")  = to_py(msg.twist.twist.linear);
        obj.attr("twist").attr("twist").attr("angular") = to_py(msg.twist.twist.angular);
        obj.attr("twist").attr("covariance") =
            py::cast(std::vector<double>(msg.twist.covariance.begin(),
                                          msg.twist.covariance.end()));
        return obj;
    }

    py::object to_py(const nav_msgs::msg::Path& msg) {
        py::object obj = path_cls_();
        obj.attr("header").attr("frame_id") = msg.header.frame_id;

        py::list poses;
        for (const auto& p : msg.poses) poses.append(to_py(p));
        obj.attr("poses") = poses;
        return obj;
    }

    py::object to_py(const trajectory_msgs::msg::JointTrajectory& msg) {
        py::object obj = traj_cls_();
        obj.attr("header").attr("frame_id") = msg.header.frame_id;
        obj.attr("joint_names") = py::cast(msg.joint_names);

        py::list points;
        for (const auto& pt : msg.points) {
            py::object p = traj_point_cls_();
            p.attr("positions")     = py::cast(pt.positions);
            p.attr("velocities")    = py::cast(pt.velocities);
            p.attr("accelerations") = py::cast(pt.accelerations);
            p.attr("effort")        = py::cast(pt.effort);
            p.attr("time_from_start").attr("sec")     = pt.time_from_start.sec;
            p.attr("time_from_start").attr("nanosec") = pt.time_from_start.nanosec;
            points.append(p);
        }
        obj.attr("points") = points;
        return obj;
    }

    py::object to_py(const nav_msgs::msg::OccupancyGrid& msg) {
        py::object obj = occ_cls_();
        obj.attr("header").attr("frame_id") = msg.header.frame_id;
        obj.attr("info").attr("resolution") = msg.info.resolution;
        obj.attr("info").attr("width")      = msg.info.width;
        obj.attr("info").attr("height")     = msg.info.height;
        obj.attr("info").attr("origin")     = to_py(msg.info.origin);
        obj.attr("data") = py::cast(msg.data);  // vector<int8_t> -> list
        return obj;
    }

    py::object to_py(const sensor_msgs::msg::Imu& msg) {
        py::object obj = imu_cls_();
        obj.attr("orientation")         = to_py(msg.orientation);
        obj.attr("angular_velocity")    = to_py(msg.angular_velocity);
        obj.attr("linear_acceleration") = to_py(msg.linear_acceleration);

        obj.attr("orientation_covariance") =
            py::cast(std::vector<double>(msg.orientation_covariance.begin(),
                                          msg.orientation_covariance.end()));
        obj.attr("angular_velocity_covariance") =
            py::cast(std::vector<double>(msg.angular_velocity_covariance.begin(),
                                          msg.angular_velocity_covariance.end()));
        obj.attr("linear_acceleration_covariance") =
            py::cast(std::vector<double>(msg.linear_acceleration_covariance.begin(),
                                          msg.linear_acceleration_covariance.end()));
        return obj;
    }
};