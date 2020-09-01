cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Start a project.
project(robotDraw LANGUAGES CXX)

find_package(YARP 3.3 REQUIRED COMPONENTS os dev)
find_package(COLOR_DEBUG REQUIRED)
find_package(ROBOTICSLAB_KINEMATICS_DYNAMICS REQUIRED)

# Set up our main executable.
add_executable(robotDraw main.cpp)

target_link_libraries(robotDraw YARP::YARP_os
                                YARP::YARP_init
                                YARP::YARP_dev
                                ROBOTICSLAB::KinematicsDynamicsInterfaces
                                ROBOTICSLAB::ColorDebug)

include(GNUInstallDirs)

install(TARGETS robotDraw
        DESTINATION ${CMAKE_INSTALL_BINDIR})
