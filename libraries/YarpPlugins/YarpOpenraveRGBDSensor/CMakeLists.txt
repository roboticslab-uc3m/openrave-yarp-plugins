# Copyright: (C) 2013 Universidad Carlos III de Madrid
# Author: Juan G. Victores

yarp_prepare_plugin(YarpOpenraveRGBDSensor
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveRGBDSensor
                    INCLUDE YarpOpenraveRGBDSensor.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase)

if(NOT SKIP_YarpOpenraveRGBDSensor)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} # yarp plugin builder needs this
                    ${OpenRAVE_INCLUDE_DIRS})

link_directories(${OpenRAVE_LIBRARY_DIRS})

yarp_add_plugin(YarpOpenraveRGBDSensor YarpOpenraveRGBDSensor.hpp
                                       DeviceDriverImpl.cpp
                                       IRGBDSensorImpl.cpp)

set_target_properties(YarpOpenraveRGBDSensor PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                        LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

target_link_libraries(YarpOpenraveRGBDSensor YARP::YARP_OS
                                             YARP::YARP_dev
                                             ${OpenRAVE_LIBRARIES}
                                             Boost::boost
                                             ROBOTICSLAB::ColorDebug
                                             YarpOpenraveBaseInterface)

# https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/92
if(NOT YARP_VERSION_SHORT VERSION_GREATER 3.1.0)
    target_compile_definitions(YarpOpenraveRGBDSensor PRIVATE -DOR_RGBDSENSOR_IFRAMEGRABBERCONTROLS_IMPL)
endif()

yarp_install(TARGETS YarpOpenraveRGBDSensor
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
             ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR})

yarp_install(FILES YarpOpenraveRGBDSensor.ini
             COMPONENT runtime
             DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
