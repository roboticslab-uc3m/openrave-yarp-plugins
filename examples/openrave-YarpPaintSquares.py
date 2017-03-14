#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPaintSquares'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/home/raul/repos/xgnitive/share/models/teo_cgda_iros.env.xml')  #ugly way of loading the environment

    OpenraveYarpPaintSquares = RaveCreateModule(env,'OpenraveYarpPaintSquares')
    print OpenraveYarpPaintSquares.SendCommand('open')
    #print OpenraveYarpPaintSquares.SendCommand('open /altPortName')

    while 1:
        pass

finally:
    RaveDestroy()

