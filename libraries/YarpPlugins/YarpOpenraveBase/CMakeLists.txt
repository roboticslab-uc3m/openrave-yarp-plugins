# Copyright: (C) 2017 Universidad Carlos III de Madrid
# Author: Juan G. Victores

option(ENABLE_YarpOpenraveBase "Enable/disable YarpOpenraveBase" ON)

if(ENABLE_YarpOpenraveBase)

find_package(YARP REQUIRED)
find_package(OpenRAVE REQUIRED)
find_package(Boost ${OpenRAVE_Boost_VERSION} EXACT REQUIRED COMPONENTS iostreams python thread system)

link_directories(${OpenRAVE_LIBRARY_DIRS})

add_library(YarpOpenraveBase YarpOpenraveBase.hpp
                             YarpOpenraveBase.cpp)

add_dependencies(YarpOpenraveBase COLOR_DEBUG)

set_target_properties(YarpOpenraveBase PROPERTIES PUBLIC_HEADER YarpOpenraveBase.hpp
                                                  COMPILE_FLAGS "${OpenRAVE_CXX_FLAGS}"
                                                  LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")

target_link_libraries(YarpOpenraveBase YARP::YARP_OS
                                       YARP::YARP_dev
                                       ${OpenRAVE_CORE_LIBRARIES}
                                       ${Boost_THREAD_LIBRARY}
                                       ${Boost_SYSTEM_LIBRARY})

target_include_directories(YarpOpenraveBase PUBLIC ${YARP_INCLUDE_DIRS}
                                                   ${OpenRAVE_INCLUDE_DIRS}
                                                   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                                   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(Boost_INCLUDE_DIRS)
    target_include_directories(YarpOpenraveBase PUBLIC ${Boost_INCLUDE_DIRS})
endif()

install(TARGETS YarpOpenraveBase
        EXPORT ROBOTICSLAB_OPENRAVE_YARP_PLUGINS
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set_property(GLOBAL APPEND PROPERTY ROBOTICSLAB_OPENRAVE_YARP_PLUGINS_TARGETS YarpOpenraveBase)

endif()
