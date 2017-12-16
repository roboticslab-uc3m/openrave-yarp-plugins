# Copyright: (C) 2017 Universidad Carlos III de Madrid
# Author: Juan G. Victores

if(ENABLE_YarpOpenraveBase)

set(CMAKE_MODULE_PATH ${TEO_MODULE_PATH} ${CMAKE_MODULE_PATH})
find_package(YARP REQUIRED)
find_package(OpenRAVE REQUIRED)
find_package(Boost COMPONENTS iostreams python thread system)

if( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
  add_definitions("-fno-strict-aliasing -Wall")
endif( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${OpenRAVE_INCLUDE_DIRS})
if( Boost_INCLUDE_DIRS )
  include_directories(${Boost_INCLUDE_DIRS})
endif()

link_directories(${OPENRAVE_YARP_PLUGINS_LINK_DIRS})

add_library(YarpOpenraveBase YarpOpenraveBase.hpp YarpOpenraveBase.cpp)
add_dependencies(YarpOpenraveBase COLOR_DEBUG)
set_target_properties(${KEYWORD} PROPERTIES COMPILE_FLAGS "${OpenRAVE_CXXFLAGS}")
set_target_properties(${KEYWORD} PROPERTIES LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")
target_link_libraries(YarpOpenraveBase ${OpenRAVE_LIBRARIES} ${OpenRAVE_CORE_LIBRARIES} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${YARP_LIBRARIES})
install(TARGETS YarpOpenraveBase DESTINATION lib )

# Exporting dependencies for TEOConfig.cmake quite manually for now... 
set(OPENRAVE_YARP_PLUGINS_INCLUDE_DIRS ${OPENRAVE_YARP_PLUGINS_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "appended header dirs")
#set(OPENRAVE_YARP_PLUGINS_LIBRARIES ${OPENRAVE_YARP_PLUGINS_LIBRARIES} YarpOpenraveBase CACHE INTERNAL "appended libraries")

endif()