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
                                                        Boost::boost
                                                        ROBOTICSLAB::YarpCloudUtils
                                                        YarpOpenraveBase)

    if(OpenRAVE_VERSION VERSION_GREATER_EQUAL 0.114)
        target_link_libraries(YarpOpenraveMeshFromRealDepth OpenRAVE::libopenrave)
    else()
        link_directories(${OpenRAVE_LIBRARY_DIRS})

        target_link_libraries(YarpOpenraveMeshFromRealDepth ${OpenRAVE_LIBRARIES})

        set_target_properties(YarpOpenraveMeshFromRealDepth PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                                       LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

        target_include_directories(YarpOpenraveMeshFromRealDepth PRIVATE ${OpenRAVE_INCLUDE_DIRS})
    endif()

    yarp_install(TARGETS YarpOpenraveMeshFromRealDepth
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
