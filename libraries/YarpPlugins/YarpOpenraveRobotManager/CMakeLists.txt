# Copyright: (C) 2013 Universidad Carlos III de Madrid
# Author: Juan G. Victores

if(NOT ASROB_YARP_DEVICES_FOUND AND (NOT DEFINED ENABLE_YarpOpenraveRobotManager OR ENABLE_YarpOpenraveRobotManager))
    message(WARNING "ASROB_YARP_DEVICES package not found, disabling YarpOpenraveRobotManager")
endif()

yarp_prepare_plugin(YarpOpenraveRobotManager
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveRobotManager
                    INCLUDE YarpOpenraveRobotManager.hpp
                    DEFAULT ON
                    DEPENDS "ENABLE_YarpOpenraveBase;ASROB_YARP_DEVICES_FOUND"
                    EXTRA_CONFIG WRAPPER=RobotServer)

if(NOT SKIP_YarpOpenraveRobotManager)

    link_directories(${OpenRAVE_LIBRARY_DIRS})

    yarp_add_plugin(YarpOpenraveRobotManager YarpOpenraveRobotManager.hpp
                                             DeviceDriverImpl.cpp
                                             IRobotManagerImpl.cpp)

    set_target_properties(YarpOpenraveRobotManager PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                              LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

    target_link_libraries(YarpOpenraveRobotManager YARP::YARP_os
                                                   YARP::YARP_dev
                                                   ${OpenRAVE_LIBRARIES}
                                                   ASROB::RobotInterfaces
                                                   YarpOpenraveBase)

    target_include_directories(YarpOpenraveRobotManager PRIVATE ${OpenRAVE_INCLUDE_DIRS})

    yarp_install(TARGETS YarpOpenraveRobotManager
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

else()

    set(ENABLE_YarpOpenraveRobotManager OFF CACHE BOOL "Enable/disable YarpOpenraveRobotManager device" FORCE)

endif()
