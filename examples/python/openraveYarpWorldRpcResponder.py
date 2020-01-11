#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpWorldRpcResponder'):
        raveLogError("Plugin not correctly loaded")
        
    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/usr/local/share/teo-openrave-models/openrave/teo/teo.robot.xml')

    OpenraveYarpWorldRpcResponder = RaveCreateModule(env,'OpenraveYarpWorldRpcResponder')
    print OpenraveYarpWorldRpcResponder.SendCommand('open')

    while 1:
        pass

finally:
    RaveDestroy()

