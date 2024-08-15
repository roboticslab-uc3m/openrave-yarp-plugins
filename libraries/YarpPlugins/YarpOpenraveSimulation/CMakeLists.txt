yarp_prepare_plugin(YarpOpenraveSimulation
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveSimulation
                    INCLUDE YarpOpenraveSimulation.hpp
                    DEFAULT ON
                    DEPENDS ENABLE_YarpOpenraveBase)

if(NOT SKIP_YarpOpenraveSimulation)

    yarp_add_plugin(YarpOpenraveSimulation YarpOpenraveSimulation.hpp
                                           DeviceDriverImpl.cpp
                                           ISimulationImpl.cpp
                                           LogComponent.hpp
                                           LogComponent.cpp)

    target_link_libraries(YarpOpenraveSimulation YARP::YARP_os
                                                 YARP::YARP_dev
                                                 OpenRAVE::libopenrave
                                                 Boost::boost
                                                 YarpOpenraveBase)

    yarp_install(TARGETS YarpOpenraveSimulation
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
