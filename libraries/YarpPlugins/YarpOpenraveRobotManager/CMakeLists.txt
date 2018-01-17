# Copyright: (C) 2013 Universidad Carlos III de Madrid
# Author: Juan G. Victores

yarp_prepare_plugin(YarpOpenraveRobotManager
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveRobotManager
                    INCLUDE YarpOpenraveRobotManager.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase)

if(NOT SKIP_YarpOpenraveRobotManager)

find_package(YARP 2.3.70 REQUIRED)
find_package(OpenRAVE REQUIRED)
find_package(Boost REQUIRED COMPONENTS iostreams python thread system)

get_property(_common_dirs GLOBAL PROPERTY ROBOTICSLAB_OPENRAVE_YARP_PLUGINS_INCLUDE_DIRS)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} # yarp plugin builder needs this
                    ${OpenRAVE_INCLUDE_DIRS}
                    ${_common_dirs})

if(Boost_INCLUDE_DIRS)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

yarp_add_plugin(YarpOpenraveRobotManager YarpOpenraveRobotManager.hpp
                                         IRobotManager.hpp
                                         DeviceDriverImpl.cpp
                                         IRobotManagerImpl.cpp)

add_dependencies(YarpOpenraveRobotManager COLOR_DEBUG)

set_target_properties(YarpOpenraveRobotManager PROPERTIES COMPILE_FLAGS "${OpenRAVE_CXX_FLAGS}"
                                                          LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

target_link_libraries(YarpOpenraveRobotManager YARP::YARP_OS
                                               YARP::YARP_dev
                                               ${OpenRAVE_CORE_LIBRARIES}
                                               ${Boost_THREAD_LIBRARY}
                                               ${Boost_SYSTEM_LIBRARY}
                                               YarpOpenraveBase)

yarp_install(TARGETS YarpOpenraveRobotManager
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
             ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

yarp_install(FILES YarpOpenraveRobotManager.ini
             COMPONENT runtime
             DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
