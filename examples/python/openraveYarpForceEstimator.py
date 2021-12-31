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
    #env.Load('/home/raul/repos/textiles/textiles/ironing/manipulation/ironingSim/ironingSim.env.xml')
    env.Load('/usr/local/share/xgnitive/contexts/models/teo_cgda_iros_iron.env.xml')  # assumes XGNITIVE installed

    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')

    print OpenraveYarpPluginLoader.SendCommand('open --device controlBoard_nws_yarp --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 2 --genRefSpeed 999999')  # rightArm is maniplator 2

    print OpenraveYarpPluginLoader.SendCommand('open --device controlBoard_nws_yarp --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 0 --genRefSpeed 999999')  # trunk is manipulator 0

    OpenraveYarpForceEstimator = RaveCreateModule(env,'OpenraveYarpForceEstimator')
    print OpenraveYarpForceEstimator.SendCommand('open --wrinkleSize 4')

    while 1:
        pass

finally:
    RaveDestroy()

