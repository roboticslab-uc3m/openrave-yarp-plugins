yarp_prepare_plugin(YarpOpenraveAnalogSensors
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveAnalogSensors
                    INCLUDE YarpOpenraveAnalogSensors.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase
                    EXTRA_CONFIG WRAPPER=multipleanalogsensorsserver)

if(NOT SKIP_YarpOpenraveAnalogSensors)

    yarp_add_plugin(YarpOpenraveAnalogSensors YarpOpenraveAnalogSensors.hpp
                                              DeviceDriverImpl.cpp
                                              ISixAxisForceTorqueSensorsImpl.cpp
                                              LogComponent.hpp
                                              LogComponent.cpp)

    target_link_libraries(YarpOpenraveAnalogSensors YARP::YARP_os
                                                    YARP::YARP_dev
                                                    OpenRAVE::libopenrave
                                                    Boost::boost
                                                    YarpOpenraveBase)

    yarp_install(TARGETS YarpOpenraveAnalogSensors
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
