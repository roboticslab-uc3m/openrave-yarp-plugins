option(ENABLE_OpenraveYarpPluginLoader "Enable/disable OpenraveYarpPluginLoader" ON)

if(ENABLE_OpenraveYarpPluginLoader)

    add_library(OpenraveYarpPluginLoader MODULE OpenraveYarpPluginLoader.cpp
                                                OyplPeriodicWrite.cpp
                                                OyplPortReader.cpp
                                                OpenraveYarpPluginLoader.hpp
                                                OyplPeriodicWrite.hpp
                                                OyplPortReader.hpp
                                                LogComponent.hpp
                                                LogComponent.cpp)

    target_link_libraries(OpenraveYarpPluginLoader YARP::YARP_os
                                                   YARP::YARP_init
                                                   YARP::YARP_dev
                                                   OpenRAVE::libopenrave
                                                   Boost::boost)

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveYarpPluginLoader)

endif()
