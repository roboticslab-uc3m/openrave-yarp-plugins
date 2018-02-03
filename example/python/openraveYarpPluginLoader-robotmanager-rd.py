#!/usr/bin/env python

# Depends (update to `develop`):
# https://github.com/asrob-uc3m/robotDevastation-openrave-models  # mapping_room.env.xml
# https://github.com/asrob-uc3m/yarp-devices  # RobotServer

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPluginLoader'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/usr/local/share/robotDevastation-openrave-models/contexts/openrave/ecro/mapping_room.env.xml')

    OpenraveYarpPluginLoader1 = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpPluginLoader1.SendCommand('open --device RobotServer --subdevice YarpOpenraveRobotManager --robotIndex 0 --physics ode')

    OpenraveYarpPluginLoader2 = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpPluginLoader2.SendCommand('open --device grabber --subdevice YarpOpenraveGrabber --robotIndex 0 --sensorIndex 1 --name /ecroSim/img:o')

    OpenraveYarpPluginLoader3 = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpPluginLoader3.SendCommand('open --device RobotServer --subdevice YarpOpenraveRobotManager --robotIndex 1 --physics ode')

    OpenraveYarpPluginLoader4 = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpPluginLoader4.SendCommand('open --device grabber --subdevice YarpOpenraveGrabber --robotIndex 1 --sensorIndex 1 --name /ecroSim2/img:o')

    #-- Model has 3d sensor, but not used yet.
    #print OpenraveYarpPluginLoader.SendCommand('open --device RGBDSensorWrapper --subdevice YarpOpenraveRGBDSensor --depthSensorIndex 0 --rgbSensorIndex 1')

    while 1:
        pass

finally:
    RaveDestroy()

