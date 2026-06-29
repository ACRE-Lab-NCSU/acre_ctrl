cmake_minimum_required(VERSION 3.8)
project(acre_ctrl)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(unitree_go REQUIRED)
find_package(unitree_api REQUIRED)

# common/ros2_sport_client.h pulls in nlohmann/json.hpp and patch.hpp from the
# unitree_ros2_example repo - vendor those under include/ alongside it.
include_directories(include)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/Go2Mode.msg"
  "msg/Go2Gait.msg"
  "srv/Go2StartSequence.srv"
)

set(DEPENDENCY_LIST
  rclcpp
  geometry_msgs
  unitree_go
  unitree_api
)

add_executable(sportmode_driver src/sportmode_driver.cpp src/common/ros2_sport_client.cpp)
target_compile_features(sportmode_driver PRIVATE cxx_std_20)
ament_target_dependencies(sportmode_driver ${DEPENDENCY_LIST})
rosidl_target_interfaces(sportmode_driver ${PROJECT_NAME} "rosidl_typesupport_cpp")

add_executable(feedback_ctrl src/feedback_ctrl.cpp)
target_compile_features(feedback_ctrl PRIVATE cxx_std_20)
ament_target_dependencies(feedback_ctrl ${DEPENDENCY_LIST})
rosidl_target_interfaces(feedback_ctrl ${PROJECT_NAME} "rosidl_typesupport_cpp")

install(TARGETS
  sportmode_driver
  feedback_ctrl
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_dependencies(rosidl_default_runtime)
ament_package()
