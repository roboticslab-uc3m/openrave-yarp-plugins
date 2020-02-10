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

    options = yarp.Property()
    options.fromString(penvStr)
    options.put('device','YarpOpenraveControlboard')
    options.put('robotIndex',0)
    options.put('manipulatorIndex',0)
    dd = yarp.PolyDriver(options)

    pos = dd.viewIPositionControl()
    pos.positionMove(1,45)

    done = False
    while not done:
        print('wait to reach...')
        yarp.delay(1.0) # [s]
        done = pos.checkMotionDone()

finally:
    RaveDestroy()
