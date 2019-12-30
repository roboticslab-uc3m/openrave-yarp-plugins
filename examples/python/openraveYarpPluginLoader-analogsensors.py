#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPluginLoader'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')

    # "default" requires [roboticslab-uc3m/forceSensor](https://github.com/roboticslab-uc3m/forceSensor)
    # "teo" requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)
    example = "default"  # "default" or "ecro"

    if example == "teo":
        env.Load('/usr/local/share/teo-openrave-models/openrave/teo/teo.robot.xml')
    else:
        env.Load('forceSensor/examples/physicsRobot.env.xml')

    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')

    if example == "teo":
        print OpenraveYarpPluginLoader.SendCommand('open --device multipleanalogsensorsserver --subdevice YarpOpenraveAnalogSensors --robotIndex 0 --sensorIndex 0 --period 50')
    else:
        print OpenraveYarpPluginLoader.SendCommand('open --device multipleanalogsensorsserver --subdevice YarpOpenraveAnalogSensors --robotIndex 0 --sensorIndex 1 --period 50')

    while 1:
        pass

finally:
    RaveDestroy()
