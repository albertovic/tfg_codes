cmake_minimum_required(VERSION 3.8)
project(gazebo_plugin)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(gazebo_ros REQUIRED)
find_package(gazebo REQUIRED)
find_package(std_msgs REQUIRED)


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

# to compile the plugin we need these lines
add_library(fin_mechanism_plugin SHARED ./src/fin_mechanism_plugin.cpp)
ament_target_dependencies(fin_mechanism_plugin rclcpp gazebo gazebo_ros std_msgs)
target_link_libraries(fin_mechanism_plugin ${GAZEBO_LIBRARIES})

ament_package()
