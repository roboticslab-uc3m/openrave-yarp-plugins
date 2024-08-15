option(ENABLE_OpenraveYarpPlanner "Enable/disable OpenraveYarpPlanner" ON)

if(ENABLE_OpenraveYarpPlanner)

    add_library(OpenraveYarpPlanner MODULE OpenraveYarpPlanner.cpp
                                           OypPortReader.cpp
                                           OpenraveYarpPlanner.hpp
                                           OypPortReader.hpp
                                           LogComponent.hpp
                                           LogComponent.cpp)

    target_link_libraries(OpenraveYarpPlanner YARP::YARP_os
                                              YARP::YARP_init
                                              YARP::YARP_dev
                                              OpenRAVE::libopenrave
                                              Boost::boost)

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveYarpPlanner)

endif()
