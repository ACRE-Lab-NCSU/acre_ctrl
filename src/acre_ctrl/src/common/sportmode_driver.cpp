#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <rclcpp/executors/multi_threaded_executor.hpp>
#include <rclcpp/rclcpp.hpp>
#include "geometry_msgs/msg/twist_stamped.hpp"
#include <nav_msgs/msg/odometry.hpp>
#include "acre_ctrl/srv/go2_start_sequence.hpp"
#include "common/ros2_sport_client.hpp"
#include "unitree_go/msg/sport_mode_state.hpp"

#define SPORT_STATE_TOPIC      "lf/sportmodestate"
#define START_SEQUENCE_SERVICE "go2_start_sequence"

enum class GaitMode {
    AGILE,
    CLASSIC,
};


class Go2SportClientNode : public rclcpp::Node {
public:
    Go2SportClientNode()
        : Node("sportmode_driver"),
          sport_client_(this)
    {
        // Declare parameters
        this->declare_parameter("gait",  "classic");

        std::string gait_str = this->get_parameter("gait").as_string();
        if (gait_to_enum_.find(gait_str) == gait_to_enum_.end()) {
            RCLCPP_FATAL(this->get_logger(), "Invalid gait '%s'", gait_str.c_str());
            throw std::runtime_error("Invalid gait_mode parameter");
        }
        gait_ = gait_to_enum_.at(gait_str);

        state_sub_ = this->create_subscription<unitree_go::msg::SportModeState>(
            SPORT_STATE_TOPIC, 1,
            [this](unitree_go::msg::SportModeState::SharedPtr data) {
                sport_state_handler(data);
            });

        cmd_sub_ = this->create_subscription<geometry_msgs::msg::TwistStamped>(
            "cmd_vel", 1,
            [this](geometry_msgs::msg::TwistStamped::SharedPtr msg) {
                cmd_callback(msg);
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

        RCLCPP_INFO(this->get_logger(), "Go2 connected, waiting for controller...");
    }

    void start_sequence(
        std::shared_ptr<acre_ctrl::srv::Go2StartSequence::Request> /*request*/,
        std::shared_ptr<acre_ctrl::srv::Go2StartSequence::Response> response)
    {
        RCLCPP_INFO(this->get_logger(), "Controller connected, running startup sequence");
        unitree_api::msg::Request req;

        sport_client_.Damp(req);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sport_client_.StandUp(req);            // damping -> standing lock
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sport_client_.BalanceStand(req);       // standing lock -> balanced standing
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sport_client_.ClassicWalk(req, true);       // balanced standing -> ClassicWalk
        if (gait_ == GaitMode::AGILE) {
            sport_client_.ClassicWalk(req, false); 
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        //sport_client_.SwitchAvoidMode(req);
        sport_client_.SwitchJoystick(req, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        get_init_state();

        ctrl_ready_ = true;
        RCLCPP_INFO(this->get_logger(), "Startup sequence complete, ready for control");
        response->success = true;
        response->message = "ready";
    }

    void stop_sequence()
    {
        RCLCPP_INFO(this->get_logger(), "Running shutdown sequence");
        unitree_api::msg::Request req;

        sport_client_.BalanceStand(req);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sport_client_.StandUp(req);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sport_client_.Damp(req);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        sport_client_.SwitchJoystick(req, true);
        ctrl_ready_ = false;
    }

private:
    void sport_state_handler(unitree_go::msg::SportModeState::SharedPtr msg)
    {
        state_         = *msg;
        state_received_ = true;
    }

    void cmd_callback(geometry_msgs::msg::TwistStamped::SharedPtr msg) 
    {
        if (!ctrl_ready_) return;  // ignore commands until startup is complete

        float vx   = static_cast<float>(msg->twist.linear.x);
        float vy   = static_cast<float>(msg->twist.linear.y);
        float vyaw = static_cast<float>(msg->twist.angular.z);

        unitree_api::msg::Request req;
        sport_client_.Move(req, vx, vy, vyaw);
    }

    void publish_odom()
    {
        if (!state_received_) return;

        nav_msgs::msg::Odometry msg;
        msg.header.stamp    = this->now();
        msg.header.frame_id = "acre_odom";
        msg.child_frame_id  = "base_link";

        auto pos  = state_.position;
        auto quat = state_.imu_state.quaternion;
        auto vel  = state_.velocity;
        auto gyro = state_.imu_state.gyroscope;

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
        msg.twist.twist.angular.z = state_.yaw_speed;

        msg.twist.covariance.fill(0.0);

        odom_pub_->publish(msg);
    }

    void get_init_state()
    {
        auto pos = state_.position;
        auto rpy = state_.imu_state.rpy;

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
    const std::unordered_map<std::string, GaitMode> gait_to_enum_ = {
        {"agile",       GaitMode::AGILE},
        {"classic",     GaitMode::CLASSIC},
    };

    GaitMode gait_;

    // ROS handles
    rclcpp::Subscription<unitree_go::msg::SportModeState>::SharedPtr state_sub_;
    rclcpp::Subscription<geometry_msgs::msg::TwistStamped>::SharedPtr cmd_sub_;
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