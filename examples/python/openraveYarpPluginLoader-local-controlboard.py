#!/usr/bin/env python

from openravepy import *
import yarp

try:
    RaveInitialize()
    env = Environment()
    env.Load("data/lab1.env.xml")
    env.SetViewer('qtcoin')

    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    penvStr = OpenraveYarpPluginLoader.SendCommand('getPenv')

    controlboardOptions = yarp.Property()
    penvNameValueStr = '(penv {' + penvStr + '})'
    controlboardOptions.fromString(penvNameValueStr)
    controlboardOptions.put('device','YarpOpenraveControlboard')
    controlboardOptions.put('robotIndex',0)
    controlboardOptions.put('manipulatorIndex',0)
    controlboardDevice = yarp.PolyDriver(controlboardOptions)

    pos = controlboardDevice.viewIPositionControl()
    pos.positionMove(1,45)

    done = False
    while not done:
        print('wait to reach...')
        yarp.delay(1.0) # [s]
        done = pos.checkMotionDone()

finally:
    RaveDestroy()
