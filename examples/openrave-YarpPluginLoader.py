#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPluginLoader'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml')

    OpenraveYarpControlboard = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpControlboard.SendCommand('open --key1 value1 --key2 value2')

    while 1:
        pass

finally:
    RaveDestroy()

