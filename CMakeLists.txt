# Copyright: Universidad Carlos III de Madrid (C) 2016;
# Authors: Juan G. Victores and Raul Fernandez-Fernandez
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

# Exploit new cmake 2.6 features (export). 

# reduce warning level with cmake 2.6
cmake_minimum_required(VERSION 2.6)
#cmake policies
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(OPENRAVE_YARP_PLUGINS)

### options: cpp libraries
option(ENABLE_FakeControlboard "Enable/disable compilation of FakeControlboard" TRUE)

### options: cpp programs
option(ENABLE_teoSim "Choose if you want to compile (deprecated) teoSim" TRUE)

### options: cpp plugins
option(ENABLE_openraveplugins "Enable/disable option openraveplugins" TRUE)
option(ENABLE_yarpplugins "Enable/disable option yarpplugins" TRUE)

### options: force default
option(ENABLE_yarpplugins_YarpOpenraveControlboard "Enable/disable option yarpplugins_YarpOpenraveControlboard" TRUE)
option(ENABLE_yarpplugins_YarpOpenravePaintSquares "Enable/disable option yarpplugins_YarpOpenravePaintSquares" TRUE)
option(ENABLE_yarpplugins_YarpOpenraveControlboardCollision "Enable/disable option yarpplugins_YarpOpenraveControlboardCollision" TRUE)
option(ENABLE_yarpplugins_FakeControlboard "Enable/disable option yarpplugins_FakeControlboard" TRUE)

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
include(YCMBootstrap)

# Superbuild phase
include(FindOrBuildPackage)

find_or_build_package(COLOR_DEBUG)

set(OPENRAVE_YARP_PLUGINS_INCLUDE_DIRS CACHE INTERNAL "appended header dirs" FORCE)
set(OPENRAVE_YARP_PLUGINS_LINK_DIRS CACHE INTERNAL "appended link dirs" FORCE)
set(OPENRAVE_YARP_PLUGINS_LIBRARIES CACHE INTERNAL "appended libraries" FORCE)

# add main contents
add_subdirectory(share)
#add_subdirectory(libraries)
add_subdirectory(programs)
add_subdirectory(yarpplugins)
add_subdirectory(openraveplugins)


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

