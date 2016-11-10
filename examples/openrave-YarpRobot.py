#!/usr/bin/env python
from openravepy import *
RaveInitialize()
RaveLoadPlugin('./YarpRobot')
try:
    env=Environment()
    env.Load('robots/barrettwam.robot.xml')
    YarpRobot = RaveCreateModule(env,'YarpRobot')
    print YarpRobot.SendCommand('open')
    while 1:
        pass
finally:
    RaveDestroy()
