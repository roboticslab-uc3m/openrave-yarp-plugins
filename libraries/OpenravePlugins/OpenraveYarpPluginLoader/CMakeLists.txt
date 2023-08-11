option(ENABLE_OpenraveYarpPluginLoader "Enable/disable OpenraveYarpPluginLoader" ON)

if(ENABLE_OpenraveYarpPluginLoader)

    add_library(OpenraveYarpPluginLoader MODULE OpenraveYarpPluginLoader.cpp
                                                OyplPeriodicWrite.cpp
                                                OyplPortReader.cpp
                                                OpenraveYarpPluginLoader.hpp
                                                OyplPeriodicWrite.hpp
                                                OyplPortReader.hpp
                                                LogComponent.hpp
                                                LogComponent.cpp)

    target_link_libraries(OpenraveYarpPluginLoader YARP::YARP_os
                                                   YARP::YARP_init
                                                   YARP::YARP_dev
                                                   Boost::boost)

    if(OpenRAVE_VERSION VERSION_GREATER_EQUAL 0.114)
        target_link_libraries(OpenraveYarpPluginLoader OpenRAVE::libopenrave)
    else()
        link_directories(${OpenRAVE_LIBRARY_DIRS})

        target_link_libraries(OpenraveYarpPluginLoader PUBLIC ${OpenRAVE_LIBRARIES})

        set_target_properties(OpenraveYarpPluginLoader PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                                  LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

        target_include_directories(OpenraveYarpPluginLoader PRIVATE ${OpenRAVE_INCLUDE_DIRS})
    endif()

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveYarpPluginLoader)

endif()
