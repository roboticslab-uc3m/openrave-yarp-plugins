option(ENABLE_openraveYarpWorldClientFile "Enable/disable openraveYarpWorldClientFile program" ON)

if(ENABLE_openraveYarpWorldClientFile)

add_executable(openraveYarpWorldClientFile main.cpp
                                           OpenraveYarpWorldClientFile.cpp
                                           OpenraveYarpWorldClientFile.hpp)

target_link_libraries(openraveYarpWorldClientFile YARP::YARP_os
                                                  YARP::YARP_init
                                                  ROBOTICSLAB::ColorDebug)

target_compile_features(openraveYarpWorldClientFile PRIVATE cxx_std_11)

install(TARGETS openraveYarpWorldClientFile
        DESTINATION ${CMAKE_INSTALL_BINDIR})

endif()
