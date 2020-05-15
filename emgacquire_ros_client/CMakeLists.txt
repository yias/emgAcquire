cmake_minimum_required(VERSION 2.8.3)
project(emgacquire_ros_client)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
)

#set(ENV{CPP_DEPENDENCIES} /home/iasonpc/cpp_ws/dependencies/)
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES socketstream_node
 CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
)

###########
## Build ##
###########

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  $ENV{CPP_DEPENDENCIES}/rapidjson/include
  include/socketStream
  include/emgAcquire
)

add_executable(emgAcquireClient src/emgAcquireClient_node.cpp src/emgAcquire.cpp src/acquireFilters.cpp src/socketStream.cpp src/jsonWrapper.cpp src/md5.cpp)
target_link_libraries(emgAcquireClient ${catkin_LIBRARIES})
#add_dependencies(socketStream beginner_tutorials_generate_messages_cpp)
