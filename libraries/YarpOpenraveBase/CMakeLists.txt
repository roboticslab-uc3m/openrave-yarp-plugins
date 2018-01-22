# Copyright: (C) 2017 Universidad Carlos III de Madrid
# Author: Juan G. Victores

option(ENABLE_YarpOpenraveBase "Enable/disable YarpOpenraveBase" ON)

if(ENABLE_YarpOpenraveBase)

find_package(YARP REQUIRED)
find_package(OpenRAVE REQUIRED)
find_package(Boost ${OpenRAVE_Boost_VERSION} EXACT REQUIRED COMPONENTS iostreams python thread system)

add_library(YarpOpenraveBase OBJECT YarpOpenraveBase.hpp
                                    YarpOpenraveBase.cpp)

add_dependencies(YarpOpenraveBase COLOR_DEBUG)

set(_compile_options "${OpenRAVE_CXX_FLAGS}")

# C++11 features.
if(NOT CMAKE_VERSION VERSION_LESS 3.1)
    get_target_property(_yarp_os_compile_features YARP::YARP_OS INTERFACE_COMPILE_FEATURES)

    if(_yarp_os_compile_features)
        set_target_properties(YarpOpenraveBase PROPERTIES COMPILE_FEATURES "${_yarp_os_compile_features}")
    endif()
else()
    get_target_property(_yarp_os_compile_options YARP::YARP_OS INTERFACE_COMPILE_OPTIONS)

    if(_yarp_os_compile_options)
        set(_compile_options "${_compile_options};${_yarp_os_compile_options}")
    endif()
endif()

set_target_properties(YarpOpenraveBase PROPERTIES COMPILE_OPTIONS "${_compile_options}"
                                                  POSITION_INDEPENDENT_CODE TRUE)

target_include_directories(YarpOpenraveBase PUBLIC ${YARP_INCLUDE_DIRS}
                                                   ${OpenRAVE_INCLUDE_DIRS}
                                                   ${Boost_INCLUDE_DIRS}
                                                   ${CMAKE_CURRENT_SOURCE_DIR})

endif()
