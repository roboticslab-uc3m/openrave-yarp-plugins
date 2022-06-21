option(ENABLE_OpenraveYarpPluginLoader "Enable/disable OpenraveYarpPluginLoader" ON)

if(ENABLE_OpenraveYarpPluginLoader)

    include_directories(${OpenRAVE_INCLUDE_DIRS})

    link_directories(${OpenRAVE_LIBRARY_DIRS})

    add_library(OpenraveYarpPluginLoader MODULE OpenraveYarpPluginLoader.cpp
                                                OyplPeriodicWrite.cpp
                                                OyplPortReader.cpp
                                                OpenraveYarpPluginLoader.hpp
                                                OyplPeriodicWrite.hpp
                                                OyplPortReader.hpp
                                                LogComponent.hpp
                                                LogComponent.cpp)

    set_target_properties(OpenraveYarpPluginLoader PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                              LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

    target_link_libraries(OpenraveYarpPluginLoader YARP::YARP_os
                                                   YARP::YARP_init
                                                   YARP::YARP_dev
                                                   ${OpenRAVE_LIBRARIES}
                                                   Boost::boost)

    include(InstallOpenravePlugin)

    install_openrave_plugin(OpenraveYarpPluginLoader)

endif()
