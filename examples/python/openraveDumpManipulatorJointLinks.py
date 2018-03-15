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

    OpenraveDumpManipulatorJointLinks = RaveCreateModule(env,'OpenraveDumpManipulatorJointLinks')
    print OpenraveDumpManipulatorJointLinks.SendCommand('open')

    while 1:
        pass

finally:
    RaveDestroy()

