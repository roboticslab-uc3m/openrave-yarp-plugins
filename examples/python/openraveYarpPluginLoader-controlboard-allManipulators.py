#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPluginLoader'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')

    # "teo" requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)
    example = "default"  # "default" or "teo"

    if example == "teo":
        env.Load('/usr/local/share/teo-openrave-models/openrave/teo/teo.robot.xml')
    else:
        env.Load('data/lab1.env.xml')

    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpPluginLoader.SendCommand('open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --allRobots --allManipulators')

    while 1:
        pass

finally:
    RaveDestroy()

