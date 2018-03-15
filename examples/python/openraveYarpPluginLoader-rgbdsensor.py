#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPluginLoader'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')

    example = "default"  # "default" or "ecro"

    if example == "ecro":
        env.Load('/usr/local/share/robotDevastation-openrave-models/contexts/openrave/ecro/mapping_room.env.xml')
    else:
        env.Load('data/testwamcamera.env.xml')

    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')

    if example == "ecro":
        print OpenraveYarpPluginLoader.SendCommand('open --device RGBDSensorWrapper --subdevice YarpOpenraveRGBDSensor --robotIndex 0 --depthSensorIndex 0 --rgbSensorIndex 1')
    else:
        print OpenraveYarpPluginLoader.SendCommand('open --device RGBDSensorWrapper --subdevice YarpOpenraveRGBDSensor --robotIndex 0 --depthSensorIndex 1 --rgbSensorIndex 0')

    while 1:
        pass

finally:
    RaveDestroy()

