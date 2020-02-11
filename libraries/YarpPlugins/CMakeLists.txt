# Copyright: (C) 2016 Universidad Carlos III de Madrid
# Author: Juan G. Victores, Raul Fernandez-Fernandez
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Device interface target.
add_library(SimulationInterfaces INTERFACE)

# Configure usage requirements.
target_include_directories(SimulationInterfaces INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Install interface headers.
install(FILES ISimulation.hpp
              DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Register export set.
install(TARGETS SimulationInterfaces
        EXPORT ROBOTICSLAB_OPENRAVE_YARP_PLUGINS)

# YARP devices.
add_subdirectory(YarpOpenraveAnalogSensors)
add_subdirectory(YarpOpenraveControlboard)
add_subdirectory(YarpOpenraveGrabber)
add_subdirectory(YarpOpenraveRGBDSensor)
add_subdirectory(YarpOpenraveRobotManager)
add_subdirectory(YarpOpenraveSimulation)
