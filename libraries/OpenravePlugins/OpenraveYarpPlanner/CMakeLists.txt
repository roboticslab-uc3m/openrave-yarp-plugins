option(ENABLE_OpenraveYarpPlanner "Enable/disable OpenraveYarpPlanner" ON)

if(ENABLE_OpenraveYarpPlanner)

    add_library(OpenraveYarpPlanner MODULE OpenraveYarpPlanner.cpp
                                           OypPortReader.cpp
                                           OpenraveYarpPlanner.hpp
                                           OypPortReader.hpp
                                           LogComponent.hpp
                                           LogComponent.cpp)

    target_link_libraries(OpenraveYarpPlanner YARP::YARP_os
                                              YARP::YARP_init
                                              YARP::YARP_dev
                                              Boost::boost)

    if(OpenRAVE_VERSION VERSION_GREATER_EQUAL 0.114)
        target_link_libraries(OpenraveYarpPlanner OpenRAVE::libopenrave)
    else()
        link_directories(${OpenRAVE_LIBRARY_DIRS})

        target_link_libraries(OpenraveYarpPlanner PUBLIC ${OpenRAVE_LIBRARIES})

        set_target_properties(OpenraveYarpPlanner PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                             LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

        target_include_directories(OpenraveYarpPlanner PRIVATE ${OpenRAVE_INCLUDE_DIRS})
    endif()

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveYarpPlanner)

endif()
