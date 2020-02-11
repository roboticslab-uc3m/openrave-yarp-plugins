#!/usr/bin/env python

import cv2
import numpy as np
import matplotlib.pylab

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

# View specific interfaces
pos = controlboardDevice.viewIPositionControl()
controls = grabberDevice.viewIFrameGrabberControls()
grabber = grabberDevice.viewIFrameGrabberImage()

# do stuff
controls.setFeature(yarp.YARP_FEATURE_ZOOM, 0.3)

npImg = np.zeros((grabber.height(), grabber.width(), 3), dtype = np.uint8)
yarpImg = yarp.ImageRgb()
yarpImg.resize(grabber.width(), grabber.height())
yarpImg.setExternal(npImg, npImg.shape[1], npImg.shape[0])

grabber.getImage(yarpImg)
matplotlib.pylab.imshow(npImg,interpolation='none')
matplotlib.pylab.show()

pos.positionMove(1,45)

done = False
while not done:
    print('wait to reach...')
    yarp.delay(1.0) # [s]
    done = pos.checkMotionDone()
