#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "acre_ctrl/srv/go2_start_sequence.hpp"
#include "algorithm_runner.hpp"
#include "input_builders.hpp"

#define START_SEQUENCE_SERVICE "go2_start_sequence"

class PointControlNode : public rclcpp::Node {
public:
    PointControlNode() : Node("point_control_node")
    {
        // Declare parameters
        this->declare_parameter("frequency",  100.0);
        this->declare_parameter("algorithm",  "");

        this->get_parameter("frequency",  frequency_);
        this->get_parameter("algorithm",  algo_path_);

        // Load and validate algorithm
        runner_ = std::make_unique<AlgorithmRunner>(algo_path_);
        if (runner_->input_type() != AlgorithmInputType::PointInput)
            throw std::runtime_error("This node requires a PointInput algorithm");

        // Set up subscriptions and publisher
        odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "odom", 10,
            [this](nav_msgs::msg::Odometry::SharedPtr msg) { odom_ = *msg; });

        goal_sub_ = this->create_subscription<geometry_msgs::msg::PoseStamped>(
            "goal_pose", 10,
            [this](geometry_msgs::msg::PoseStamped::SharedPtr msg) { goal_ = *msg; });

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
        // Track dt
        auto now = this->now();
        double dt = (now - last_tick_).seconds();
        last_tick_ = now;

        // Build input — no ROS types past this line
        PointInput input;
        input.odom = buildOdom(odom_, dt);
        input.goal = buildGoal(input.odom, goal_);

        // Run algorithm
        auto out = runner_->compute(&input);

        // Publish
        geometry_msgs::msg::TwistStamped msg;
        msg.header.stamp    = now;
        msg.header.frame_id = "base_link";
        msg.twist.linear.x  = out.linear_vel;
        msg.twist.angular.z = out.angular_vel;
        twist_pub_->publish(msg);
    }

    // Parameters
    double      frequency_ = 100.0;
    std::string algo_path_;

    // State
    nav_msgs::msg::Odometry         odom_;
    geometry_msgs::msg::PoseStamped goal_;
    rclcpp::Time                    last_tick_;

    // Algorithm
    std::unique_ptr<AlgorithmRunner> runner_;

    // ROS handles
    rclcpp::TimerBase::SharedPtr                                     timer_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr         odom_sub_;
    rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr goal_sub_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr   twist_pub_;
    rclcpp::Client<acre_ctrl::srv::Go2StartSequence>::SharedPtr      start_client_;
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