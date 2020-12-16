yarp_prepare_plugin(YarpOpenraveMeshFromRealDepth
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveMeshFromRealDepth
                    INCLUDE YarpOpenraveMeshFromRealDepth.hpp
                    DEFAULT ON
                    DEPENDS "ENABLE_YarpOpenraveBase;TARGET ROBOTICSLAB::YarpCloudUtils")

if(NOT SKIP_YarpOpenraveMeshFromRealDepth)

    if(NOT YARP_VERSION VERSION_GREATER_EQUAL 3.4)
        set(CMAKE_INCLUDE_CURRENT_DIR TRUE) # yarp plugin builder needs this
    endif()

    link_directories(${OpenRAVE_LIBRARY_DIRS})

    yarp_add_plugin(YarpOpenraveMeshFromRealDepth YarpOpenraveMeshFromRealDepth.hpp
                                                  DeviceDriverImpl.cpp
                                                  PeriodicThreadImpl.cpp)

    set_target_properties(YarpOpenraveMeshFromRealDepth PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                                   LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

    target_link_libraries(YarpOpenraveMeshFromRealDepth YARP::YARP_os
                                                        YARP::YARP_dev
                                                        YARP::YARP_sig
                                                        ${OpenRAVE_LIBRARIES}
                                                        Boost::boost
                                                        ROBOTICSLAB::YarpCloudUtils
                                                        YarpOpenraveBase)

    target_include_directories(YarpOpenraveMeshFromRealDepth PRIVATE ${OpenRAVE_INCLUDE_DIRS})

    target_compile_features(YarpOpenraveMeshFromRealDepth PRIVATE cxx_std_14)

    yarp_install(TARGETS YarpOpenraveMeshFromRealDepth
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
