if(NOT TARGET ROBOTICSLAB::YarpCloudUtils AND (NOT DEFINED ENABLE_openraveYarpWorldClientMesh OR ENABLE_openraveYarpWorldClientMesh))
    message(WARNING "ROBOTICSLAB::YarpCloudUtils not found, disabling openraveYarpWorldClientMesh program")
endif()

cmake_dependent_option(ENABLE_openraveYarpWorldClientMesh "Enable/disable openraveYarpWorldClientMesh program" ON
                       "TARGET ROBOTICSLAB::YarpCloudUtils" OFF)

if(ENABLE_openraveYarpWorldClientMesh)

    add_executable(openraveYarpWorldClientMesh main.cpp
                                               OpenraveYarpWorldClientMesh.cpp
                                               OpenraveYarpWorldClientMesh.hpp)

    target_link_libraries(openraveYarpWorldClientMesh YARP::YARP_os
                                                      YARP::YARP_init
                                                      YARP::YARP_dev
                                                      YARP::YARP_sig
                                                      ROBOTICSLAB::YarpCloudUtils)

    if(TARGET ROBOTICSLAB::KinematicsDynamicsInterfaces)
        set_property(SOURCE OpenraveYarpWorldClientMesh.cpp PROPERTY COMPILE_DEFINITIONS HAVE_RL_KD)
        target_link_libraries(openraveYarpWorldClientMesh ROBOTICSLAB::KinematicsDynamicsInterfaces)
    endif()

    install(TARGETS openraveYarpWorldClientMesh
            DESTINATION ${CMAKE_INSTALL_BINDIR})

else()

    set(ENABLE_openraveYarpWorldClientMesh OFF CACHE BOOL "Enable/disable openraveYarpWorldClientMesh program" FORCE)

endif()
