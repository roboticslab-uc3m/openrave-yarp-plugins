#!/usr/bin/env python

import roboticslab_openrave_yarp_plugins
import yarp

yarp.Network.init()
yarp.Network.setLocalMode(True)

# Create (Controlboard + environment + OpenraveYarpPluginLoader + viewer)
controlboardOptions = yarp.Property()
controlboardOptions.put('device','YarpOpenraveControlboard')
controlboardOptions.put('robotIndex',0)
controlboardOptions.put('manipulatorIndex',0)
controlboardOptions.put('env','data/testwamcamera.env.xml')
controlboardOptions.put('orPlugin','OpenraveYarpPluginLoader')
controlboardOptions.put('view',1)
controlboardDevice = yarp.PolyDriver(controlboardOptions)

# Connect to OpenraveYarpPluginLoader and obtain environment pointer (penv)
rpcClient = yarp.RpcClient()
rpcClient.open('/OpenraveYarpPluginLoader/rpc:c')
rpcClient.addOutput('/OpenraveYarpPluginLoader/rpc:s')
cmd = yarp.Bottle()
res = yarp.Bottle()
cmd.addString('getPenv')
rpcClient.write(cmd,res)
penvValue = res.get(0) # penvValue.isBlob()
print(penvValue.toString())

# Create Grabber using penv
grabberOptions = yarp.Property()
grabberOptions.put('penv',penvValue)
grabberOptions.put('device','YarpOpenraveGrabber')
grabberOptions.put('robotIndex',0)
grabberOptions.put('sensorIndex',0)
grabberDevice = yarp.PolyDriver(grabberOptions)

# Create Simulation using penv
simulationOptions = yarp.Property()
simulationOptions.put('penv',penvValue)
simulationOptions.put('device','YarpOpenraveSimulation')
simulationOptions.put('robotIndex',0) # required, dummy
simulationDevice = yarp.PolyDriver(simulationOptions)

# View specific interfaces
pos = controlboardDevice.viewIPositionControl()
controls = grabberDevice.viewIFrameGrabberControls()
grabber = grabberDevice.viewIFrameGrabberImage()
simulation = roboticslab_openrave_yarp_plugins.viewISimulation(simulationDevice) # syntax is different

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
