#include <chrono>
#include <memory>
#include <optional>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "nav_msgs/msg/path.hpp"
#include <grid_map_msgs/msg/grid_map.hpp>
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "trajectory_msgs/msg/joint_trajectory.hpp"
#include "acre_ctrl_msgs/srv/go2_start_sequence.hpp"
#include "algo/algorithm_runner.hpp"

#define START_SEQUENCE_SERVICE "go2_start_sequence"

class ControlNode : public rclcpp::Node {
public:
    ControlNode() : Node("point_control_node")
    {
        // Declare parameters
        this->declare_parameter("frequency",         10.0);
        this->declare_parameter("algorithm",         "");
        this->declare_parameter("pose_topic",        "/mocap_pose");
        this->declare_parameter("odom_topic",        "/sport_odom");
        this->declare_parameter("map_topic",         "/acre/sdf_cbf");
        this->declare_parameter("point_cloud_topic", "/cloud_registered");
        this->declare_parameter("imu_topic",         "/livox/imu");
        this->declare_parameter("path_topic",        "/plan");
        this->declare_parameter("traj_topic",        "/joint_trajectory");

        this->get_parameter("frequency",         frequency_);
        this->get_parameter("algorithm",         algo_path_);
        this->get_parameter("pose_topic",        pose_topic_);
        this->get_parameter("odom_topic",        odom_topic_);
        this->get_parameter("map_topic",         map_topic_);
        this->get_parameter("point_cloud_topic", point_cloud_topic_);
        this->get_parameter("imu_topic",         imu_topic_);
        this->get_parameter("path_topic",        path_topic_);
        this->get_parameter("traj_topic",        traj_topic_);

        // exit if no algorithm was provided
        if (algo_path_.empty()) {
            RCLCPP_ERROR(this->get_logger(), "Parameter 'algorithm' is empty. Exiting...");
            throw std::runtime_error("Exiting node due to empty algorithm parameter.");
        }

        // Load algorithm and read in the components it needs
        runner_ = std::make_unique<AlgorithmRunner>(algo_path_);

        // Only subscribe to what the loaded algorithm declared using @components(...)
        if (runner_->components().pose) {
            pose_sub_ = this->create_subscription<geometry_msgs::msg::Pose>(
                pose_topic_, 10,
                [this](geometry_msgs::msg::Pose::SharedPtr msg) { pose_ = *msg; });
        }
        if (runner_->components().odom) {
            odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
                odom_topic_, 10,
                [this](nav_msgs::msg::Odometry::SharedPtr msg) { odom_ = *msg; });
        }
        if (runner_->components().goal) {
            goal_sub_ = this->create_subscription<geometry_msgs::msg::Pose>(
                "goal_pose", 10,
                [this](geometry_msgs::msg::Pose::SharedPtr msg) { goal_ = *msg; });
        }
        if (runner_->components().path) {
            path_sub_ = this->create_subscription<nav_msgs::msg::Path>(
                path_topic_, 10,
                [this](nav_msgs::msg::Path::SharedPtr msg) { path_ = *msg; });
        }
        if (runner_->components().traj) {
            traj_sub_ = this->create_subscription<trajectory_msgs::msg::JointTrajectory>(
                traj_topic_, 10,
                [this](trajectory_msgs::msg::JointTrajectory::SharedPtr msg) { traj_ = *msg; });
        }
        if (runner_->components().point_cloud) {
            point_cloud_sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
                point_cloud_topic_, 10,
                [this](sensor_msgs::msg::PointCloud2::SharedPtr msg) { point_cloud_ = *msg; });
        }
        if (runner_->components().map) {
            map_sub_ = this->create_subscription<grid_map_msgs::msg::GridMap>(
                map_topic_, 10,
                [this](grid_map_msgs::msg::GridMap::SharedPtr msg) { map_ = *msg; });
        }
        if (runner_->components().imu) {
            imu_sub_ = this->create_subscription<sensor_msgs::msg::Imu>(
                imu_topic_, 10,
                [this](sensor_msgs::msg::Imu::SharedPtr msg) { imu_ = *msg; });
        }

        // Algorithm output publisher
        twist_pub_ = this->create_publisher<geometry_msgs::msg::TwistStamped>(
            "cmd_vel", 10);

        // Start sequence client
        start_client_ = this->create_client<acre_ctrl_msgs::srv::Go2StartSequence>(
            START_SEQUENCE_SERVICE);

        last_tick_ = this->now();
    }

    // Blocks until the driver replies that the startup sequence is complete
    bool wait_for_start_sequence()
    {
        while (!start_client_->wait_for_service(std::chrono::seconds(1))) {
            RCLCPP_INFO(this->get_logger(), "Waiting for %s service...",
                        START_SEQUENCE_SERVICE);
        }

        auto request = std::make_shared<acre_ctrl_msgs::srv::Go2StartSequence::Request>();
        auto future  = start_client_->async_send_request(request);

        if (rclcpp::spin_until_future_complete(this->get_node_base_interface(), future) !=
                rclcpp::FutureReturnCode::SUCCESS) {
            RCLCPP_ERROR(this->get_logger(), "Failed to call %s", START_SEQUENCE_SERVICE);
            return false;
        }

        auto response = future.get();
        RCLCPP_INFO(this->get_logger(), "Startup sequence response: success=%d message=%s",
                    response->success, response->message.c_str());
        return response->success;
    }

    // Only call once wait_for_start_sequence() returns true
    void start_control_loop()
    {
        auto period_ms = static_cast<int>(1000.0 / frequency_);
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(period_ms),
            std::bind(&ControlNode::ctrl_callback, this));

        RCLCPP_INFO(this->get_logger(), "Control loop started at %.1f Hz", frequency_);
    }

