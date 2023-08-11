option(ENABLE_OpenraveYarpWorld "Enable/disable OpenraveYarpWorld" ON)

if(ENABLE_OpenraveYarpWorld)

    add_library(OpenraveYarpWorld MODULE OpenraveYarpWorld.cpp
                                         OywPeriodicWrite.cpp
                                         OywPortReader.cpp
                                         OpenraveYarpWorld.hpp
                                         OywPeriodicWrite.hpp
                                         OywPortReader.hpp
                                         LogComponent.hpp
                                         LogComponent.cpp)


    target_link_libraries(OpenraveYarpWorld YARP::YARP_os
                                            YARP::YARP_init
                                            Boost::boost)

    if(OpenRAVE_VERSION VERSION_GREATER_EQUAL 0.114)
        target_link_libraries(OpenraveYarpWorld OpenRAVE::libopenrave)
    else()
        link_directories(${OpenRAVE_LIBRARY_DIRS})

        target_link_libraries(OpenraveYarpWorld PUBLIC ${OpenRAVE_LIBRARIES})

        set_target_properties(OpenraveYarpWorld PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                           LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

        target_include_directories(OpenraveYarpWorld PRIVATE ${OpenRAVE_INCLUDE_DIRS})
    endif()

    include(InstallOpenravePlugin)
    install_openrave_plugin(OpenraveYarpWorld)

endif()
