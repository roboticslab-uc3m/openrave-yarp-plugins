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

    while 1:
	pass

except openrave_exception,e:
    print e
    RaveDestroy()

except KeyboardInterrupt:
    RaveDestroy()

