# Copyright: (C) 2017 Universidad Carlos III de Madrid
# Author: Juan G. Victores

option(ENABLE_YarpOpenraveBase "Enable/disable YarpOpenraveBase" ON)

if(ENABLE_YarpOpenraveBase)

add_library(YarpOpenraveBase OBJECT YarpOpenraveBase.hpp
                                    YarpOpenraveBase.cpp)

set_target_properties(YarpOpenraveBase PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                  POSITION_INDEPENDENT_CODE TRUE
                                                  COMPILE_FEATURES "$<TARGET_PROPERTY:YARP::YARP_OS,INTERFACE_COMPILE_FEATURES>")

target_include_directories(YarpOpenraveBase PUBLIC $<TARGET_PROPERTY:YARP::YARP_OS,INTERFACE_INCLUDE_DIRECTORIES>
                                                   ${OpenRAVE_INCLUDE_DIRS}
                                                   $<TARGET_PROPERTY:Boost::thread,INTERFACE_INCLUDE_DIRECTORIES>
                                                   ${CMAKE_CURRENT_SOURCE_DIR}
                                            PRIVATE $<TARGET_PROPERTY:ROBOTICSLAB::ColorDebug,INTERFACE_INCLUDE_DIRECTORIES>)

# Alternative interface target for ease of use by downstreams, see:
# https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/62#issuecomment-361017934
add_library(YarpOpenraveBaseInterface INTERFACE)
target_include_directories(YarpOpenraveBaseInterface INTERFACE $<TARGET_PROPERTY:YarpOpenraveBase,INTERFACE_INCLUDE_DIRECTORIES>)
set_target_properties(YarpOpenraveBaseInterface PROPERTIES INTERFACE_SOURCES $<TARGET_OBJECTS:YarpOpenraveBase>)

endif()
