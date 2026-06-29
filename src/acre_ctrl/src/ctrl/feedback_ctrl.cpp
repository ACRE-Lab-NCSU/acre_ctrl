#include <chrono>
#include <memory>

#include "acre_ctrl/srv/go2_start_sequence.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

#define FREQUENCY_CONVERSION = 1000
#define START_SEQUENCE_SERVICE "go2_start_sequence"

// TODO(follow-up): the actual control command published here (vx/vy/vyaw vs.
// euler) is not finalized yet - this just demonstrates the handshake gate.

// The Node should accept arguments for a control Algorithm and frequency
// We need to convert from frequency to period = 1 / freq

class FeedbackCtrl : public rclcpp::Node {
 public:
  FeedbackCtrl() : Node("feedback_ctrl") {

    this->get_parameter("frequency", frequency);
    twist_publisher = this->create_publisher<geometry_msgs::msg::TwistStamped>("cmd_vel", 10);
    start_client = this->create_client<acre_ctrl::srv::Go2StartSequence>(START_SEQUENCE_SERVICE);
  }

  // Blocks until the driver replies that the startup sequence is complete.
  // Returns false if the service never becomes available or the call fails.
  bool wait_for_start_sequence() {
    while (!start_client->wait_for_service(1s)) {
      RCLCPP_INFO(this->get_logger(), "Waiting for %s service...", START_SEQUENCE_SERVICE);
    }

    auto request = std::make_shared<acre_ctrl::srv::Go2StartSequence::Request>();
    auto future = start_client->async_send_request(request);

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

  // Only call this once wait_for_start_sequence() has returned true.
  void start_control_loop() {
    timer = this->create_wall_timer({FREQUENCY_CONVERSION / frequency}ms, std::bind(&FeedbackCtrl::ctrl_callback, this));
  }

 private:
  void ctrl_callback() {
    // Execute control algorithm
    auto twist_msg = geometry_msgs::msg::TwistStamped();
    twist_msg.header.stamp = this->now();
    twist_msg.header.frame_id = "base_link";

    twist_msg.twist.linear.x = 0.0;
    twist_msg.twist.linear.y = 0.0;
    twist_msg.twist.linear.z = 0.0;

    twist_msg.twist.angular.x = 0.0;
    twist_msg.twist.angular.y = 0.0;
    twist_msg.twist.angular.z = 0.0;

    twist_publisher->publish(twist_msg);
  }

  double frequency;
  rclcpp::TimerBase::SharedPtr timer;
  rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr twist_publisher;
  rclcpp::Client<acre_ctrl::srv::Go2StartSequence>::SharedPtr start_client;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<FeedbackCtrl>();

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
