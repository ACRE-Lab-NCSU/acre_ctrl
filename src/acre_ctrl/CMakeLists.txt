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
find_package(nav_msgs REQUIRED)
find_package(unitree_go REQUIRED)
find_package(unitree_api REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Development Interpreter)
find_package(pybind11 REQUIRED)

# common/ros2_sport_client.hpp pulls in nlohmann/json.hpp and patch.hpp from the
# unitree_ros2_example repo - vendor those under include/ alongside it.
include_directories(include/acre_ctrl)

rosidl_generate_interfaces(${PROJECT_NAME}
  "srv/Go2StartSequence.srv"
)

set(DEPENDENCY_LIST
  rclcpp
  geometry_msgs
  nav_msgs
  unitree_go
  unitree_api
)

add_executable(sportmode_driver
  src/common/sportmode_driver.cpp
  src/common/ros2_sport_client.cpp
)
target_compile_features(sportmode_driver PRIVATE cxx_std_20)
ament_target_dependencies(sportmode_driver ${DEPENDENCY_LIST})
rosidl_target_interfaces(sportmode_driver ${PROJECT_NAME} "rosidl_typesupport_cpp")

add_executable(point_control_node
  src/ctrl/point_control_node.cpp
  src/common/ros2_sport_client.cpp
)
target_compile_features(point_control_node PRIVATE cxx_std_20)
ament_target_dependencies(point_control_node ${DEPENDENCY_LIST})
rosidl_target_interfaces(point_control_node ${PROJECT_NAME} "rosidl_typesupport_cpp")
target_link_libraries(point_control_node pybind11::embed)

# spin_sample is a plugin loaded at runtime (REGISTER_ALGORITHM exposes
# extern "C" algo_create/algo_destroy/etc for dlopen), not a standalone
# executable - build it as a shared library.
add_library(spin_sample SHARED
  src/algo/spin_sample.cpp
)
target_compile_features(spin_sample PRIVATE cxx_std_20)
ament_target_dependencies(spin_sample ${DEPENDENCY_LIST})
rosidl_target_interfaces(spin_sample ${PROJECT_NAME} "rosidl_typesupport_cpp")

install(TARGETS
  sportmode_driver
  point_control_node
  DESTINATION lib/${PROJECT_NAME}
)

install(TARGETS
  spin_sample
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_dependencies(rosidl_default_runtime)
ament_package()