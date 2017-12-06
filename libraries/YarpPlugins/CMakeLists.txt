#Copyright: (C) 2016 Universidad Carlos III de Madrid
# Author: Juan G. Victores, Raul Fernandez-Fernandez
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

#IF (ENABLE_yarpplugins)

if( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
  add_definitions("-fno-strict-aliasing -Wall -fPIC")
endif( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_definitions("-fPIC")
endif ()

find_package(YARP REQUIRED)
include_directories(${YARP_INCLUDE_DIRS})
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include(YarpPlugin)

#  yarp_begin_plugin_library(yarpplugins)
    add_subdirectory(YarpOpenraveControlboard)
    add_subdirectory(YarpOpenraveControlboardCollision)
    add_subdirectory(FakeControlboard)
    add_subdirectory(YarpOpenraveGrabber)
    add_subdirectory(YarpOpenraveRGBDSensor)
#  yarp_end_plugin_library(yarpplugins)
  
#ENDIF (ENABLE_yarpplugins)

