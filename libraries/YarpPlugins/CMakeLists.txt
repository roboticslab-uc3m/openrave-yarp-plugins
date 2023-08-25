# Device interface target.
add_library(SimulationInterfaces INTERFACE)

# Configure usage requirements.
target_include_directories(SimulationInterfaces INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                                          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Register interface headers.
set_property(TARGET SimulationInterfaces PROPERTY PUBLIC_HEADER ISimulation.hpp)

# Register export set.
install(TARGETS SimulationInterfaces
        EXPORT ROBOTICSLAB_OPENRAVE_YARP_PLUGINS)

# Create alias target.
add_library(ROBOTICSLAB::SimulationInterfaces ALIAS SimulationInterfaces)

# YARP devices.
add_subdirectory(YarpOpenraveAnalogSensors)
add_subdirectory(YarpOpenraveControlBoard)
add_subdirectory(YarpOpenraveGrabber)
add_subdirectory(YarpOpenraveMeshFromRealDepth)
add_subdirectory(YarpOpenraveRGBDSensor)
add_subdirectory(YarpOpenraveRobotManager)
add_subdirectory(YarpOpenraveSimulation)
