#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpForceEstimator'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/home/raul/repos/textiles/textiles/ironing/manipulation/ironingSim/ironingSim.env.xml')

    OpenraveYarpForceEstimator = RaveCreateModule(env,'OpenraveYarpForceEstimator')
    print OpenraveYarpForceEstimator.SendCommand('open')

    while 1:
        pass

finally:
    RaveDestroy()

