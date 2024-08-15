option(ENABLE_OpenraveYarpForceEstimator "Enable/disable OpenraveYarpForceEstimator" ON)

if(ENABLE_OpenraveYarpForceEstimator)

    add_library(OpenraveYarpForceEstimator MODULE OpenraveYarpForceEstimator.cpp)

    target_link_libraries(OpenraveYarpForceEstimator YARP::YARP_os
                                                     YARP::YARP_init
                                                     OpenRAVE::libopenrave
                                                     Boost::boost)

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveYarpForceEstimator)

endif()
