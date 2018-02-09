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
    print OpenraveYarpPluginLoader.SendCommand('open --device RGBDSensorWrapper --subdevice YarpOpenraveRGBDSensor --robotIndex 0 --depthSensorIndex 0 --rgbSensorIndex 1')

    while 1:
        pass

finally:
    RaveDestroy()
