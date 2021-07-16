# Copyright: (C) 2019 Universidad Carlos III de Madrid
# Author: Juan G. Victores

yarp_prepare_plugin(YarpOpenraveAnalogSensors
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveAnalogSensors
                    INCLUDE YarpOpenraveAnalogSensors.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase
                    EXTRA_CONFIG WRAPPER=multipleanalogsensorsserver)

if(NOT SKIP_YarpOpenraveAnalogSensors)

    link_directories(${OpenRAVE_LIBRARY_DIRS})

    yarp_add_plugin(YarpOpenraveAnalogSensors YarpOpenraveAnalogSensors.hpp
                                              DeviceDriverImpl.cpp
                                              ISixAxisForceTorqueSensorsImpl.cpp
                                              LogComponent.hpp
                                              LogComponent.cpp)

    set_target_properties(YarpOpenraveAnalogSensors PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                               LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

    target_link_libraries(YarpOpenraveAnalogSensors YARP::YARP_os
                                                    YARP::YARP_dev
                                                    ${OpenRAVE_LIBRARIES}
                                                    Boost::boost
                                                    YarpOpenraveBase)

    target_include_directories(YarpOpenraveAnalogSensors PRIVATE ${OpenRAVE_INCLUDE_DIRS})

    yarp_install(TARGETS YarpOpenraveAnalogSensors
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
