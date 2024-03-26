cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(fin_mechanism_plugin)

find_package(gazebo REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

include_directories(${GAZEBO_INCLUDE_DIRS})
link_directories(${GAZEBO_LIBRARY_DIRS})
list(APPEND CMAKE_CXX_FLAGS "${GAZEBO_CXX_FLAGS}")

add_library(fin_mechanism_plugin SHARED fin_mechanism_plugin.cc)
target_link_libraries(fin_mechanism_plugin ${GAZEBO_LIBRARIES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS}")