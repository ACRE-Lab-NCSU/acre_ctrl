#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <rclcpp/executors/multi_threaded_executor.hpp>
#include <rclcpp/rclcpp.hpp>
#include "geometry_msgs/msg/twist_stamped.hpp"
#include <nav_msgs/msg/odometry.hpp>
#include "acre_ctrl/srv/go2_start_sequence.hpp"
#include "common/ros2_sport_client.h"
#include "unitree_go/msg/sport_mode_state.hpp"

#define SPORT_STATE_TOPIC      "rt/sportmodestate"
#define START_SEQUENCE_SERVICE "go2_start_sequence"

class Go2SportClientNode : public rclcpp::Node {
public:
    Go2SportClientNode()
        : Node("sportmode_driver"),
          sport_client_(this)
    {
        state_sub_ = this->create_subscription<unitree_go::msg::SportModeState>(
            SPORT_STATE_TOPIC, 1,
            [this](unitree_go::msg::SportModeState::SharedPtr data) {
                sport_state_handler(data);
            });

        start_sequence_srv_ = this->create_service<acre_ctrl::srv::Go2StartSequence>(
            START_SEQUENCE_SERVICE,
            [this](std::shared_ptr<acre_ctrl::srv::Go2StartSequence::Request> request,
                   std::shared_ptr<acre_ctrl::srv::Go2StartSequence::Response> response) {
                start_sequence(request, response);
            });

        odom_pub_ = this->create_publisher<nav_msgs::msg::Odometry>("acre/odom", 10);

        odom_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(10),
            std::bind(&Go2SportClientNode::publish_odom, this));

        sport_client_.Damp(req_);
    }

    void start_sequence(
        std::shared_ptr<acre_ctrl::srv::Go2StartSequence::Request> /*request*/,
        std::shared_ptr<acre_ctrl::srv::Go2StartSequence::Response> response)
    {
        RCLCPP_INFO(this->get_logger(), "Controller connected, running startup sequence");

        sport_client_.SwitchAvoidMode(req_);
        sport_client_.SwitchJoystick(req_, false);

        sport_client_.Damp(req_);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sport_client_.StandUp(req_);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sport_client_.FreeBound(req_, false);

        get_init_state();

        ctrl_ready_ = true;
        RCLCPP_INFO(this->get_logger(), "Startup sequence complete, ready for control");
        response->success = true;
        response->message = "ready";
    }

    void stop_sequence()
    {
        RCLCPP_INFO(this->get_logger(), "Running shutdown sequence");

        sport_client_.StandUp(req_);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sport_client_.Damp(req_);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sport_client_.SwitchJoystick(req_, true);
        ctrl_ready_ = false;
    }

private:
    void sport_state_handler(unitree_go::msg::SportModeState::SharedPtr msg)
    {
        state_         = *msg;
        state_received_ = true;
    }

    void publish_odom()
    {
        if (!state_received_) return;

        nav_msgs::msg::Odometry msg;
        msg.header.stamp    = this->now();
        msg.header.frame_id = "acre_odom";
        msg.child_frame_id  = "base_link";

        auto pos  = state_.position();    // std::array<float, 3>
        auto quat = state_.imu_state().quaternion();  // std::array<float, 4> (w,x,y,z)
        auto vel  = state_.velocity();    // std::array<float, 3>
        auto gyro = state_.imu_state().gyroscope();   // std::array<float, 3>

        msg.pose.pose.position.x = pos[0];
        msg.pose.pose.position.y = pos[1];
        msg.pose.pose.position.z = pos[2];

        // SDK quaternion order is (w, x, y, z)
        msg.pose.pose.orientation.w = quat[0];
        msg.pose.pose.orientation.x = quat[1];
        msg.pose.pose.orientation.y = quat[2];
        msg.pose.pose.orientation.z = quat[3];

        msg.pose.covariance.fill(0.0);

        msg.twist.twist.linear.x  = vel[0];
        msg.twist.twist.linear.y  = vel[1];
        msg.twist.twist.linear.z  = vel[2];

        msg.twist.twist.angular.x = gyro[0];
        msg.twist.twist.angular.y = gyro[1];
        msg.twist.twist.angular.z = state_.yaw_speed();

        msg.twist.covariance.fill(0.0);

        odom_pub_->publish(msg);
    }

    void get_init_state()
    {
        auto pos = state_.position();
        auto rpy = state_.imu_state().rpy();

        init_pos_x_ = pos[0];
        init_pos_y_ = pos[1];
        init_yaw_   = rpy[2];

        RCLCPP_INFO(this->get_logger(),
                    "Initial position: x=%.3f y=%.3f yaw=%.3f",
                    init_pos_x_, init_pos_y_, init_yaw_);
    }

    // State
    unitree_go::msg::SportModeState state_;
    bool   state_received_ = false;
    bool   ctrl_ready_     = false;
    double init_pos_x_     = 0.0;
    double init_pos_y_     = 0.0;
    double init_yaw_       = 0.0;

    // SDK
    SportClient               sport_client_;
    unitree_api::msg::Request req_;

    // ROS handles
    rclcpp::Subscription<unitree_go::msg::SportModeState>::SharedPtr state_sub_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr            odom_pub_;
    rclcpp::Service<acre_ctrl::srv::Go2StartSequence>::SharedPtr     start_sequence_srv_;
    rclcpp::TimerBase::SharedPtr                                     odom_timer_;
};

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Go2SportClientNode>();

    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(node);
    executor.spin();

    node->stop_sequence();
    rclcpp::shutdown();
    return EXIT_SUCCESS;
}