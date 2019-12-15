# YarpOpenraveGrabber

Disclaimer: Please read about [Offscreen Rendering (OpenRAVE RGB Cameras)](http://robots.uc3m.es/gitbook-installation-guides/install-openrave.html#offscreen-rendering-openrave-rgb-cameras) first.

First open a:
```bash
yarp server
```

## Example 1
Open `YarpOpenraveGrabber` device with `data/testwamcamera.env.xml` environment, view, apply to robot index `0` and sensor index `0` (`name` and `view` are required):

```bash
yarpdev --device YarpOpenraveGrabber --env data/testwamcamera.env.xml --view --robotIndex 0 --sensorIndex 0 --name /BarrettWAM/camera
```

Note: This invocation not working due to [#60](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/60) and beyond. Therefore, instead use: [../../../examples/python/openraveYarpPluginLoader-grabber.py](../../../examples/python/openraveYarpPluginLoader-grabber.py)

Then open a viewer:
```bash
yarpview --name /yarpview/camera/img:i
```

Connect the viewer:
```bash
yarp connect /BarrettWAM/camera /yarpview/camera/img:i
```
