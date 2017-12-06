# Copyright: Universidad Carlos III de Madrid (C) 2016;
# Authors: Juan G. Victores and Raul Fernandez-Fernandez
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

# Exploit new cmake 2.6 features (export). 

cmake_minimum_required(VERSION 2.8.9)

# Safety check against missing dummy library for system-available COLOR_DEBUG.
# Remove if cmake_minimum_required() above is set to 3.0 or later.
cmake_policy(SET CMP0046 NEW)

project(OPENRAVE_YARP_PLUGINS)

set(BUILD_SHARED_LIBS TRUE CACHE INTERNAL "hide this!")

### options: cpp libraries (yarp plugins)
option(ENABLE_FakeControlboard "Enable/disable compilation of FakeControlboard" TRUE)
option(ENABLE_YarpOpenraveControlboard "Enable/disable option YarpOpenraveControlboard" TRUE)
option(ENABLE_YarpOpenraveControlboardCollision "Enable/disable option YarpOpenraveControlboardCollision" TRUE)
option(ENABLE_YarpOpenraveGrabber "Enable/disable option YarpOpenraveGrabber" TRUE)
option(ENABLE_YarpOpenraveRGBDSensor  "Enable/disable option YarpOpenraveRGBDSensor" TRUE)

### options: cpp libraries (openrave plugins)
option(ENABLE_OpenraveYarpControlboard "Enable/disable option OpenraveYarpControlboard" TRUE)
option(ENABLE_OpenraveYarpPaintSquares "Enable/disable option OpenraveYarpPaintSquares" TRUE)
option(ENABLE_OpenraveWorldRpcResponder "Enable/disable option OpenraveWorldRpcResponder" TRUE)
option(ENABLE_OpenraveYarpForce "Enable/disable option OpenraveYarpForce" TRUE)
option(ENABLE_OpenraveYarpForceEstimator "Enable/disable option OpenraveYarpForceEstimator" TRUE)
option(ENABLE_OpenraveYarpPluginLoader "Enable/disable option OpenraveYarpPluginLoader" TRUE)

### options: cpp programs
option(ENABLE_teoSim "Choose if you want to compile (deprecated) teoSim" TRUE)


if(MSVC)
  MESSAGE(STATUS "Running on windows")    
  set(CMAKE_DEBUG_POSTFIX "d")
endif(MSVC)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING 
    "Choose the type of build, recommanded options are: Debug or Release")
endif(NOT CMAKE_BUILD_TYPE)
# Hide variable to MSVC users, since it is not needed
if (MSVC)
   mark_as_advanced(CMAKE_BUILD_TYPE)
endif(MSVC)

######################

include(GNUInstallDirs)

### this makes everything go in $OPENRAVE_YARP_PLUGINS_DIR/lib and $OPENRAVE_YARP_PLUGINS_DIR/bin
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
message(STATUS "Libraries go to ${LIBRARY_OUTPUT_PATH}")
message(STATUS "Executables go to ${EXECUTABLE_OUTPUT_PATH}")
# this doesn't happen automatically for makefiles
make_directory(${LIBRARY_OUTPUT_PATH})
make_directory(${EXECUTABLE_OUTPUT_PATH})
# and let us clean their contents on a "make clean"
##set_directory_properties(PROPERTIES LIBRARY_OUTPUT_PATH ADDITIONAL_MAKE_CLEAN_FILES)
##set_directory_properties(PROPERTIES EXECUTABLE_OUTPUT_PATH ADDITIONAL_MAKE_CLEAN_FILES)
mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
mark_as_advanced(CMAKE_BACKWARDS_COMPATIBILITY)
##########################################

# Pick up our cmake modules - they are all in the cmake subdirectory
set(OPENRAVE_YARP_PLUGINS_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
# let cmake use them
list(APPEND CMAKE_MODULE_PATH ${OPENRAVE_YARP_PLUGINS_MODULE_PATH})

# Bootstrap YCM
# https://github.com/robotology/ycm/issues/118
set(YCM_TAG v0.2.2)
include(YCMBootstrap)

# Superbuild phase
include(FindOrBuildPackage)

find_or_build_package(COLOR_DEBUG)

# find_or_build_package() doesn't bring into scope COLOR_DEBUG_INCLUDE_DIRS
# nor other config variables. If a system copy is found, we have to call the
# find_package() command and proceed as usual.
if(USE_SYSTEM_COLOR_DEBUG)
    find_package(COLOR_DEBUG REQUIRED)
    include_directories(${COLOR_DEBUG_INCLUDE_DIRS})
endif()

set(OPENRAVE_YARP_PLUGINS_INCLUDE_DIRS CACHE INTERNAL "appended header dirs" FORCE)
set(OPENRAVE_YARP_PLUGINS_LINK_DIRS CACHE INTERNAL "appended link dirs" FORCE)
set(OPENRAVE_YARP_PLUGINS_LIBRARIES CACHE INTERNAL "appended libraries" FORCE)

# add main contents
add_subdirectory(share)
add_subdirectory(libraries)
add_subdirectory(programs)

# export our variables to a OPENRAVE_YARP_PLUGINSConfig.cmake creation
set(OPENRAVE_YARP_PLUGINS_LINK_DIRS ${OPENRAVE_YARP_PLUGINS_LINK_DIRS} ${LIBRARY_OUTPUT_PATH})
configure_file(${CMAKE_SOURCE_DIR}/cmake/template/OPENRAVE_YARP_PLUGINSConfig.cmake.in
               ${CMAKE_BINARY_DIR}/OPENRAVE_YARP_PLUGINSConfig.cmake @ONLY)

# create a symbolic link to the shared directory (as contains models and may be heavy)
# If you want a deep copy, you can use the copy_directory command instead of create_symlink.
#add_custom_target(install_applications COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/share ${CMAKE_SOURCE_DIR}/../share)

#endif(MSVC)
##add_custom_target(nuke
##  "${CMAKE_COMMAND}" -E "remove_directory" "${CMAKE_SOURCE_DIR}/bin" ";"
##  "${CMAKE_COMMAND}" -E "remove_directory" "${CMAKE_SOURCE_DIR}/lib")

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/template/OPENRAVE_YARP_PLUGINSConfigUninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/OPENRAVE_YARP_PLUGINSConfigUninstall.cmake" IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/OPENRAVE_YARP_PLUGINSConfigUninstall.cmake)

