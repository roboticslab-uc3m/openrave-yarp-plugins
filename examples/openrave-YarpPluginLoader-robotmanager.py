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

    OpenraveYarpPluginLoader1 = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpPluginLoader1.SendCommand('open --device RobotServer --subdevice YarpOpenraveRobotManager --robotIndex 0 --physics ode')

    OpenraveYarpPluginLoader2 = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpPluginLoader2.SendCommand('open --device RGBDSensorWrapper --subdevice YarpOpenraveRGBDSensor --robotIndex 0 --sensorIndex 0')

    while 1:
        pass

finally:
    RaveDestroy()

