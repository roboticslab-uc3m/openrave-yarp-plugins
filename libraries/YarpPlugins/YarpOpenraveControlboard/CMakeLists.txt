# Copyright: (C) 2013 Universidad Carlos III de Madrid
# Author: Juan G. Victores

yarp_prepare_plugin(YarpOpenraveControlboard
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveControlboard
                    INCLUDE YarpOpenraveControlboard.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase)

if(NOT SKIP_YarpOpenraveControlboard)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} # yarp plugin builder needs this
                    ${OpenRAVE_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS}
                    $<TARGET_PROPERTY:YarpOpenraveBase,INTERFACE_INCLUDE_DIRECTORIES>)

link_directories(${OpenRAVE_LIBRARY_DIRS}
                 ${Boost_LIBRARY_DIRS})

yarp_add_plugin(YarpOpenraveControlboard YarpOpenraveControlboard.hpp
                                         DeviceDriverImpl.cpp
                                         IControlLimits2Impl.cpp
                                         IControlMode2Impl.cpp
                                         IEncodersImpl.cpp
                                         IEncodersTimedImpl.cpp
                                         IPositionControlImpl.cpp
                                         IPositionControl2Impl.cpp
                                         IPositionDirectImpl.cpp
                                         ITorqueControlImpl.cpp
                                         IVelocityControlImpl.cpp
                                         IVelocityControl2Impl.cpp
                                         $<TARGET_OBJECTS:YarpOpenraveBase>)

add_dependencies(YarpOpenraveControlboard COLOR_DEBUG)

set_target_properties(YarpOpenraveControlboard PROPERTIES COMPILE_FLAGS "${OpenRAVE_CXX_FLAGS}"
                                                          LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

target_link_libraries(YarpOpenraveControlboard YARP::YARP_OS
                                               YARP::YARP_dev
                                               ${OpenRAVE_CORE_LIBRARIES}
                                               ${Boost_THREAD_LIBRARY}
                                               ${Boost_SYSTEM_LIBRARY})

# Hack, fool CMake to think that YARP already switched to 3.0 while still at devel.
if(NOT YARP_VERSION_SHORT VERSION_LESS 2.3.73)
    target_compile_definitions(YarpOpenraveControlboard PUBLIC YARP_VERSION_MAJOR=3)
endif()

yarp_install(TARGETS YarpOpenraveControlboard
             COMPONENT runtime
             LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
             ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR})

yarp_install(FILES YarpOpenraveControlboard.ini
             COMPONENT runtime
             DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
