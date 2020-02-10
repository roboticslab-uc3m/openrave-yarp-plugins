#!/usr/bin/env python

import yarp

yarp.Network.init()
yarp.Network.setLocalMode(True)

# environment with YarpOpenraveControlboard and OpenraveYarpPluginLoader
controlboardOptions = yarp.Property()
controlboardOptions.put('env','data/testwamcamera.env.xml')
controlboardOptions.put('device','YarpOpenraveControlboard')
controlboardOptions.put('robotIndex',0)
controlboardOptions.put('manipulatorIndex',0)
controlboardOptions.put('orPlugin','OpenraveYarpPluginLoader')
controlboardOptions.put('view',1)
controlboardDevice = yarp.PolyDriver(controlboardOptions)

# connect to OpenraveYarpPluginLoader to obtain environment pointer
rpcClient = yarp.RpcClient()
rpcClient.open('/OpenraveYarpPluginLoader/rpc:c')
rpcClient.addOutput('/OpenraveYarpPluginLoader/rpc:s')
cmd = yarp.Bottle()
res = yarp.Bottle()
cmd.addString('getPenv')
rpcClient.write(cmd,res)
optionsBase = res.get(0).asDict() # yarp.Property()
print(optionsBase.toString())

# create more devices using the property that contains the environment pointer
grabberOptions = yarp.Property(optionsBase)
grabberOptions.put('device','YarpOpenraveGrabber')
grabberOptions.put('robotIndex',0)
grabberOptions.put('sensorIndex',0)
grabberDevice = yarp.PolyDriver(grabberOptions)

# view specific interfaces
pos = controlboardDevice.viewIPositionControl()
controls = grabberDevice.viewIFrameGrabberControls()
grabber = grabberDevice.viewIFrameGrabberImage()

# do stuff
controls.setFeature(yarp.YARP_FEATURE_ZOOM, 0.3)

image = yarp.ImageRgb()
grabber.getImage(image)

pos.positionMove(1,45)

done = False
while not done:
    print('wait to reach...')
    yarp.delay(1.0) # [s]
    done = pos.checkMotionDone()

