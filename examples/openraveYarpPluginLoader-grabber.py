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
    print OpenraveYarpPluginLoader.SendCommand('open --device YarpOpenraveGrabber --robotIndex 0 --sensorIndex 1 --prefix /drl')

    while 1:
        pass

finally:
    RaveDestroy()
