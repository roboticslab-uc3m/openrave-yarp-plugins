#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPluginLoader'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')

    # Force "ode" physics
    physics = RaveCreatePhysicsEngine(env,'ode')
    env.SetPhysicsEngine(physics)

    # "default" requires [roboticslab-uc3m/forceSensor](https://github.com/roboticslab-uc3m/forceSensor)
    # "teo" requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)
    example = "default"  # "default" or "teo"

    if example == "teo":
        env.Load('/usr/local/share/teo-openrave-models/openrave/teo/teo.robot.xml')
    else:
        env.Load('forceSensor/examples/physicsRobot.env.xml')

    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')

    if example == "teo":
        print OpenraveYarpPluginLoader.SendCommand('open --device multipleanalogsensorsserver --subdevice YarpOpenraveAnalogSensors --robotIndex 0 --ftSensorIndices 1 2 --period 50')
    else:
        print OpenraveYarpPluginLoader.SendCommand('open --device multipleanalogsensorsserver --subdevice YarpOpenraveAnalogSensors --robotIndex 0 --ftSensorIndices 0 --period 50')

    while 1:
        pass

finally:
    RaveDestroy()
