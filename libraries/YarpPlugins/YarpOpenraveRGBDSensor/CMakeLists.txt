yarp_prepare_plugin(YarpOpenraveRGBDSensor
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveRGBDSensor
                    INCLUDE YarpOpenraveRGBDSensor.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase
                    EXTRA_CONFIG WRAPPER=rgbdSensor_nws_yarp)

if(NOT SKIP_YarpOpenraveRGBDSensor)

    yarp_add_plugin(YarpOpenraveRGBDSensor YarpOpenraveRGBDSensor.hpp
                                           DeviceDriverImpl.cpp
                                           IRGBDSensorImpl.cpp
                                           LogComponent.hpp
                                           LogComponent.cpp)

    target_link_libraries(YarpOpenraveRGBDSensor YARP::YARP_os
                                                 YARP::YARP_dev
                                                 OpenRAVE::libopenrave
                                                 Boost::boost
                                                 YarpOpenraveBase)

    yarp_install(TARGETS YarpOpenraveRGBDSensor
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
