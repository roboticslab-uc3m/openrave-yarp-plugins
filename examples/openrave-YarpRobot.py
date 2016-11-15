#!/usr/bin/env python
from openravepy import *
RaveInitialize()

if not RaveLoadPlugin('./OpenraveYarpControlboard'):
    raveLogError("Plugin not correctly loaded")
    
try:
    env=Environment()
    env.Load('robots/barrettwam.robot.xml')
    YarpRobot = RaveCreateModule(env,'OpenraveYarpControlboard')
    print YarpRobot.SendCommand('open')
    while 1:
        pass
finally:
    RaveDestroy()
