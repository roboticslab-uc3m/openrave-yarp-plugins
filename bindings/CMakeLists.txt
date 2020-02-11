# Created by Juan G Victores 2013 @ UC3M
# Thanks to Paul Fitzpatrick for all the YARP and iCub SWIG stuff for inspiration

cmake_minimum_required(VERSION 2.8.11)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_MODULE_PATH})

find_package(YARP REQUIRED)
include_directories(${YARP_INCLUDE_DIRS})
link_libraries(${YARP_LIBRARIES})

find_package(ROBOTICSLAB_OPENRAVE_YARP_PLUGINS REQUIRED)
get_target_property(_simulation_interface_include_dirs ROBOTICSLAB::SimulationInterfaces INTERFACE_INCLUDE_DIRECTORIES)
include_directories(${_simulation_interface_include_dirs})

#MESSAGE(${CMAKE_SOURCE_DIR})
#include_directories(${CMAKE_SOURCE_DIR}/../src/libraries/ColorClientLib)

# for yarp.i
include_directories($ENV{YARP_ROOT}/bindings)

set(CREATE_PYTHON FALSE CACHE BOOL "Create Python interface")
set(CREATE_RUBY FALSE CACHE BOOL "Create Ruby interface")
set(CREATE_JAVA FALSE CACHE BOOL "Create Java interface")
SET(CREATE_CSHARP FALSE CACHE BOOL "Create CSharp interface")
SET(CREATE_LUA FALSE CACHE BOOL "Create Lua interface")

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
MESSAGE(STATUS "${SWIG_USE_FILE}")
SET (MIN_SWIG_VERSION "1.3.29")
IF ("${SWIG_VERSION}" VERSION_LESS "${MIN_SWIG_VERSION}")
  MESSAGE( FATAL_ERROR "Please update your SWIG. Current version: ${SWIG_VERSION}. The minimum version needed: ${MIN_SWIG_VERSION}")
ENDIF ("${SWIG_VERSION}" VERSION_LESS "${MIN_SWIG_VERSION}")

SET_SOURCE_FILES_PROPERTIES(roboticslab_openrave_yarp_plugins.i PROPERTIES CPLUSPLUS ON)

include(GNUInstallDirs)

if (CREATE_PYTHON)
  set(CMAKE_SWIG_OUTDIR "${CMAKE_BINARY_DIR}")
  SET(CMAKE_SWIG_FLAGS "-Wall;-module;roboticslab_openrave_yarp_plugins;-threads")
  SET(CREATE_PYTHON_VERSION "" CACHE STRING "Control python version used, if you wish." )
  FIND_PACKAGE(PythonLibs ${CREATE_PYTHON_VERSION})
  INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
  LINK_LIBRARIES(${PYTHON_LIBRARY})
  
  ## in Windows it seems necessary to declare explictly the link_directory
  if (WIN32)
    get_filename_component(PYTHON_DIR ${PYTHON_LIBRARY} PATH)
    link_directories(${PYTHON_DIR})
  endif()
  
  SWIG_ADD_MODULE(roboticslab_openrave_yarp_plugins python roboticslab_openrave_yarp_plugins.i)
  # uncomment if we want a debug version of the library
  #ADD_DEFINITIONS(-DPy_DEBUG)

  # installation path is determined reliably on most platforms using distutils
  FIND_PACKAGE(PythonInterp ${CREATE_PYTHON_VERSION})

  EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} 
    -c "from distutils import sysconfig; print(sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_PREFIX}'))" 
    OUTPUT_VARIABLE PYTHON_INSTDIR OUTPUT_STRIP_TRAILING_WHITESPACE )
  INSTALL(FILES ${CMAKE_BINARY_DIR}/roboticslab_openrave_yarp_plugins.py ${CMAKE_BINARY_DIR}/_roboticslab_openrave_yarp_plugins.so 
    DESTINATION ${PYTHON_INSTDIR} )

  IF (WIN32)
    # Check if we have a target called _roboticslab_openrave_yarp_plugins as SWIG_ADD_MODULE
    # will currently give. If not, we're probably in the distant
    # future, and we'd best not meddle.
    GET_TARGET_PROPERTY(roboticslab_openrave_yarp_plugins_lib_location _roboticslab_openrave_yarp_plugins LOCATION)
    IF (roboticslab_openrave_yarp_plugins_lib_location)
      SET_TARGET_PROPERTIES(_roboticslab_openrave_yarp_plugins PROPERTIES SUFFIX ".pyd")
    ENDIF ()    
  ENDIF (WIN32)
endif (CREATE_PYTHON)

if (CREATE_RUBY)
  find_package(Ruby REQUIRED)
  set(CMAKE_SWIG_FLAGS "-Wall;-module;roboticslab_openrave_yarp_plugins")
  include_directories(${RUBY_INCLUDE_PATH})
  swig_add_module(roboticslab_openrave_yarp_plugins ruby roboticslab_openrave_yarp_plugins.i)
  swig_link_libraries(roboticslab_openrave_yarp_plugins ${RUBY_LIBRARY})
  set_target_properties(roboticslab_openrave_yarp_plugins PROPERTIES PREFIX "")
endif (CREATE_RUBY)

