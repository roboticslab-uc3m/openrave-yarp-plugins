#!/usr/bin/env python

import yarp

yarp.Network.init()
yarp.Network.setLocalMode(True)

# Create (ControlBoard + environment + OpenraveYarpPluginLoader + viewer)
controlBoardOptions = yarp.Property()
controlBoardOptions.put('device','YarpOpenraveControlBoard')
controlBoardOptions.put('robotIndex',0)
controlBoardOptions.put('manipulatorIndex',0)
controlBoardOptions.put('env','data/testwamcamera.env.xml')
controlBoardOptions.put('orPlugin','OpenraveYarpPluginLoader')
controlBoardOptions.put('view',1)
controlBoardDevice = yarp.PolyDriver(controlBoardOptions)

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

# View specific interfaces
pos = controlBoardDevice.viewIPositionControl()
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
