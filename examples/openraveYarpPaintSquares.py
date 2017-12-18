#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPluginLoader'):
        raveLogError("Plugin not correctly loaded")

    if not RaveLoadPlugin('OpenraveYarpPaintSquares'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/usr/local/share/xgnitive/contexts/models/teo_cgda_iros.env.xml')  # assumes XGNITIVE installed

    OpenraveYarpControlboard = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpControlboard.SendCommand('open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 2 --genRefSpeed 999999')  # rightArm is maniplator 2

    OpenraveYarpPaintSquares = RaveCreateModule(env,'OpenraveYarpPaintSquares')
    print OpenraveYarpPaintSquares.SendCommand('open --squares 16')
    #print OpenraveYarpPaintSquares.SendCommand('open --name /altPortName')

    while 1:
        pass

finally:
    RaveDestroy()