if (CREATE_JAVA)
  OPTION(PREPARE_CLASS_FILES "Prepare class files" OFF)
  SET(ROBOT_DEVASTATION_ROBOTS_HELPERS LoadAsrobYarpDevices.java)
  MAKE_DIRECTORY("${CMAKE_CURRENT_BINARY_DIR}/roboticslab_openrave_yarp_plugins")
  SET(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/roboticslab_openrave_yarp_plugins")
  SET(TARGET_DIR src)
  IF (NOT BINDINGS_SRC_DIR)
    SET(BINDINGS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
  ENDIF ()
  IF (NOT PREPARE_CLASS_FILES)
    SET(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/generated_src")
    SET(TARGET_DIR generated_src)
  ENDIF ()
  MAKE_DIRECTORY("${CMAKE_CURRENT_BINARY_DIR}/${TARGET_DIR}")
  FOREACH(src ${ROBOT_DEVASTATION_ROBOTS_HELPERS})
    SET(target ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_DIR}/${src})
    CONFIGURE_FILE(${BINDINGS_SRC_DIR}/${src}
      ${target} IMMEDIATE)
    MESSAGE(STATUS "copied ${target}")
  ENDFOREACH(src)
  SET(CMAKE_SWIG_FLAGS "-package;roboticslab_openrave_yarp_plugins;-Wall;-module;roboticslab_openrave_yarp_plugins")
  FIND_PACKAGE(JNI REQUIRED)
  INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} )
  SET(target_name jroboticslab_openrave_yarp_plugins)
  SWIG_ADD_MODULE(${target_name} java roboticslab_openrave_yarp_plugins.i)
  IF (PREPARE_CLASS_FILES)
    ADD_CUSTOM_COMMAND(TARGET ${target_name}
      POST_BUILD
      COMMAND javac -source 1.3 -target 1.3 "src/*.java" "roboticslab_openrave_yarp_plugins/*.java"
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  ENDIF ()

  # issue on MINGW where C++ name-mangling prevents java finding methods
  IF (MINGW)
    MESSAGE(STATUS "untested MINGW patch - see CMakeLists.txt")
    SET_TARGET_PROPERTIES(${target_name} PROPERTIES 
      LINK_FLAGS "--add-stdcall-alias")
  ENDIF (MINGW)
  ## the following is based on:
  ## http://www.cmake.org/pipermail/cmake/2007-September/016343.html
  IF(APPLE)
    SET_TARGET_PROPERTIES(${target_name} PROPERTIES SUFFIX ".jnilib")
  ENDIF(APPLE)
endif (CREATE_JAVA)

IF (CREATE_CSHARP)
   SET(target_name roboticslab_openrave_yarp_plugins)
   SET(CMAKE_SWIG_FLAGS "-Wall;-module;roboticslab_openrave_yarp_plugins")
   SWIG_ADD_MODULE(roboticslab_openrave_yarp_plugins csharp roboticslab_openrave_yarp_plugins.i)
ENDIF (CREATE_CSHARP)

if(CREATE_LUA)
	set(CMAKE_SWIG_FLAGS "-Wall;-module;roboticslab_openrave_yarp_plugins")
	find_package(Lua REQUIRED)
	include_directories(SYSTEM ${LUA_INCLUDE_DIR})
	swig_add_module(roboticslab_openrave_yarp_plugins lua roboticslab_openrave_yarp_plugins.i)
	swig_link_libraries(roboticslab_openrave_yarp_plugins ${LUA_LIBRARY})
	set(_CMAKE_INSTALL_LUADIR "${CMAKE_INSTALL_LIBDIR}/lua/${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}")
	set(CMAKE_INSTALL_LUADIR ${_CMAKE_INSTALL_LUADIR} CACHE PATH "lua bindings (${_CMAKE_INSTALL_LUADIR})")
	mark_as_advanced(CMAKE_INSTALL_LUADIR)

	if(NOT IS_ABSOLUTE ${CMAKE_INSTALL_LUADIR})
		set(CMAKE_INSTALL_FULL_LUADIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LUADIR}")
	else()
		set(CMAKE_INSTALL_FULL_LUADIR "${CMAKE_INSTALL_LUADIR}")
	endif()

	set_target_properties(${SWIG_MODULE_roboticslab_openrave_yarp_plugins_REAL_NAME} PROPERTIES PREFIX ""
                                                        					             OUTPUT_NAME "roboticslab_openrave_yarp_plugins"
                                                        					             LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LUADIR})

	# Update RPATH
	if(NOT CMAKE_SKIP_RPATH AND NOT CMAKE_SKIP_INSTALL_RPATH)
		file(RELATIVE_PATH _rel_path "${CMAKE_INSTALL_FULL_LUADIR}" "${CMAKE_INSTALL_FULL_LIBDIR}")
		get_target_property(_current_rpath "${SWIG_MODULE_roboticslab_openrave_yarp_plugins_REAL_NAME}" INSTALL_RPATH)
    list(APPEND _current_rpath "\$ORIGIN/${_rel_path}")
		set_target_properties(${SWIG_MODULE_roboticslab_openrave_yarp_plugins_REAL_NAME} PROPERTIES INSTALL_RPATH "${_current_rpath}")
	endif()

	install(TARGETS ${SWIG_MODULE_roboticslab_openrave_yarp_plugins_REAL_NAME}
		      DESTINATION ${CMAKE_INSTALL_LUADIR})
endif(CREATE_LUA)
