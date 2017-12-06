# Copyright: (C) 2013 Universidad Carlos III de Madrid
# Author: Juan G. Victores

yarp_prepare_plugin(YarpOpenraveRGBDSensor
                    CATEGORY device
                    TYPE roboticslab::YarpOpenraveRGBDSensor
                    INCLUDE YarpOpenraveRGBDSensor.hpp
                    WRAPPER grabber)

IF (NOT SKIP_YarpOpenraveRGBDSensor)

SET(CMAKE_MODULE_PATH ${TEO_MODULE_PATH} ${CMAKE_MODULE_PATH})
FIND_PACKAGE(YARP 2.3.70 REQUIRED)
FIND_PACKAGE(OpenRAVE REQUIRED)
find_package(Boost COMPONENTS iostreams python thread system)

if( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
  add_definitions("-fno-strict-aliasing -Wall")
endif( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${OpenRAVE_INCLUDE_DIRS})  # kdl name depends on version
if( Boost_INCLUDE_DIRS )
  include_directories(${Boost_INCLUDE_DIRS})
endif()

LINK_DIRECTORIES(${OPENRAVE_YARP_PLUGINS_LINK_DIRS})

YARP_ADD_PLUGIN(YarpOpenraveRGBDSensor YarpOpenraveRGBDSensor.hpp DeviceDriverImpl.cpp IRGBDSensorImpl.cpp)
add_dependencies(YarpOpenraveRGBDSensor COLOR_DEBUG)
set_target_properties(${KEYWORD} PROPERTIES COMPILE_FLAGS "${OpenRAVE_CXXFLAGS}")
set_target_properties(${KEYWORD} PROPERTIES LINK_FLAGS "${OpenRAVE_LINK_FLAGS}")
TARGET_LINK_LIBRARIES(YarpOpenraveRGBDSensor ${OpenRAVE_LIBRARIES} ${OpenRAVE_CORE_LIBRARIES} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${YARP_LIBRARIES})

# Exporting dependencies for TEOConfig.cmake quite manually for now... 
set(TEO_INCLUDE_DIRS ${TEO_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "appended libraries")
set(TEO_LIBRARIES ${TEO_LIBRARIES} TeoYarp YarpOpenraveRGBDSensor CACHE INTERNAL "appended libraries")

  #install(TARGETS YarpOpenraveRGBDSensor DESTINATION lib)
  yarp_install(TARGETS YarpOpenraveRGBDSensor
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
             ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

  yarp_install(FILES YarpOpenraveRGBDSensor.ini
              COMPONENT runtime
              DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

ENDIF (NOT SKIP_YarpOpenraveRGBDSensor)

