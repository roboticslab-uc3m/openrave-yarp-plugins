# Generated by Juan G Victores 2013
# Thanks Rosen Diankov for all OpenRAVE developments

option(ENABLE_OpenraveYarpPaintSquares "Enable/disable OpenraveYarpPaintSquares" ON)

if(ENABLE_OpenraveYarpPaintSquares)

include_directories(${OpenRAVE_INCLUDE_DIRS})

link_directories(${OpenRAVE_LIBRARY_DIRS})

add_library(OpenraveYarpPaintSquares MODULE OpenraveYarpPaintSquares.cpp)

set_target_properties(OpenraveYarpPaintSquares PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                          LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

target_link_libraries(OpenraveYarpPaintSquares YARP::YARP_os
                                               YARP::YARP_init
                                               ${OpenRAVE_LIBRARIES}
                                               Boost::boost)

include(InstallOpenravePlugin)

install_openrave_plugin(OpenraveYarpPaintSquares)

endif()
