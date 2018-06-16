# Copyright: (C) 2017 Universidad Carlos III de Madrid
# Author: Juan G. Victores

option(ENABLE_YarpOpenraveBase "Enable/disable YarpOpenraveBase" ON)

if(ENABLE_YarpOpenraveBase)

add_library(YarpOpenraveBase OBJECT YarpOpenraveBase.hpp
                                    YarpOpenraveBase.cpp)

set(_compile_options "${OpenRAVE_CXX_FLAGS}")

# Transitive headers.
get_target_property(_color_debug_include_dirs ROBOTICSLAB::ColorDebug INTERFACE_INCLUDE_DIRECTORIES)

# C++11 features.
get_target_property(_yarp_os_compile_features YARP::YARP_OS INTERFACE_COMPILE_FEATURES)
get_target_property(_yarp_os_compile_options YARP::YARP_OS INTERFACE_COMPILE_OPTIONS)

if(NOT CMAKE_VERSION VERSION_LESS 3.1 AND _yarp_os_compile_features)
    set_target_properties(YarpOpenraveBase PROPERTIES COMPILE_FEATURES "${_yarp_os_compile_features}")
elseif(_yarp_os_compile_options)
    set(_compile_options "${_compile_options};${_yarp_os_compile_options}")
endif()

set_target_properties(YarpOpenraveBase PROPERTIES COMPILE_OPTIONS "${_compile_options}"
                                                  POSITION_INDEPENDENT_CODE TRUE)

target_include_directories(YarpOpenraveBase PUBLIC ${YARP_INCLUDE_DIRS}
                                                   ${OpenRAVE_INCLUDE_DIRS}
                                                   ${Boost_INCLUDE_DIRS}
                                                   ${CMAKE_CURRENT_SOURCE_DIR}
                                            PRIVATE ${_color_debug_include_dirs})

endif()
