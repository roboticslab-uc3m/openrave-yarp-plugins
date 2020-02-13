#!/usr/bin/env python

import roboticslab_openrave_yarp_plugins
import yarp

yarp.Network.init()
yarp.Network.setLocalMode(True)

# Create (Simulation + environment + viewer)
simulationOptions = yarp.Property()
simulationOptions.put('env','data/testwamcamera.env.xml')
simulationOptions.put('device','YarpOpenraveSimulation')
simulationOptions.put('robotIndex',0) # required, dummy
simulationOptions.put('view',1)
simulationDevice = yarp.PolyDriver(simulationOptions)

simulation = roboticslab_openrave_yarp_plugins.viewISimulation(simulationDevice) # syntax is different
penvValue = yarp.Value()
simulation.getSimulationRawPointerValue(penvValue)

# Create Controlboard + using penv
controlboardOptions = yarp.Property()
controlboardOptions.put('penv',penvValue)
controlboardOptions.put('device','YarpOpenraveControlboard')
controlboardOptions.put('robotIndex',0)
controlboardOptions.put('manipulatorIndex',0)
controlboardDevice = yarp.PolyDriver(controlboardOptions)

# Create Grabber using penv
grabberOptions = yarp.Property()
grabberOptions.put('penv',penvValue)
grabberOptions.put('device','YarpOpenraveGrabber')
grabberOptions.put('robotIndex',0)
grabberOptions.put('sensorIndex',0)
grabberDevice = yarp.PolyDriver(grabberOptions)

# View specific interfaces
pos = controlboardDevice.viewIPositionControl()
controls = grabberDevice.viewIFrameGrabberControls()
grabber = grabberDevice.viewIFrameGrabberImage()

# do stuff
simulation.stop()
simulation.step(0.1)
simulation.start(0.1)

controls.setFeature(yarp.YARP_FEATURE_ZOOM, 0.3)

image = yarp.ImageRgb()
grabber.getImage(image)

pos.positionMove(1,45)

done = False
while not done:
    print('wait to reach...')
    yarp.delay(1.0) # [s]
    done = pos.checkMotionDone()
