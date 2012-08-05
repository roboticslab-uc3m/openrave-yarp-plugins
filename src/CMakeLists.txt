SET(LIB_NAME ForceSensor)

# OpenRave
include(${CMAKE_MODULE_PATH}/FindOpenRAVE.cmake)
if( NOT OPENRAVE_FOUND )
  message(FATAL_ERROR "could not find openrave, check for openrave-config in path")
endif( NOT OPENRAVE_FOUND )

if( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
  add_definitions("-fno-strict-aliasing -Wall")
endif( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )

if( MSVC )
  # have to include the module definition file
  set(PLUGIN_LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/openraveplugin.def\"")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(PLUGIN_LINK_FLAGS "")
endif()

include_directories(${OPENRAVE_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} . ../../ ../include)
link_directories(${OPENRAVE_LINK_DIRS})

add_library(${LIB_NAME} SHARED  ForceSensorMain.cpp
								ForceSensor.cpp
							   )

set_target_properties(${LIB_NAME} PROPERTIES COMPILE_FLAGS "${OPENRAVE_CXXFLAGS}")
set_target_properties(${LIB_NAME} PROPERTIES LINK_FLAGS "${OPENRAVE_LINK_FLAGS} ${PLUGIN_LINK_FLAGS}")

if( MSVC )
	target_link_libraries(${LIB_NAME} ${OPENRAVE_LIBRARY} libxml2)
else()
	target_link_libraries(${LIB_NAME} ${OPENRAVE_LIBRARY})
endif()

if( NOT DEFINED ENV{PLUGIN_INSTALL_DIR} )
    set(PLUGIN_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins")
else( NOT DEFINED ENV{PLUGIN_INSTALL_DIR} )
    set(PLUGIN_INSTALL_DIR "$ENV{PLUGIN_INSTALL_DIR}")
endif( NOT DEFINED ENV{PLUGIN_INSTALL_DIR} )

install(TARGETS ${LIB_NAME} DESTINATION ${PLUGIN_INSTALL_DIR} )
