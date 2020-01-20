# YarpOpenraveRGBDSensor

First open a:
```bash
yarp server
```

# Notes on Invocation
- RGB Disclaimer: Please read about [Offscreen Rendering (OpenRAVE RGB Cameras)](http://robots.uc3m.es/gitbook-installation-guides/install-openrave.html#offscreen-rendering-openrave-rgb-cameras) first.
- Depth Disclaimer: Make sure you use ODE as your Collision Checker (e.g. via `openrave --collison ode` or switching within the OpenRAVE GUI).
- These examples use the `yarpdev` executable to load `YarpOpenraveRGBDSensor` directly. However, you can find more interesting examples loading `YarpOpenraveRGBDSensor` via `OpenraveYarpPluginLoader` at [../../OpenravePlugins/OpenraveYarpPluginLoader#yarpopenravergbdsensor](../../OpenravePlugins/OpenraveYarpPluginLoader#yarpopenravergbdsensor).

## Example 1
Open `YarpOpenraveRGBDSensor` device with `data/testwamcamera.env.xml` environment, view, apply to robot index `0`, rgbSensor index `0` and depthSensorIndex `1` (`name` and `view` are required):

```bash
yarpdev --device YarpOpenraveRGBDSensor --env data/testwamcamera.env.xml --robotIndex 0 --rgbSensorIndex 0 --depthSensorIndex 3 --view --name /BarrettWAM --collision ode
```

Note: There is a race condition and `--collsion ode` is not caught. Via this invocation, if FCL is installed, activate collision ODE via the GUI.
   
Then open a viewer for RGB:
```bash
yarpview --name /yarpview/rgb/img:i
```

Connect the viewer for RGB (remove `mjpeg` if not available; change `/BarrettWAM` to `/ecroSim` for alternative):
```bash
yarp connect /BarrettWAM/rgbImage:o /yarpview/rgb/img:i mjpeg
```

Open another viewer for depth:
```bash
yarpview --name /yarpview/depth/img:i
```

Connect the viewer for depth (remove `udp+recv.portmonitor+type.dll+file.depthimage` if not available; change `/BarrettWAM` to `/ecroSim` for alternative):
```bash
yarp connect /BarrettWAM/depthImage:o /yarpview/depth/img:i udp+recv.portmonitor+type.dll+file.depthimage
```

## Example 2 (no RGB)

```bash
yarpdev --device YarpOpenraveRGBDSensor --env openrave/mapping_room.env.xml --robotIndex 0 --depthSensorIndex 0 --view --name /ecroSim --collision ode
```

Note: There is a race condition and `--collsion ode` is not caught. Via this invocation, if FCL is installed, activate collision ODE via the GUI.

[examples/python/openraveYarpPluginLoader-rgbdsensor-noRGB.py](../../../examples/python/openraveYarpPluginLoader-rgbdsensor-noRGB.py)

Open a viewer for depth:
```bash
yarpview --name /yarpview/depth/img:i
```

Connect the viewer for depth (remove `udp+recv.portmonitor+type.dll+file.depthimage` if not available; change `/BarrettWAM` to `/ecroSim` for alternative):
```bash
yarp connect /BarrettWAM/depthImage:o /yarpview/depth/img:i udp+recv.portmonitor+type.dll+file.depthimage
```
