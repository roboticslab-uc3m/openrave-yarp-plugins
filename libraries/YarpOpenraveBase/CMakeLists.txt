# Copyright: (C) 2017 Universidad Carlos III de Madrid
# Author: Juan G. Victores

option(ENABLE_YarpOpenraveBase "Enable/disable YarpOpenraveBase" ON)

if(ENABLE_YarpOpenraveBase)

add_library(YarpOpenraveBase OBJECT YarpOpenraveBase.hpp
                                    YarpOpenraveBase.cpp)

set_target_properties(YarpOpenraveBase PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}"
                                                  POSITION_INDEPENDENT_CODE TRUE)

target_link_libraries(YarpOpenraveBase PUBLIC YARP::YARP_OS
                                              ${OpenRAVE_CORE_LIBRARIES}
                                              Boost::thread
                                       PRIVATE ROBOTICSLAB::ColorDebug)

target_include_directories(YarpOpenraveBase PUBLIC ${OpenRAVE_INCLUDE_DIRS}
                                                   ${CMAKE_CURRENT_SOURCE_DIR})

endif()
