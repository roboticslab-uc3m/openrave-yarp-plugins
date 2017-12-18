#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpCoupled'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/home/yo/repos/textiles/textiles/ironing/manipulation/ironingSim/ironingSim.env.xml')

    OpenraveYarpCoupled = RaveCreateModule(env,'OpenraveYarpCoupled')
    print OpenraveYarpCoupled.SendCommand('open')

    while 1:
        pass

finally:
    RaveDestroy()

