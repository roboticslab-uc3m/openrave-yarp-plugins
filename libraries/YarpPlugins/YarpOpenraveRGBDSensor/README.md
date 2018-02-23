# YarpOpenraveRGBDSensor

For now best use:
- https://github.com/roboticslab-uc3m/openrave-yarp-plugins/blob/develop/examples/python/openraveYarpPluginLoader-rgbdsensor.py
- https://github.com/roboticslab-uc3m/openrave-yarp-plugins/blob/develop/examples/python/openraveYarpPluginLoader-rgbdsensor-noRGB.py

Example invocation not working due to https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/60 and beyond:
```bash
yarpdev --device YarpOpenraveRGBDSensor --env /usr/local/share/robotDevastation-openrave-models/contexts/openrave/ecro/mapping_room.env.xml --robotIndex 0 --depthSensorIndex 0 --rgbSensorIndex 1 --view --name /robot
```
