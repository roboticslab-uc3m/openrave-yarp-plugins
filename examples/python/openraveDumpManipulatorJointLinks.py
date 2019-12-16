#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveDumpManipulatorJointLinks'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')

    env.Load('data/lab1.env.xml')

    OpenraveDumpManipulatorJointLinks = RaveCreateModule(env,'OpenraveDumpManipulatorJointLinks')
    print OpenraveDumpManipulatorJointLinks.SendCommand('open')

    while 1:
        pass

finally:
    RaveDestroy()

