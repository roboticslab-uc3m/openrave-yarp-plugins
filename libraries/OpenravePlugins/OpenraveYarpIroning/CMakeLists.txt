option(ENABLE_OpenraveYarpIroning "Enable/disable OpenraveYarpIroning" ON)

if(ENABLE_OpenraveYarpIroning)

    add_library(OpenraveYarpIroning MODULE OpenraveYarpIroning.cpp)

    target_link_libraries(OpenraveYarpIroning YARP::YARP_os
                                                   YARP::YARP_init
                                                   Boost::boost)

    if(OpenRAVE_VERSION VERSION_GREATER_EQUAL 0.114)
        target_link_libraries(OpenraveYarpIroning OpenRAVE::libopenrave)
    else()
        link_directories(${OpenRAVE_LIBRARY_DIRS})

        target_link_libraries(OpenraveYarpIroning PUBLIC ${OpenRAVE_LIBRARIES})

        set_target_properties(OpenraveYarpIroning PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                                  LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

        target_include_directories(OpenraveYarpIroning PRIVATE ${OpenRAVE_INCLUDE_DIRS})
    endif()

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveYarpIroning)

endif()
