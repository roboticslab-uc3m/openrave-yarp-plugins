#!/usr/bin/env python

# Authors: see AUTHORS.md at project root.
# CopyPolicy: released under the terms of the LGPLv2.1, see LICENSE at project root.
# URL: https://github.com/roboticslab-uc3m/openrave-yarp-plugins

# Dependencies:
# - https://github.com/asrob-uc3m/robotDevastation-openrave-models (provides `mapping_room.env.xml`)

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
        env.Load('/usr/local/share/robotDevastation-openrave-models/openrave/mapping_room.env.xml')
    else:
        env.Load('data/testwamcamera.env.xml')

    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')

    if example == "ecro":
        print OpenraveYarpPluginLoader.SendCommand('open --device RGBDSensorWrapper --subdevice YarpOpenraveRGBDSensor --robotIndex 0 --depthSensorIndex 0 --rgbSensorIndex 1')
    else:
        print OpenraveYarpPluginLoader.SendCommand('open --device RGBDSensorWrapper --subdevice YarpOpenraveRGBDSensor --robotIndex 0 --depthSensorIndex 3 --rgbSensorIndex 0')

    while 1:
        pass

finally:
    RaveDestroy()
