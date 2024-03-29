option(ENABLE_OpenraveYarpPaintSquares "Enable/disable OpenraveYarpPaintSquares" ON)

if(ENABLE_OpenraveYarpPaintSquares)

    add_library(OpenraveYarpPaintSquares MODULE OpenraveYarpPaintSquares.cpp)

    target_link_libraries(OpenraveYarpPaintSquares YARP::YARP_os
                                                   YARP::YARP_init
                                                   Boost::boost)

    if(OpenRAVE_VERSION VERSION_GREATER_EQUAL 0.114)
        target_link_libraries(OpenraveYarpPaintSquares OpenRAVE::libopenrave)
    else()
        link_directories(${OpenRAVE_LIBRARY_DIRS})

        target_link_libraries(OpenraveYarpPaintSquares ${OpenRAVE_LIBRARIES})

        set_target_properties(OpenraveYarpPaintSquares PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                                  LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

        target_include_directories(OpenraveYarpPaintSquares PRIVATE ${OpenRAVE_INCLUDE_DIRS})
    endif()

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveYarpPaintSquares)

endif()
