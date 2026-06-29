#include <chrono>
#include <cmath>
#include <memory>
#include <rclcpp/executors/multi_threaded_executor.hpp>
#include <rclcpp/rclcpp.hpp>
#include "geometry_msgs/msg/twist_stamped.hpp"
#include <nav_msgs/msg/odometry.hpp>
#include <thread>

#include "acre_ctrl/srv/go2_start_sequence.hpp"
#include "common/ros2_sport_client.h"
#include "unitree_go/msg/sport_mode_state.hpp"

#define SPORT_STATE_TOPIC "lf/sportmodestate"
#define START_SEQUENCE_SERVICE "go2_start_sequence"
// https://docs.ros.org/en/humble/Tutorials/Beginner-Client-Libraries/Writing-A-Simple-Cpp-Service-And-Client.html

/**
 * There are other modes that we chose not to support
 * due to their impracticality. They could be added later
 * source: https://support.unitree.com/home/en/developer/Motion_Services_Interface_V2.0
*/

// TODO(follow-up): move/euler command subscription (Go2Ctrl EULAR/MOVE) is not
// wired up yet - the command message shape still needs to be decided. The
// driver currently only handles the start/stop handshake below.

// TODO: The Driver should publish the Go2 State to the controller.

class Go2SportClientNode : public rclcpp::Node {
 public:
  Go2SportClientNode()
      : Node("sportmode_driver"),
        sport_client(this) {
    state_sub = this->create_subscription<unitree_go::msg::SportModeState>(
        SPORT_STATE_TOPIC, 1,
        [this](const unitree_go::msg::SportModeState::SharedPtr data) {
          sport_state_handler(data);
        });

    start_sequence_srv = this->create_service<acre_ctrl::srv::Go2StartSequence>(
        START_SEQUENCE_SERVICE,
        [this](const std::shared_ptr<acre_ctrl::srv::Go2StartSequence::Request> request,
               std::shared_ptr<acre_ctrl::srv::Go2StartSequence::Response> response) {
          start_sequence(request, response);
        });

    odom_pub = this->create_publisher<nav_msgs::msg::Odometry>("acre_ctrl/odom", 10);
    // Stay damped until the controller requests the startup sequence.
    sport_client.Damps(req);
  }

  // Service callback: runs synchronously, so it doesn't reply to the
  // controller until the robot has actually finished standing up. The
  // controller's service call blocks on this same response.
  void start_sequence(
      const std::shared_ptr<acre_ctrl::srv::Go2StartSequence::Request> /*request*/,
      std::shared_ptr<acre_ctrl::srv::Go2StartSequence::Response> response) {
    RCLCPP_INFO(this->get_logger(), "Controller connected, running startup sequence");

    // Turn off obstacle avoidance
    sport_client.SwitchAvoidMode(req);

    // Disable joystick control while the feedback controller drives the robot
    sport_client.SwitchJoystick(req, false);

    // Damp for a second
    sport_client.Damp(req);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Stand for a second
    sport_client.StandUp(req);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Enter Agile mode (the only way to do this is by disabling FreeBound)
    sport_client.FreeBound(req, false);

    ctrl_ready = true;
    RCLCPP_INFO(this->get_logger(), "Startup sequence complete, ready for control commands");
    response->success = true;
    response->message = "ready";
  }

  void stop_sequence() {
    RCLCPP_INFO(this->get_logger(), "Running shutdown sequence");

    // Stand for a second
    sport_client.StandUp(req);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Damp for a second
    sport_client.Damp();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Hand control back to the joystick
    sport_client.SwitchJoystick(req, true);
    ctrl_ready = false;
  }

  void get_init_state() {
    init_pos_x = state.position[0];
    init_pos_y = state.position[1];
    init_yaw = state.imu_state.rpy[2];
    RCLCPP_INFO(this->get_logger(),
                "initial position: x0: %f, y0: %f, yaw0: %f", init_pos_x, init_pos_y,
                init_yaw);
  }

  void sport_state_handler(const unitree_go::msg::SportModeState::SharedPtr msg) {
    state = *msg;
    RCLCPP_INFO(this->get_logger(), "Position: %f, %f, %f", state.position[0],
                state.position[1], state.position[2]);
    RCLCPP_INFO(this->get_logger(), "IMU rpy: %f, %f, %f",
                state.imu_state.rpy[0], state.imu_state.rpy[1],
                state.imu_state.rpy[2]);

    // Build Odom message

    auto now = this->get_clock()->now();
    // Update poses based on velocity (replace with real data)
    x_ += 0.01; y_ += 0.005; theta_ += 0.01; 

    // Create and send transform
    geometry_msgs::msg::TransformStamped t;
    t.header.stamp = now;
    t.header.frame_id = "acre_ctrl_odom";
    t.child_frame_id = "base_link";
    t.transform.translation.x = x_;
    t.transform.translation.y = y_;
    t.transform.rotation.z = sin(theta_ / 2.0);
    t.transform.rotation.w = cos(theta_ / 2.0);
    tf_broadcaster_->sendTransform(t);

    // Create and publish message
    auto msg = nav_msgs::msg::Odometry();
    msg.header.stamp = now;
    msg.header.frame_id = "odom";
    msg.child_frame_id = "base_link";
    msg.pose.pose.position.x = x_;
    msg.pose.pose.position.y = y_;
    msg.pose.pose.orientation = t.transform.rotation;
    msg.twist.twist.linear.x = 0.1; // Velocity
    odom_pub_->publish(msg);

    // Publish Odom
    auto odom_msg = nav_msgs::msg::Odometry;
  }
  
 private:
  unitree_go::msg::SportModeState state;
  SportClient sport_client;

  // Pubs/Subs/Services
  rclcpp::Subscription<unitree_go::msg::SportModeState>::SharedPtr state_sub;
  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub
  rclcpp::Service<acre_ctrl::srv::Go2StartSequence>::SharedPtr start_sequence_srv;

  unitree_api::msg::Request req;  // Unitree Go2 ROS2 request message
  double init_pos_x{}, init_pos_y{}, init_yaw{};
  bool ctrl_ready{false};
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto sport_node = std::make_shared<Go2SportClientNode>();
  sport_node->get_init_state();

  rclcpp::executors::MultiThreadedExecutor executor;
  executor.add_node(sport_node);
  executor.spin();

  sport_node->stop_sequence();
  rclcpp::shutdown();
  return EXIT_SUCCESS;
}
