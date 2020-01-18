# openraveYarpPluginLoaderClient

Example invocation:

Terminal 1:
```bash
yarp server
```

Terminal 2:
```bash
openrave --module OpenraveYarpPluginLoader "env data/testwamcamera.env.xml" --viewer qtcoin --collision ode
```

Terminal 3:
```bash
openraveYarpPluginLoaderClient --device RGBDSensorWrapper --subdevice YarpOpenraveRGBDSensor --robotIndex 0 --depthSensorIndex 3
```
