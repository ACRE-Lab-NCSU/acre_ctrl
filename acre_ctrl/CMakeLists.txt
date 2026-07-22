cmake_minimum_required(VERSION 3.8)
project(acre_ctrl)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-attributes)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(trajectory_msgs REQUIRED)
find_package(unitree_go REQUIRED)
find_package(unitree_api REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(acre_ctrl_msgs REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Development Interpreter)
find_package(pybind11 REQUIRED)
find_package(grid_map_ros REQUIRED)
find_package(grid_map_msgs REQUIRED)

# common/ros2_sport_client.hpp pulls in nlohmann/json.hpp and patch.hpp from the
# unitree_ros2_example repo - vendor those under include/ alongside it.
include_directories(include/acre_ctrl)

set(DEPENDENCY_LIST
  rclcpp
  geometry_msgs
  nav_msgs
  sensor_msgs
  trajectory_msgs
  unitree_go
  unitree_api
  acre_ctrl_msgs
  grid_map_ros
  grid_map_msgs
)

add_executable(sportmode_driver
  src/common/sportmode_driver.cpp
  src/common/ros2_sport_client.cpp
)
target_compile_features(sportmode_driver PRIVATE cxx_std_20)
ament_target_dependencies(sportmode_driver ${DEPENDENCY_LIST})

add_executable(control_node
  src/ctrl/control_node.cpp
  src/common/ros2_sport_client.cpp
)

target_compile_features(control_node PRIVATE cxx_std_20)
ament_target_dependencies(control_node ${DEPENDENCY_LIST})
target_link_libraries(control_node pybind11::embed)

add_executable(unitree_topic_bridge src/common/unitree_topic_bridge.cpp)
ament_target_dependencies(unitree_topic_bridge ${DEPENDENCY_LIST})

install(TARGETS
  sportmode_driver
  control_node
  unitree_topic_bridge
  DESTINATION lib/${PROJECT_NAME}
)

ament_python_install_package(${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()