#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPluginLoader'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')

    example = "default"  # "default" or "asibot" or "ecro"

    if example == "asibot":
        env.Load('/usr/local/share/asibot/contexts/openrave/models/asibot_kitchen_cameras.env.xml')
    elif example == "ecro":
        env.Load('/usr/local/share/robotDevastation-openrave-models/openrave/mapping_room.env.xml')
    else:
        env.Load('data/testwamcamera.env.xml')
        
    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')

    if example == "asibot":
        print OpenraveYarpPluginLoader.SendCommand('open --device grabberDual --subdevice YarpOpenraveGrabber --robotIndex 1 --sensorIndex 0')
    elif example == "ecro":
        print OpenraveYarpPluginLoader.SendCommand('open --device grabberDual --subdevice YarpOpenraveGrabber --robotIndex 0 --sensorIndex 1')
    else:
        print OpenraveYarpPluginLoader.SendCommand('open --device grabberDual --subdevice YarpOpenraveGrabber --robotIndex 0 --sensorIndex 0')

    while 1:
        pass

finally:
    RaveDestroy()

