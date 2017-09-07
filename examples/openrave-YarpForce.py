#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpControlboard'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml')

    OpenraveYarpControlboard = RaveCreateModule(env,'OpenraveYarpControlboard')
    print OpenraveYarpControlboard.SendCommand('open')

    OpenraveWorldRpcResponder = RaveCreateModule(env,'OpenraveWorldRpcResponder')
    print OpenraveWorldRpcResponder.SendCommand('open')

    OpenraveYarpForce = RaveCreateModule(env,'OpenraveYarpForce')
    print OpenraveYarpForce.SendCommand('open')

    while 1:
        pass

finally:
    RaveDestroy()

