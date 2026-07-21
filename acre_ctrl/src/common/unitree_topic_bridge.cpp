#include "rclcpp/rclcpp.hpp"
#include "unitree_go/msg/sport_mode_state.hpp"
#include "nav_msgs/msg/odometry.hpp"

class SportOdomBridge : public rclcpp::Node
{
public:
  SportOdomBridge() : Node("sport_odom_bridge")
  {
    pub_ = this->create_publisher<nav_msgs::msg::Odometry>("/sport_odom", 10);

    sub_ = this->create_subscription<unitree_go::msg::SportModeState>(
      "/sportmodestate", 10,
      std::bind(&SportOdomBridge::callback, this, std::placeholders::_1));
  }

private:
  void callback(const unitree_go::msg::SportModeState::SharedPtr msg)
  {
    nav_msgs::msg::Odometry odom;
    odom.header.stamp = this->now();
    odom.header.frame_id = "odom";
    odom.child_frame_id = "base_link";

    odom.pose.pose.position.x = msg->position[0];
    odom.pose.pose.position.y = msg->position[1];
    odom.pose.pose.position.z = msg->position[2];

    // imu_state.quaternion order is [w, x, y, z]
    odom.pose.pose.orientation.w = msg->imu_state.quaternion[0];
    odom.pose.pose.orientation.x = msg->imu_state.quaternion[1];
    odom.pose.pose.orientation.y = msg->imu_state.quaternion[2];
    odom.pose.pose.orientation.z = msg->imu_state.quaternion[3];

    odom.twist.twist.linear.x = msg->velocity[0];
    odom.twist.twist.linear.y = msg->velocity[1];
    odom.twist.twist.linear.z = msg->velocity[2];
    odom.twist.twist.angular.z = msg->yaw_speed;

    pub_->publish(odom);
  }

  rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr pub_;
  rclcpp::Subscription<unitree_go::msg::SportModeState>::SharedPtr sub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SportOdomBridge>());
  rclcpp::shutdown();
  return 0;
}