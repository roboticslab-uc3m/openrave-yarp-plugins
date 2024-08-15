yarp_prepare_plugin(YarpOpenraveMeshFromRealDepth
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveMeshFromRealDepth
                    INCLUDE YarpOpenraveMeshFromRealDepth.hpp
                    DEFAULT ON
                    DEPENDS "ENABLE_YarpOpenraveBase;TARGET ROBOTICSLAB::YarpCloudUtils")

if(NOT SKIP_YarpOpenraveMeshFromRealDepth)

    yarp_add_plugin(YarpOpenraveMeshFromRealDepth YarpOpenraveMeshFromRealDepth.hpp
                                                  DeviceDriverImpl.cpp
                                                  PeriodicThreadImpl.cpp
                                                  LogComponent.hpp
                                                  LogComponent.cpp)

    target_link_libraries(YarpOpenraveMeshFromRealDepth YARP::YARP_os
                                                        YARP::YARP_dev
                                                        YARP::YARP_sig
                                                        OpenRAVE::libopenrave
                                                        Boost::boost
                                                        ROBOTICSLAB::YarpCloudUtils
                                                        YarpOpenraveBase)

    yarp_install(TARGETS YarpOpenraveMeshFromRealDepth
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
