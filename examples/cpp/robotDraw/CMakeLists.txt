cmake_minimum_required(VERSION 3.19)

project(robotDraw LANGUAGES CXX)

if(NOT YARP_FOUND)
    set(_yarp_components os dev)
    find_package(YARP 3.12 QUIET COMPONENTS ${_yarp_components})

    if(NOT YARP_FOUND)
        find_package(YARP 4.0 REQUIRED COMPONENTS ${_yarp_components})
    endif()
endif()

find_package(ROBOTICSLAB_KINEMATICS_DYNAMICS REQUIRED)

add_executable(robotDraw main.cpp)

target_link_libraries(robotDraw YARP::YARP_os
                                YARP::YARP_init
                                YARP::YARP_dev
                                ROBOTICSLAB::KinematicsDynamicsInterfaces)
