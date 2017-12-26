#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPluginLoader'):
        raveLogError("Plugin not correctly loaded")
        
    if not RaveLoadPlugin('OpenraveYarpForceEstimator'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/home/raul/repos/textiles/textiles/ironing/manipulation/ironingSim/ironingSim.env.xml')

    OpenraveYarpPluginLoader0 = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpPluginLoader0.SendCommand('open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 2 --genRefSpeed 999999')  # rightArm is maniplator 2
    
    OpenraveYarpPluginLoader2 = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpPluginLoader2.SendCommand('open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 0 --genRefSpeed 999999')  # trunk is manipulator 0
    
    OpenraveYarpForceEstimator = RaveCreateModule(env,'OpenraveYarpForceEstimator')
    print OpenraveYarpForceEstimator.SendCommand('open')

    while 1:
        pass

finally:
    RaveDestroy()

