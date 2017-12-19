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

    #OpenraveYarpControlboard = RaveCreateModule(env,'OpenraveYarpControlboard')
    #print OpenraveYarpControlboard.SendCommand('open')

    #OpenraveWorldRpcResponder = RaveCreateModule(env,'OpenraveWorldRpcResponder')
    #print OpenraveWorldRpcResponder.SendCommand('open')

    #OpenraveYarpForceEstimator = RaveCreateModule(env,'OpenraveYarpForceEstimator')
    #print OpenraveYarpForceEstimator.SendCommand('open')

    #New config:

    OpenraveYarpControlboard = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpControlboard.SendCommand('open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 2 --genRefSpeed 999999')  # rightArm is maniplator 2
    print OpenraveYarpControlboard.SendCommand('open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 0 --genRefSpeed 999999')  # trunk is manipulator 0
    
    OpenraveYarpForceEstimator = RaveCreateModule(env,'OpenraveYarpForceEstimator')
    print OpenraveYarpForceEstimator.SendCommand('open')

    while 1:
        pass

finally:
    RaveDestroy()

