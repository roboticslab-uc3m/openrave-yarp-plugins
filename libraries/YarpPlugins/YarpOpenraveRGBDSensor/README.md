# YarpOpenraveRGBDSensor

For now best use:
- https://github.com/roboticslab-uc3m/openrave-yarp-plugins/blob/develop/examples/python/openraveYarpPluginLoader-rgbdsensor.py
   
   Open a viewer for RGB:
   ```bash
   yarpview --name /yarpview/rgb/img:i
   ```
   Connect the viewer for RGB:
   ```bash
   yarp connect /ecroSim/rgbImage:o /yarpview/rgb/img:i
   ```
   Open another viewer for depth:
   ```bash
   yarpview --name /yarpview/depth/img:i
   ```
   Connect the viewer for depth:
   ```bash
   yarp connect /ecroSim/depthImage:o /yarpview/depth/img:i
   ```

- https://github.com/roboticslab-uc3m/openrave-yarp-plugins/blob/develop/examples/python/openraveYarpPluginLoader-rgbdsensor-noRGB.py

Example invocation not working due to https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/60 and beyond:
```bash
yarpdev --device YarpOpenraveRGBDSensor --env /usr/local/share/robotDevastation-openrave-models/contexts/openrave/ecro/mapping_room.env.xml --robotIndex 0 --depthSensorIndex 0 --rgbSensorIndex 1 --view --name /robot
```
