
option(ENABLE_OpenraveForceSensor "Enable/disable OpenraveForceSensor" ON)

if(ENABLE_OpenraveForceSensor)

    add_library(OpenraveForceSensor MODULE ForceSensor.cpp
                                           ForceSensorMain.cpp
                                           ForceSensor.h)

    target_link_libraries(OpenraveForceSensor YARP::YARP_os
                                              YARP::YARP_init
                                              OpenRAVE::libopenrave
                                              Boost::boost)

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveForceSensor)

endif()
