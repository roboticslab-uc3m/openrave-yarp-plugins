option(ENABLE_OpenraveYarpForceEstimator "Enable/disable OpenraveYarpForceEstimator" ON)

if(ENABLE_OpenraveYarpForceEstimator)

    add_library(OpenraveYarpForceEstimator MODULE OpenraveYarpForceEstimator.cpp)

    target_link_libraries(OpenraveYarpForceEstimator YARP::YARP_os
                                                     YARP::YARP_init
                                                     Boost::boost)

    if(OpenRAVE_VERSION VERSION_GREATER_EQUAL 0.114)
        target_link_libraries(OpenraveYarpForceEstimator OpenRAVE::libopenrave)
    else()
        link_directories(${OpenRAVE_LIBRARY_DIRS})

        target_link_libraries(OpenraveYarpForceEstimator PUBLIC ${OpenRAVE_LIBRARIES})

        set_target_properties(OpenraveYarpForceEstimator PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                                    LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

        target_include_directories(OpenraveYarpForceEstimator PRIVATE ${OpenRAVE_INCLUDE_DIRS})
    endif()

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveYarpForceEstimator)

endif()
