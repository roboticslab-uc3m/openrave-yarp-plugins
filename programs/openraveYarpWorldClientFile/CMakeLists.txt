option(ENABLE_openraveYarpWorldClientFile "Enable/disable openraveYarpWorldClientFile program" ON)

if(ENABLE_openraveYarpWorldClientFile)

        add_executable(openraveYarpWorldClientFile main.cpp
                                                   OpenraveYarpWorldClientFile.cpp
                                                   OpenraveYarpWorldClientFile.hpp)

        target_link_libraries(openraveYarpWorldClientFile YARP::YARP_os
                                                          YARP::YARP_init)

        install(TARGETS openraveYarpWorldClientFile)

endif()
