# Copyright: (C) 2013 Universidad Carlos III de Madrid
# Author: Juan G. Victores

yarp_prepare_plugin(YarpOpenraveControlboard
                    CATEGORY device
                    TYPE teo::YarpOpenraveControlboard
                    INCLUDE YarpOpenraveControlboard.hpp
                    WRAPPER controlboardwrapper2)

IF (NOT SKIP_YarpOpenraveControlboard)

SET(CMAKE_MODULE_PATH ${TEO_MODULE_PATH} ${CMAKE_MODULE_PATH})
FIND_PACKAGE(YARP REQUIRED)
FIND_PACKAGE(OpenRAVE REQUIRED)
FIND_PACKAGE(COLOR_DEBUG REQUIRED)
find_package(Boost COMPONENTS iostreams python thread system)

if( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
  add_definitions("-fno-strict-aliasing -Wall")
endif( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${OpenRAVE_INCLUDE_DIRS} ${COLOR_DEBUG_INCLUDE_DIRS})  # kdl name depends on version
if( Boost_INCLUDE_DIRS )
  include_directories(${Boost_INCLUDE_DIRS})
endif()

LINK_DIRECTORIES(${TEO_LINK_DIRS})

YARP_ADD_PLUGIN(YarpOpenraveControlboard YarpOpenraveControlboard.hpp DeviceDriverImpl.cpp IControlLimits2Impl.cpp IControlModeImpl.cpp IEncodersImpl.cpp IEncodersTimedImpl.cpp IPositionImpl.cpp IPosition2Impl.cpp IVelocityImpl.cpp IVelocity2Impl.cpp ITorqueImpl.cpp)
set_target_properties(${KEYWORD} PROPERTIES COMPILE_FLAGS "${OpenRAVE_CXXFLAGS}")
set_target_properties(${KEYWORD} PROPERTIES LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")
TARGET_LINK_LIBRARIES(YarpOpenraveControlboard ${OpenRAVE_LIBRARIES} ${OpenRAVE_CORE_LIBRARIES} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${YARP_LIBRARIES})

install(TARGETS YarpOpenraveControlboard DESTINATION lib)

# Exporting dependencies for TEOConfig.cmake quite manually for now... 
set(TEO_INCLUDE_DIRS ${TEO_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "appended libraries")
set(TEO_LIBRARIES ${TEO_LIBRARIES} TeoYarp YarpOpenraveControlboard CACHE INTERNAL "appended libraries")

ENDIF (NOT SKIP_YarpOpenraveControlboard)

