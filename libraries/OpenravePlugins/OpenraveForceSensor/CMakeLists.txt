
option(ENABLE_OpenraveForceSensor "Enable/disable OpenraveForceSensor" ON)

if(ENABLE_OpenraveForceSensor)

include_directories(${OpenRAVE_INCLUDE_DIRS})

link_directories(${OpenRAVE_LIBRARY_DIRS})

add_library(OpenraveForceSensor MODULE ForceSensor.cpp
                                       ForceSensorMain.cpp
                                       ForceSensor.h)

set_target_properties(OpenraveForceSensor PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                          LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

target_link_libraries(OpenraveForceSensor YARP::YARP_os
                                          YARP::YARP_init
                                          ${OpenRAVE_LIBRARIES}
                                          Boost::boost)

include(InstallOpenravePlugin)

install_openrave_plugin(OpenraveForceSensor)

endif()
