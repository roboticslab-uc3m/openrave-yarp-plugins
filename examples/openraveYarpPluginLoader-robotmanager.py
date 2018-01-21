#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPluginLoader'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/usr/local/share/robotDevastation-openrave-models/contexts/openrave/ecro/mapping_room.env.xml')

    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')

    print OpenraveYarpPluginLoader.SendCommand('open --device RobotServer --subdevice YarpOpenraveRobotManager --robotIndex 0 --physics ode')

    print OpenraveYarpPluginLoader.SendCommand('open --device grabber --subdevice YarpOpenraveGrabber --robotIndex 0 --sensorIndex 1 --name /ecroSim/img:o')

    #-- Model has 3d sensor, but not used yet.
    #print OpenraveYarpPluginLoader.SendCommand('open --device RGBDSensorWrapper --subdevice YarpOpenraveRGBDSensor --depthSensorIndex 0 --rgbSensorIndex 1')

    while 1:
        pass

finally:
    RaveDestroy()

