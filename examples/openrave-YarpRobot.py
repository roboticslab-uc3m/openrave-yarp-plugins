#!/usr/bin/env python
import openravepy
from openravepy import *
RaveInitialize()

if not RaveLoadPlugin('./OpenraveYarpControlboard'):
    raveLogError("Plugin not correctly loaded")
   
try:
    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/home/raul/repos/teo-main/share/openrave/teo/teo.robot.xml')
    teo_robot = env.GetRobots()[0]

    YarpRobot = RaveCreateModule(env,'OpenraveYarpControlboard')

    print YarpRobot.SendCommand('open')
    while 1:
        pass

finally:
    RaveDestroy()

