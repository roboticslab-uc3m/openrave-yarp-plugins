# Copyright: Universidad Carlos III de Madrid (C) 2016;
# Authors: Juan G. Victores and Raul Fernandez-Fernandez
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(ROBOTICSLAB_OPENRAVE_YARP_PLUGINS LANGUAGES CXX)

# C/C++ compile flags.
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-fno-strict-aliasing -Wall)
endif()

# Add suffix for debug libraries.
if(MSVC)
    message(STATUS "Running on windows")
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

# Let the user specify a configuration (only single-config generators).
if(NOT CMAKE_CONFIGURATION_TYPES)
    # Possible values.
    set(_configurations Debug Release MinSizeRel RelWithDebInfo)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${_configurations})

    foreach(_conf ${_configurations})
        set(_conf_string "${_conf_string} ${_conf}")
    endforeach()

    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING
                 "Choose the type of build, options are:${_conf_string}")

    if(NOT CMAKE_BUILD_TYPE)
        # Encourage the user to specify build type.
        message(STATUS "Setting build type to 'Release' as none was specified.")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE Release)
    endif()
endif()

# Pick up our cmake modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake
                              ${CMAKE_SOURCE_DIR}/cmake/modules)

# Hard dependencies.
find_package(YCM 0.8 REQUIRED)
find_package(COLOR_DEBUG REQUIRED)
find_package(OpenRAVE 0.9 REQUIRED)
find_package(Boost "${OpenRAVE_Boost_VERSION}" REQUIRED COMPONENTS thread)
find_package(YARP 3.0 QUIET)
# https://github.com/roboticslab-uc3m/questions-and-answers/issues/65
if(NOT YARP_FOUND)
    find_package(YARP 2.3.70 REQUIRED)
endif()

# Soft dependencies.
find_package(ASROB_YARP_DEVICES QUIET)

# Always build YARP devices as MODULE libraries.
set(YARP_FORCE_DYNAMIC_PLUGINS TRUE CACHE INTERNAL "Force dynamic plugins")

# Configure installation paths for YARP resources.
yarp_configure_external_installation(roboticslab-openrave-yarp-plugins WITH_PLUGINS)

# Standard installation directories.
include(GNUInstallDirs)

# Control where libraries and executables are placed during the build.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

# Enabling coverage.
option(ENABLE_coverage "Choose if you want to enable coverage collection" OFF)

if(ENABLE_coverage)
    # List supported compilers.
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif()
endif()

# Create targets if specific requirements are satisfied.
include(CMakeDependentOption)

# Add main contents.
add_subdirectory(cmake)
add_subdirectory(libraries)
#add_subdirectory(share)

# Store the package in the user registry.
export(PACKAGE ROBOTICSLAB_OPENRAVE_YARP_PLUGINS)

# Retrieve global properties.
get_property(_exported_targets GLOBAL PROPERTY ROBOTICSLAB_OPENRAVE_YARP_PLUGINS_TARGETS)

# CMake installation path.
if(WIN32)
    set(_cmake_destination cmake)
else()
    set(_cmake_destination ${CMAKE_INSTALL_LIBDIR}/cmake/ROBOTICSLAB_OPENRAVE_YARP_PLUGINS)
endif()

# Create and install config files.
include(CMakePackageConfigHelpers)

# Set exported variables (build tree).
set(ROBOTICSLAB_OPENRAVE_YARP_PLUGINS_MODULE_DIR ${CMAKE_SOURCE_DIR}/cmake)

# <pkg>Config.cmake (build tree).
configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/templates/ROBOTICSLAB_OPENRAVE_YARP_PLUGINSConfig.cmake.in
                              ${CMAKE_BINARY_DIR}/ROBOTICSLAB_OPENRAVE_YARP_PLUGINSConfig.cmake
                              INSTALL_DESTINATION ${CMAKE_BINARY_DIR}
                              INSTALL_PREFIX ${CMAKE_BINARY_DIR}
                              PATH_VARS ROBOTICSLAB_OPENRAVE_YARP_PLUGINS_MODULE_DIR
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO)

# Set exported variables (build tree).
set(ROBOTICSLAB_OPENRAVE_YARP_PLUGINS_MODULE_DIR ${CMAKE_INSTALL_DATADIR}/ROBOTICSLAB_OPENRAVE_YARP_PLUGINS/cmake)

# <pkg>Config.cmake (install tree).
configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/templates/ROBOTICSLAB_OPENRAVE_YARP_PLUGINSConfig.cmake.in
                              ${CMAKE_BINARY_DIR}/ROBOTICSLAB_OPENRAVE_YARP_PLUGINSConfig.cmake.install
                              INSTALL_DESTINATION ${_cmake_destination}
                              PATH_VARS ROBOTICSLAB_OPENRAVE_YARP_PLUGINS_MODULE_DIR
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO)

# Install <pkg>Config.cmake.
install(FILES ${CMAKE_BINARY_DIR}/ROBOTICSLAB_OPENRAVE_YARP_PLUGINSConfig.cmake.install
        RENAME ROBOTICSLAB_OPENRAVE_YARP_PLUGINSConfig.cmake
        DESTINATION ${_cmake_destination})

# Export library targets if enabled.
# https://github.com/roboticslab-uc3m/project-generator/issues/19
if(_exported_targets)
    # <pkg>Targets.cmake (build tree).
    export(EXPORT ROBOTICSLAB_OPENRAVE_YARP_PLUGINS
           NAMESPACE ROBOTICSLAB::
           FILE ROBOTICSLAB_OPENRAVE_YARP_PLUGINSTargets.cmake)

    # <pkg>Targets.cmake (install tree).
    install(EXPORT ROBOTICSLAB_OPENRAVE_YARP_PLUGINS
            DESTINATION ${_cmake_destination}
            NAMESPACE ROBOTICSLAB::
            FILE ROBOTICSLAB_OPENRAVE_YARP_PLUGINSTargets.cmake)
endif()

# Configure and create uninstall target.
include(AddUninstallTarget)
