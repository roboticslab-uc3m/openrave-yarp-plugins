#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpControlboard'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/home/raul/repos/textiles/textiles/ironing/manipulation/ironingSim/ironingSim.env.xml')

    OpenraveYarpControlboard = RaveCreateModule(env,'OpenraveYarpControlboard')
    print OpenraveYarpControlboard.SendCommand('open')

    OpenraveWorldRpcResponder = RaveCreateModule(env,'OpenraveWorldRpcResponder')
    print OpenraveWorldRpcResponder.SendCommand('open')

    OpenraveYarpForceEstimator = RaveCreateModule(env,'OpenraveYarpForceEstimator')
    print OpenraveYarpForceEstimator.SendCommand('open')

    while 1:
        pass

finally:
    RaveDestroy()

