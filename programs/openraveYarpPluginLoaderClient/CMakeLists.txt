option(ENABLE_openraveYarpPluginLoaderClient "Enable/disable openraveYarpPluginLoaderClient program" ON)

if(ENABLE_openraveYarpPluginLoaderClient)

add_executable(openraveYarpPluginLoaderClient main.cpp
                                              OpenraveYarpPluginLoaderClient.cpp
                                              OpenraveYarpPluginLoaderClient.hpp)

target_link_libraries(openraveYarpPluginLoaderClient YARP::YARP_OS
                                                     YARP::YARP_init
                                                     ROBOTICSLAB::ColorDebug)

install(TARGETS openraveYarpPluginLoaderClient
        DESTINATION ${CMAKE_INSTALL_BINDIR})

endif()
