#!/usr/bin/env python
import openravepy
from openravepy import *
RaveInitialize()

if not RaveLoadPlugin('./OpenraveYarpControlboard'):
    raveLogError("Plugin not correctly loaded")
   
try:
    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml')
    teo_robot = env.GetRobots()[0]

    OpenraveYarpControlboard = RaveCreateModule(env,'OpenraveYarpControlboard')
    print OpenraveYarpControlboard.SendCommand('open')
	
    #print 'Finished loading visualization'

    while 1:
        pass

finally:
    RaveDestroy()

