
option(ENABLE_OpenraveForceSensor "Enable/disable OpenraveForceSensor" ON)

if(ENABLE_OpenraveForceSensor)

    add_library(OpenraveForceSensor MODULE ForceSensor.cpp
                                           ForceSensorMain.cpp
                                           ForceSensor.h)

    target_link_libraries(OpenraveForceSensor YARP::YARP_os
                                              YARP::YARP_init
                                              Boost::boost)

    if(OpenRAVE_VERSION VERSION_GREATER_EQUAL 0.114)
        target_link_libraries(OpenraveForceSensor OpenRAVE::libopenrave)
    else()
        link_directories(${OpenRAVE_LIBRARY_DIRS})

        target_link_libraries(OpenraveForceSensor ${OpenRAVE_LIBRARIES})

        set_target_properties(OpenraveForceSensor PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                             LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

        target_include_directories(OpenraveForceSensor PRIVATE ${OpenRAVE_INCLUDE_DIRS})
    endif()

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveForceSensor)

endif()
