# Copyright: (C) 2019 Universidad Carlos III de Madrid
# Author: Juan G. Victores

yarp_prepare_plugin(YarpOpenraveFTSensors
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveFTSensors
                    INCLUDE YarpOpenraveFTSensors.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase
                    EXTRA_CONFIG WRAPPER=multipleanalogsensorsserver)

if(NOT SKIP_YarpOpenraveFTSensors)

    set(CMAKE_INCLUDE_CURRENT_DIR TRUE) # yarp plugin builder needs this

    link_directories(${OpenRAVE_LIBRARY_DIRS})

    yarp_add_plugin(YarpOpenraveFTSensors YarpOpenraveFTSensors.hpp
                                          DeviceDriverImpl.cpp
                                          ISixAxisForceTorqueSensorsImpl.cpp)

    set_target_properties(YarpOpenraveFTSensors PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                           LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

    target_link_libraries(YarpOpenraveFTSensors YARP::YARP_OS
                                                YARP::YARP_dev
                                                ${OpenRAVE_LIBRARIES}
                                                Boost::boost
                                                ROBOTICSLAB::ColorDebug
                                                YarpOpenraveBase)

    target_include_directories(YarpOpenraveFTSensors PRIVATE ${OpenRAVE_INCLUDE_DIRS})

    yarp_install(TARGETS YarpOpenraveFTSensors
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
