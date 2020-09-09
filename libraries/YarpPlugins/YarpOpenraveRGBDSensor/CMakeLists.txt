# Copyright: (C) 2013 Universidad Carlos III de Madrid
# Author: Juan G. Victores

yarp_prepare_plugin(YarpOpenraveRGBDSensor
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveRGBDSensor
                    INCLUDE YarpOpenraveRGBDSensor.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase
                    EXTRA_CONFIG WRAPPER=RGBDSensorWrapper)

if(NOT SKIP_YarpOpenraveRGBDSensor)

    if(NOT YARP_VERSION VERSION_GREATER_EQUAL 3.4)
        set(CMAKE_INCLUDE_CURRENT_DIR TRUE) # yarp plugin builder needs this
    endif()

    link_directories(${OpenRAVE_LIBRARY_DIRS})

    yarp_add_plugin(YarpOpenraveRGBDSensor YarpOpenraveRGBDSensor.hpp
                                           DeviceDriverImpl.cpp
                                           IRGBDSensorImpl.cpp)

    set_target_properties(YarpOpenraveRGBDSensor PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                            LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

    target_link_libraries(YarpOpenraveRGBDSensor YARP::YARP_os
                                                 YARP::YARP_dev
                                                 ${OpenRAVE_LIBRARIES}
                                                 Boost::boost
                                                 ROBOTICSLAB::ColorDebug
                                                 YarpOpenraveBase)

    target_include_directories(YarpOpenraveRGBDSensor PRIVATE ${OpenRAVE_INCLUDE_DIRS})

    yarp_install(TARGETS YarpOpenraveRGBDSensor
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
