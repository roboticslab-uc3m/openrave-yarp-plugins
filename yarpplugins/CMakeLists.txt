#Copyright: (C) 2016 Universidad Carlos III de Madrid
# Author: Juan G. Victores, Raul Fernandez-Fernandez
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF (ENABLE_yarpplugins)

  option(ENABLE_launchyarpplugins "Choose if you want to compile launchTeoYarp" TRUE)

if( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )
  add_definitions("-fno-strict-aliasing -Wall -fPIC")
endif( CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX )

  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
  include(YarpPlugin)


  yarp_begin_plugin_library(yarpplugins)
    add_subdirectory(YarpOpenraveControlboard)
  yarp_end_plugin_library(yarpplugins)
  
  IF (ENABLE_launchyarpplugins)

    link_directories(${TEO_LINK_DIRS})
    
    yarp_add_plugin_yarpdev_executable(launchyarpplugins yarpplugins)
    install(TARGETS launchyarpplugins DESTINATION bin)

  ENDIF (ENABLE_launchyarpplugins)

ENDIF (ENABLE_yarpplugins)