private:
    void ctrl_callback()
    {
        // Push whatever's been received into the runner's registry.
        // TODO: We might want a gaurd that stops the loop if these values are not published yet
        if (pose_)        runner_->set_pose(*pose_);
        if (odom_)        runner_->set_odom(*odom_);
        if (goal_)        runner_->set_goal(*goal_);
        if (path_)        runner_->set_path(*path_);
        if (traj_)        runner_->set_traj(*traj_);
        if (point_cloud_) runner_->set_point_cloud(*point_cloud_);
        if (map_)         runner_->set_map(*map_);
        if (imu_)         runner_->set_imu(*imu_);

        // Track dt
        auto now = this->now();
        double dt = (now - last_tick_).seconds();
        if (runner_->components().dt) {
            runner_->set_dt(dt);
        }
        last_tick_ = now;

        // Run algorithm
        geometry_msgs::msg::Twist out = runner_->compute();

        // Repackage the Twist message to be stamped and publish
        geometry_msgs::msg::TwistStamped msg;
        msg.header.stamp    = now;
        msg.header.frame_id = "base_link";
        msg.twist           = out;
        twist_pub_->publish(msg);
    }

    // Topics
    std::string pose_topic_;
    std::string odom_topic_;
    std::string map_topic_;
    std::string point_cloud_topic_;
    std::string imu_topic_;
    std::string path_topic_;
    std::string traj_topic_;

    // Parameters
    double      frequency_;
    std::string algo_path_;

    // State
    std::optional<geometry_msgs::msg::Pose>                 pose_           = std::nullopt;
    std::optional<nav_msgs::msg::Odometry>                  odom_           = std::nullopt;
    std::optional<geometry_msgs::msg::Pose>                 goal_           = std::nullopt;
    std::optional<nav_msgs::msg::Path>                      path_           = std::nullopt;
    std::optional<trajectory_msgs::msg::JointTrajectory>    traj_           = std::nullopt;
    std::optional<sensor_msgs::msg::PointCloud2>            point_cloud_    = std::nullopt;
    std::optional<grid_map_msgs::msg::GridMap>              map_            = std::nullopt;
    std::optional<sensor_msgs::msg::Imu>                    imu_            = std::nullopt;
    rclcpp::Time last_tick_;

    // Algorithm
    std::unique_ptr<AlgorithmRunner> runner_;

    // ROS pub/subs
    rclcpp::TimerBase::SharedPtr timer_;

    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr               pose_sub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr                odom_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr               goal_sub_;
    rclcpp::Subscription<nav_msgs::msg::Path>::SharedPtr                    path_sub_;
    rclcpp::Subscription<trajectory_msgs::msg::JointTrajectory>::SharedPtr  traj_sub_;
    rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr          point_cloud_sub_;
    rclcpp::Subscription<grid_map_msgs::msg::GridMap>::SharedPtr            map_sub_;
    rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr                  imu_sub_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr          twist_pub_;
    rclcpp::Client<acre_ctrl_msgs::srv::Go2StartSequence>::SharedPtr             start_client_;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<ControlNode>();

    if (!node->wait_for_start_sequence()) {
        RCLCPP_ERROR(node->get_logger(), "Startup sequence failed, shutting down");
        rclcpp::shutdown();
        return EXIT_FAILURE;
    }

    node->start_control_loop();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return EXIT_SUCCESS;
}