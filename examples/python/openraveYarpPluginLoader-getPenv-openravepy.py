#!/usr/bin/env python

from openravepy import *
import yarp

yarp.Network.init()
yarp.Network.setLocalMode(True)

try:
    # Create environment
    RaveInitialize()
    env = Environment()
    env.Load("data/testwamcamera.env.xml")

    # Create viewer
    env.SetViewer('qtcoin')

    # Create OpenraveYarpPluginLoader and obtain environment pointer (penv)
    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    penvStr = OpenraveYarpPluginLoader.SendCommand('getPenv')
    penvNameValueStr = '(penv {' + penvStr + '})'

    # ControlBoard using penv
    controlBoardOptions = yarp.Property()
    controlBoardOptions.fromString(penvNameValueStr) # put('penv',penvValue)
    controlBoardOptions.put('device','YarpOpenraveControlBoard')
    controlBoardOptions.put('robotIndex',0)
    controlBoardOptions.put('manipulatorIndex',0)
    controlBoardDevice = yarp.PolyDriver(controlBoardOptions)

    # Create Grabber using penv
    grabberOptions = yarp.Property()
    grabberOptions.fromString(penvNameValueStr) # put('penv',penvValue)
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

finally:
    RaveDestroy()
