#!/usr/bin/env python

import openravepy
from openravepy import *

try:
    RaveInitialize()

    if not RaveLoadPlugin('OpenraveYarpPluginLoader'):
        raveLogError("Plugin not correctly loaded")

    env=Environment()
    env.SetViewer('qtcoin')
    env.Load('/usr/local/share/teo-openrave-models/openrave/teo/teo.robot.xml')

    OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')
    print OpenraveYarpPluginLoader.SendCommand('open --device controlBoard_nws_yarp --subdevice YarpOpenraveControlBoard --robotIndex 0 --manipulatorIndex 0 --collision')

    # Convex Decomposition
    teo_robot = env.GetRobots()[0]
    cdmodel = databases.convexdecomposition.ConvexDecompositionModel(teo_robot, padding=0.02)  # Else defaults to zero padding

    if not cdmodel.load():
        # If not already in the database. Generate:
        print 'ConvexDecomposition not found, generating...'
        cdmodel.generate(padding=0.02)  # Else defaults to 0.005 padding
        print 'ConvexDecomposition generated, saving...'
        cdmodel.save()
        print 'Finished saving'

    print 'Setting robot...'
    cdmodel.setrobot()
    print 'Finish setrobot'

    print 'Starting visualization of the convexdecomposition model'
    cdmodel.show()
    #print 'Finished loading visualization'

    while 1:
        pass

finally:
    RaveDestroy()

