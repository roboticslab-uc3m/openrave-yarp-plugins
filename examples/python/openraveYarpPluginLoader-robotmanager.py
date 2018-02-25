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

    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')

    print OpenraveYarpPluginLoader.SendCommand('open --device RobotServer --subdevice YarpOpenraveRobotManager --robotIndex 0 --physics ode --mode transform')

    print OpenraveYarpPluginLoader.SendCommand('open --device RobotServer --subdevice YarpOpenraveRobotManager --robotIndex 1 --physics ode --mode 4wd')

    while 1:
        pass

finally:
    RaveDestroy()

