option(ENABLE_YarpOpenraveBase "Enable/disable YarpOpenraveBase" ON)

if(ENABLE_YarpOpenraveBase)

    add_library(YarpOpenraveBase OBJECT YarpOpenraveBase.hpp
                                        YarpOpenraveBase.cpp)

    set_target_properties(YarpOpenraveBase PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

    target_link_libraries(YarpOpenraveBase PUBLIC YARP::YARP_os
                                                  OpenRAVE::libopenrave-core
                                                  Boost::thread)

    target_include_directories(YarpOpenraveBase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

endif()
