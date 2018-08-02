# Copyright: (C) 2017 Universidad Carlos III de Madrid
# Author: Juan G. Victores

option(ENABLE_YarpOpenraveBase "Enable/disable YarpOpenraveBase" ON)

if(ENABLE_YarpOpenraveBase)

add_library(YarpOpenraveBase OBJECT YarpOpenraveBase.hpp
                                    YarpOpenraveBase.cpp)

# Transitive headers.
get_target_property(_color_debug_include_dirs ROBOTICSLAB::ColorDebug INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(_yarp_os_include_dirs YARP::YARP_OS INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(_boost_include_dirs Boost::thread INTERFACE_INCLUDE_DIRECTORIES)

# C++11 features.
get_target_property(_yarp_os_compile_features YARP::YARP_OS INTERFACE_COMPILE_FEATURES)

set_target_properties(YarpOpenraveBase PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                  POSITION_INDEPENDENT_CODE TRUE)

if(_yarp_os_compile_features)
    # non-empty since YARP 2.3.72
    set_target_properties(YarpOpenraveBase PROPERTIES COMPILE_FEATURES "${_yarp_os_compile_features}")
endif()

target_include_directories(YarpOpenraveBase PUBLIC ${_yarp_os_include_dirs}
                                                   ${OpenRAVE_INCLUDE_DIRS}
                                                   ${_boost_include_dirs}
                                                   ${CMAKE_CURRENT_SOURCE_DIR}
                                            PRIVATE ${_color_debug_include_dirs})

get_target_property(_yorb_include_dirs YarpOpenraveBase INTERFACE_INCLUDE_DIRECTORIES)

# Alternative interface target for ease of use by downstreams, see:
# https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/62#issuecomment-361017934
add_library(YarpOpenraveBaseInterface INTERFACE)
target_include_directories(YarpOpenraveBaseInterface INTERFACE ${_yorb_include_dirs})
set_target_properties(YarpOpenraveBaseInterface PROPERTIES INTERFACE_SOURCES $<TARGET_OBJECTS:YarpOpenraveBase>)

endif()
