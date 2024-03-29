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
                                                 Boost::boost
                                                 YarpOpenraveBase)

    if(OpenRAVE_VERSION VERSION_GREATER_EQUAL 0.114)
        target_link_libraries(YarpOpenraveSimulation OpenRAVE::libopenrave)
    else()
        link_directories(${OpenRAVE_LIBRARY_DIRS})

        target_link_libraries(YarpOpenraveSimulation ${OpenRAVE_LIBRARIES})

        set_target_properties(YarpOpenraveSimulation PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                                LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

        target_include_directories(YarpOpenraveSimulation PRIVATE ${OpenRAVE_INCLUDE_DIRS})
    endif()

    yarp_install(TARGETS YarpOpenraveSimulation
                 LIBRARY DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_STATIC_PLUGINS_INSTALL_DIR}
                 YARP_INI DESTINATION ${ROBOTICSLAB-OPENRAVE-YARP-PLUGINS_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
