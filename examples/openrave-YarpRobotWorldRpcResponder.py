#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveWorldRpcResponder'):
        raveLogError("Plugin not correctly loaded")
        
    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml')

    OpenraveWorldRpcResponder = RaveCreateModule(env,'OpenraveWorldRpcResponder')
    print OpenraveWorldRpcResponder.SendCommand('open')

    while 1:
        pass

finally:
    RaveDestroy()

