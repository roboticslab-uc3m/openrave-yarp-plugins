option(ENABLE_openraveYarpPluginLoaderClient "Enable/disable openraveYarpPluginLoaderClient program" ON)

if(ENABLE_openraveYarpPluginLoaderClient)

add_executable(openraveYarpPluginLoaderClient main.cpp
                                              OpenraveYarpPluginLoaderClient.cpp
                                              OpenraveYarpPluginLoaderClient.hpp)

target_link_libraries(openraveYarpPluginLoaderClient YARP::YARP_os
                                                     YARP::YARP_init)

install(TARGETS openraveYarpPluginLoaderClient
        DESTINATION ${CMAKE_INSTALL_BINDIR})

endif()
