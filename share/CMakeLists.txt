# Copyright: UC3M 2016
# Author: Juan G. Victores and Raul Fernandez-Fernandez
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
# 

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include(YarpInstallationHelpers)
yarp_configure_external_installation(openrave-yarp-plugins)

### Go through single applications

#add_subdirectory(prog)

