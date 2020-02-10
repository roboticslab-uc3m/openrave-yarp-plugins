#!/usr/bin/env python

import yarp

controlboardOptions = yarp.Property()
controlboardOptions.put('env','data/testwamcamera.env.xml')
controlboardOptions.put('view',1)
controlboardOptions.put('device','YarpOpenraveControlboard')
controlboardOptions.put('robotIndex',0)
controlboardOptions.put('manipulatorIndex',0)
controlboardDevice = yarp.PolyDriver(controlboardOptions)

pos = controlboardDevice.viewIPositionControl()
pos.positionMove(1,45)

#OpenraveYarpPluginLoader = RaveCreateModule(env,'OpenraveYarpPluginLoader')
#penvStr = OpenraveYarpPluginLoader.SendCommand('getPenv')

#controlboardOptions = yarp.Property()
#controlboardOptions.fromString(penvStr)
#controlboardOptions.put('device','YarpOpenraveControlboard')
#controlboardOptions.put('robotIndex',0)
#controlboardOptions.put('manipulatorIndex',0)

done = False
while not done:
    print 'wait to reach...'
    yarp.delay(1.0) # [s]
    done = pos.checkMotionDone()

