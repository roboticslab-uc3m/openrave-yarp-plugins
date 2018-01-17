# Copyright: (C) 2013 Universidad Carlos III de Madrid
# Author: Juan G. Victores

yarp_prepare_plugin(YarpOpenraveControlboard
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveControlboard
                    INCLUDE YarpOpenraveControlboard.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase)

if(NOT SKIP_YarpOpenraveControlboard)

find_package(OpenRAVE REQUIRED)
find_package(Boost ${OpenRAVE_Boost_VERSION} EXACT REQUIRED COMPONENTS iostreams python thread system)

get_property(_common_dirs GLOBAL PROPERTY ROBOTICSLAB_OPENRAVE_YARP_PLUGINS_INCLUDE_DIRS)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} # yarp plugin builder needs this
                    ${OpenRAVE_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS}
                    ${_common_dirs}
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

yarp_install(TARGETS YarpOpenraveControlboard
             COMPONENT runtime
             LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
             ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

yarp_install(FILES YarpOpenraveControlboard.ini
             COMPONENT runtime
             DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
