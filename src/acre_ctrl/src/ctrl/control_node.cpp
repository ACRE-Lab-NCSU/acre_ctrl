#include <chrono>
#include <memory>
#include <optional>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "acre_ctrl/srv/go2_start_sequence.hpp"
#include "algo/algorithm_runner.hpp"
//#include "input_builders.hpp"

#define START_SEQUENCE_SERVICE "go2_start_sequence"

class ControlNode : public rclcpp::Node {
public:
    PointControlNode() : Node("point_control_node")
    {
        // Declare parameters
        this->declare_parameter("frequency",    10.0);
        this->declare_parameter("algorithm",    "");
        this->declare_parameter("odom_topic",   "acre_odom");
        this->declare_parameter("map_topic",    "/map");
        this->declare_parameter("lidar_topic",  "/unitree/slam_lidar/points");
        this->declare_parameter("imu_topic",    "/unitree/slam_lidar/imu");

        this->get_parameter("frequency",    frequency_);
        this->get_parameter("algorithm",    algo_path_);
        this->get_parameter("odom_topic",   odom_topic_);
        this->get_parameter("map_topic",    map_topic_);
        this->get_parameter("lidar_topic",  lidar_topic_);
        this->get_parameter("imu_topic",    imu_topic_);

        // Load and validate algorithm
        runner_ = std::make_unique<AlgorithmRunner>(algo_path_);
        if (runner_->input_type() != AlgorithmInputType::PointInput)
            throw std::runtime_error("This node requires a PointInput algorithm");\

        // Get all the inputs of the algorithm

        // Load the needed Subscribers based on algorithm input
        if (runner_.components.use_odom) {
            odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
                odom_topic_, 10,
                [this](nav_msgs::msg::Odometry::SharedPtr msg) { odom_ = *msg; });
        }

        goal_sub_ = this->create_subscription<geometry_msgs::msg::PoseStamped>(
            "goal_pose", 10,
            [this](geometry_msgs::msg::PoseStamped::SharedPtr msg) { goal_ = *msg; });

        // Algorithm output publisher
        twist_pub_ = this->create_publisher<geometry_msgs::msg::TwistStamped>(
            "cmd_vel", 10);

        // Start sequence client
        start_client_ = this->create_client<acre_ctrl::srv::Go2StartSequence>(
            START_SEQUENCE_SERVICE);
    }

    // Blocks until the driver replies that the startup sequence is complete
    bool wait_for_start_sequence()
    {
        while (!start_client_->wait_for_service(std::chrono::seconds(1))) {
            RCLCPP_INFO(this->get_logger(), "Waiting for %s service...",
                        START_SEQUENCE_SERVICE);
        }

        auto request = std::make_shared<acre_ctrl::srv::Go2StartSequence::Request>();
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
            std::bind(&PointControlNode::ctrl_callback, this));

        RCLCPP_INFO(this->get_logger(), "Control loop started at %.1f Hz", frequency_);
    }

private:
    void ctrl_callback()
    {
        // Add all needed inputs to the runners input registry
        if (odom_) { /** runner_.input.odom = build_odom_input(odom_) */ }
        if (goal_) { /** runner_.input.goal = build_goal_input(goal_) */ }
        if (path_) { /** runner_.input.path = build_path_input(path_) */ }
        if (traj_) { /** runner_.input.traj = build_traj_input(traj_) */ }
        if (lidar_) { /** runner_.input.lidar = build_lidar_input(lidar_) */ }
        if (costmap_) { /** runner_.input.map = build_map_input(costmap_) */ }
        if (imu_) { /** runner_.input.imu = build_imu_input(imu_) */ }

        // Track dt
        auto now = this->now();
        /**
        double dt = (now - last_tick_).seconds();
        last_tick_ = now;

        // Build input — no ROS types past this line
        PointInput input;
        input.odom = buildOdom(odom_, dt);
        input.goal = buildGoal(input.odom, goal_);

        // Run algorithm
        auto out = runner_->compute(runner_.input);

        // Publish
        geometry_msgs::msg::TwistStamped msg;
        msg.header.stamp    = now;
        msg.header.frame_id = "base_link";
        msg.twist.linear.x  = out.linear_vel;
        msg.twist.angular.z = out.angular_vel;
        */
        geometry_msgs::msg::TwistStamped msg;
        msg.header.stamp    = now;
        msg.header.frame_id = "base_link";
        msg.twist.linear.x  = 0.0;
        msg.twist.linear.y = 0.0;
        msg.twist.angular.z = 0.5;
        twist_pub_->publish(msg);
    }

    // Topics 
    std::string odom_topic_;
    std::string map_topic_;
    std::string lidar_topic_;
    std::string imu_topic_;

    // Parameters
    double      frequency_;
    std::string algo_path_;

    // State
    std::optional<std::nav_msgs::msg::Odometry>         odom_       = std::nullopt;
    std::optional<geometry_msgs::msg::PoseStamped>      goal_       = std::nullopt;
    std::optional<nav_msgs::msg::Path>                  path_       = std::nullopt;
    std::optional<nav_msgs::msg::Path>                  traj_       = std::nullopt;
    std::optional<sensor_msgs::msg::LaserScan>          lidar_      = std::nullopt;
    std::optional<nav_msgs::msg::OccupancyGrid>         costmap_    = std::nullopt;
    std::optional<sensor_msgs::msg::Imu>                imu_        = std::nullopt;
    rclcpp::Time                    last_tick_;

    // Algorithm
    std::unique_ptr<AlgorithmRunner> runner_;

    // ROS pub/subs
    rclcpp::TimerBase::SharedPtr timer_;

    std::optional<rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr>             odom_sub_       = std::nullopt;
    std::optional<rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr>     goal_sub_       = std::nullopt;
    std::optional<rclcpp::Subscription<nav_msgs::msg::Path>::SharedPtr>                 path_sub_       = std::nullopt;
    std::optional<rclcpp::Subscription<nav_msgs::msg::Trajectory>::SharedPtr>           traj_sub_       = std::nullopt;
    std::optional<rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr>     lidar_sub_      = std::nullopt;
    std::optional<rclcpp::Subscription<nav_msgs::msg::CostMap>::SharedPtr>              costmap_sub_    = std::nullopt;
    std::optional<rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr>     imu_sub_        = std::nullopt;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr                      twist_pub_;
    rclcpp::Client<acre_ctrl::srv::Go2StartSequence>::SharedPtr                         start_client_;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);

    auto node = std::make_shared<PointControlNode>();

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