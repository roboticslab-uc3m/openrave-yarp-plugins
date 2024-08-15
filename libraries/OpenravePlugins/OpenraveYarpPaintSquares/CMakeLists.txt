option(ENABLE_OpenraveYarpPaintSquares "Enable/disable OpenraveYarpPaintSquares" ON)

if(ENABLE_OpenraveYarpPaintSquares)

    add_library(OpenraveYarpPaintSquares MODULE OpenraveYarpPaintSquares.cpp)

    target_link_libraries(OpenraveYarpPaintSquares YARP::YARP_os
                                                   YARP::YARP_init
                                                   OpenRAVE::libopenrave
                                                   Boost::boost)

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveYarpPaintSquares)

endif()
