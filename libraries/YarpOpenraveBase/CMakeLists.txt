option(ENABLE_YarpOpenraveBase "Enable/disable YarpOpenraveBase" ON)

if(ENABLE_YarpOpenraveBase)

    add_library(YarpOpenraveBase OBJECT YarpOpenraveBase.hpp
                                        YarpOpenraveBase.cpp)

    set_target_properties(YarpOpenraveBase PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

    target_link_libraries(YarpOpenraveBase PUBLIC YARP::YARP_os
                                                  Boost::thread)

    target_include_directories(YarpOpenraveBase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    if(OpenRAVE_VERSION VERSION_GREATER_EQUAL 0.114)
        target_link_libraries(YarpOpenraveBase PUBLIC OpenRAVE::libopenrave-core)
    else()
        target_link_libraries(YarpOpenraveBase PUBLIC ${OpenRAVE_CORE_LIBRARIES})
        set_target_properties(YarpOpenraveBase PROPERTIES COMPILE_OPTIONS "${OpenRAVE_CXX_FLAGS}")
        target_include_directories(YarpOpenraveBase PUBLIC ${OpenRAVE_INCLUDE_DIRS})
    endif()

endif()
