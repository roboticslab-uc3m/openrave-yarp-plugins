option(ENABLE_openraveYarpWorldClient "Enable/disable openraveYarpWorldClient program" ON)

if(ENABLE_openraveYarpWorldClient)

add_executable(openraveYarpWorldClient main.cpp
                                              OpenraveYarpWorldClient.cpp
                                              OpenraveYarpWorldClient.hpp)

target_link_libraries(openraveYarpWorldClient YARP::YARP_os
                                                     YARP::YARP_init
                                                     ROBOTICSLAB::ColorDebug)

target_compile_features(openraveYarpWorldClient PRIVATE cxx_std_11)

install(TARGETS openraveYarpWorldClient
        DESTINATION ${CMAKE_INSTALL_BINDIR})

endif()
