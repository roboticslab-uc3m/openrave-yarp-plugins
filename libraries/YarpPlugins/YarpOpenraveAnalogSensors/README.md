# YarpOpenraveAnalogSensors


## Requirements
Depends on:
- [YARP 3.3+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-yarp.md/)

## Usage
First open a:
```bash
yarp server
```

### Notes on Invocation
These examples use the `yarpdev` executable to load `YarpOpenraveAnalogSensors` directly. However, you can find more interesting examples loading `YarpOpenraveAnalogSensors` via `OpenraveYarpPluginLoader` at [../../OpenravePlugins/OpenraveYarpPluginLoader#yarpopenraveanalogsensors](../../OpenravePlugins/OpenraveYarpPluginLoader#yarpopenraveanalogsensors).

### Example 1
Open `YarpOpenraveAnalogSensors` device with the `physicsRobot.env.xml` environment provided upon installation, apply to robot index `0` and force/torque sensor index `0` (`name` and `view` are required):

```bash
yarpdev --device multipleanalogsensorsserver --subdevice YarpOpenraveAnalogSensors --robotIndex 0 --ftSensorIndices 0 --period 50 --env openrave/physicsRobot.env.xml --name /Physicsbot --view
```

Note: This invocation not working due to [#60](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/60) and beyond. Therefore, you can instead jump directly and use: [../../OpenravePlugins/OpenraveYarpPluginLoader#yarpopenraveanalogsensors](../../OpenravePlugins/OpenraveYarpPluginLoader#yarpopenraveanalogsensors)

Read from port:
```bash
yarp read ... /Physicsbot/measures:o
```
