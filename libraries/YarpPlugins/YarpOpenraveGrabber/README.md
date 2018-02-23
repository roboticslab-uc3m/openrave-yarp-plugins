# YarpOpenraveGrabber

First open a:
```bash
yarp server
```

For now best use:
- Python example 1: https://github.com/roboticslab-uc3m/openrave-yarp-plugins/blob/develop/examples/python/openraveYarpPluginLoader-grabber.py
   Then open a viewer:
   ```bash
   yarpview --name /yarpview/camera/img:i
   ```
   Connect the viewer:
   ```bash
   yarp connect /BarrettWAM/camera /yarpview/camera/img:i
   ```

Example invocation not working due to https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/60 and beyond:
```bash
yarpdev --device YarpOpenraveGrabber --env data/testwamcamera.env.xml --view --robotIndex 0 --sensorIndex 0 --name /camera/img:o
```
