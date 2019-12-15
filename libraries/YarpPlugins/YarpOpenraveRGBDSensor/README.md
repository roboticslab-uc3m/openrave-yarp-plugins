# YarpOpenraveRGBDSensor

Disclaimer: Please read about [Offscreen Rendering (OpenRAVE RGB Cameras)](http://robots.uc3m.es/gitbook-installation-guides/install-openrave.html#offscreen-rendering-openrave-rgb-cameras) first. Note that things are easier for pure Depth (no RGB).

First open a:
```bash
yarp server
```

For now best use:
- Python example 1: [examples/python/openraveYarpPluginLoader-rgbdsensor.py](../../../examples/python/openraveYarpPluginLoader-rgbdsensor.py)
   
   Then open a viewer for RGB:
   ```bash
   yarpview --name /yarpview/rgb/img:i
   ```
   Connect the viewer for RGB (change `/BarrettWAM` to `/ecroSim` for alternative):
   ```bash
   yarp connect /BarrettWAM/rgbImage:o /yarpview/rgb/img:i
   ```
   Open another viewer for depth:
   ```bash
   yarpview --name /yarpview/depth/img:i
   ```
   Connect the viewer for depth (change `/BarrettWAM` to `/ecroSim` for alternative):
   ```bash
   yarp connect /BarrettWAM/depthImage:o /yarpview/depth/img:i
   ```

- Python example 2 (no RGB): [examples/python/openraveYarpPluginLoader-rgbdsensor-noRGB.py](../../../examples/python/openraveYarpPluginLoader-rgbdsensor-noRGB.py)

   Open a viewer for depth:
   ```bash
   yarpview --name /yarpview/depth/img:i
   ```
   Connect the viewer for depth (change `/BarrettWAM` to `/ecroSim` for alternative):
   ```bash
   yarp connect /BarrettWAM/depthImage:o /yarpview/depth/img:i
   ```

Example invocation not working due to https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/60 and beyond:
```bash
yarpdev --device YarpOpenraveRGBDSensor --env openrave/mapping_room.env.xml --robotIndex 0 --depthSensorIndex 0 --rgbSensorIndex 1 --view --name /robot
```
