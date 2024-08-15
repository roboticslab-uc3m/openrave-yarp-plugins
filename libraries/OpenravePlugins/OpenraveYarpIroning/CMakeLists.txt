option(ENABLE_OpenraveYarpIroning "Enable/disable OpenraveYarpIroning" ON)

if(ENABLE_OpenraveYarpIroning)

    add_library(OpenraveYarpIroning MODULE OpenraveYarpIroning.cpp)

    target_link_libraries(OpenraveYarpIroning YARP::YARP_os
                                                   YARP::YARP_init
                                                   OpenRAVE::libopenrave
                                                   Boost::boost)

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveYarpIroning)

endif()
