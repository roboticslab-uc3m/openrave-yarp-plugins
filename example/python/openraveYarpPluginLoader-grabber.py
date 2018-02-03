#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPluginLoader'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')

    example = "asibot"  # "asibot" or "ecro"

    if example == "asibot":
        env.Load('/usr/local/share/asibot/contexts/openrave/models/asibot_kitchen_cameras.env.xml')
    else:
        env.Load('/usr/local/share/robotDevastation-openrave-models/contexts/openrave/ecro/mapping_room.env.xml')

    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')

    if example == "asibot":
        print OpenraveYarpPluginLoader.SendCommand('open --device grabber --subdevice YarpOpenraveGrabber --robotIndex 1 --sensorIndex 0')
    else:
        print OpenraveYarpPluginLoader.SendCommand('open --device grabber --subdevice YarpOpenraveGrabber --robotIndex 0 --sensorIndex 1')

    while 1:
        pass

finally:
    RaveDestroy()

