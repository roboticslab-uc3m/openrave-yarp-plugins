#!/usr/bin/env python

import cv2 as cv
import numpy as np
import matplotlib.pylab

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
grabberControls = grabberDevice.viewIFrameGrabberControls()
grabber = grabberDevice.viewIFrameGrabberImage()

# do stuff
grabberControls.setFeature(yarp.YARP_FEATURE_ZOOM, 0.3)

npImg = np.zeros((grabber.height(), grabber.width(), 3), dtype = np.uint8)
yarpImg = yarp.ImageRgb()
yarpImg.resize(grabber.width(), grabber.height())
yarpImg.setExternal(npImg, npImg.shape[1], npImg.shape[0])
yarp.delay(0.5)

grabber.getImage(yarpImg) # refeshes npImg
#matplotlib.pylab.imshow(npImg,interpolation='none')
#matplotlib.pylab.show() # blocking

r,g,b = cv.split(npImg)
colorMinusColor = r-g
res, binary = cv.threshold(colorMinusColor,55,255,cv.THRESH_BINARY)
kernel = np.ones((2,2),np.uint8)
binary = cv.dilate(binary,kernel,iterations = 1)
binary = cv.erode(binary,kernel,iterations = 1)
contours = cv.findContours(binary, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)

contours = contours[1] # cv2
contoursSorted = sorted(contours, key=lambda x: cv.contourArea(x), reverse=True)
largestContour = contoursSorted[0]
moments = cv.moments(largestContour)

x = int(moments['m10']/moments['m00'])
y = int(moments['m01']/moments['m00'])
area = cv.contourArea(largestContour)
print('x',x,'y',y,'area',area)

cv.drawContours(npImg, contoursSorted, 0, (0,255,0), 3)
matplotlib.pylab.imshow(npImg,interpolation='none')
matplotlib.pylab.show() # blocking

pos.positionMove(1,45)

done = False
while not done:
    print('wait to reach...')
    yarp.delay(1.0) # [s]
    done = pos.checkMotionDone()
