#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveDumpManipulatorJointLinks'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')

    example = "default"  # "default" or "teo"

    if example == "teo":
        env.Load('/usr/local/share/teo-openrave-models/openrave/teo/teo.robot.xml')
    else:
        env.Load('data/lab1.env.xml')

    OpenraveDumpManipulatorJointLinks = RaveCreateModule(env,'OpenraveDumpManipulatorJointLinks')
    print OpenraveDumpManipulatorJointLinks.SendCommand('open')

    while 1:
        pass

finally:
    RaveDestroy()

