option(ENABLE_OpenraveYarpWorld "Enable/disable OpenraveYarpWorld" ON)

if(ENABLE_OpenraveYarpWorld)

    add_library(OpenraveYarpWorld MODULE OpenraveYarpWorld.cpp
                                         OywPeriodicWrite.cpp
                                         OywPortReader.cpp
                                         OpenraveYarpWorld.hpp
                                         OywPeriodicWrite.hpp
                                         OywPortReader.hpp
                                         LogComponent.hpp
                                         LogComponent.cpp)


    target_link_libraries(OpenraveYarpWorld YARP::YARP_os
                                            YARP::YARP_init
                                            OpenRAVE::libopenrave
                                            Boost::boost)

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveYarpWorld)

endif()
