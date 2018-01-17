# Copyright: (C) 2016 Universidad Carlos III de Madrid
# Author: Juan G. Victores, Raul Fernandez-Fernandez
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Find YARP (common dependency for all devices).
find_package(YARP REQUIRED)

# Include YARP CMake modules (needless since YARP 2.3.70).
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

# Utilities for creating YARP devices.
include(YarpPlugin)

# Common YARP includes for subdirectories (needless since YARP 2.3.70).
include_directories(${YARP_INCLUDE_DIRS})

# YARP devices.
add_subdirectory(YarpOpenraveBase)
add_subdirectory(YarpOpenraveControlboard)
add_subdirectory(YarpOpenraveControlboardCollision)
add_subdirectory(YarpOpenraveGrabber)
add_subdirectory(YarpOpenraveRGBDSensor)
add_subdirectory(YarpOpenraveRobotManager)
