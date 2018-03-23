# Copyright: (C) 2016 Universidad Carlos III de Madrid
# Author: Juan G. Victores, Raul Fernandez-Fernandez
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(YARP_VERSION_SHORT VERSION_LESS 2.3.70)
    include_directories(${YARP_INCLUDE_DIRS})
endif()

# YARP devices.
add_subdirectory(YarpOpenraveControlboard)
add_subdirectory(YarpOpenraveGrabber)
add_subdirectory(YarpOpenraveRGBDSensor)
add_subdirectory(YarpOpenraveRobotManager)
