# YarpOpenraveAnalogSensors


## Requirements
Depends on:
- [YARP 3.3+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-yarp.md)

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

Read from port:
```bash
yarp read ... /Physicsbot/measures:o
```

Or even (requires [BottleExtract](https://github.com/roboticslab-uc3m/yarp-devices/tree/e3b9d8e0874c75d26a1b0e05d9d63bcf805aff76/libraries/YarpPlugins/PortMonitorPlugins/cpp/bottle_extract_portmonitor)):
```bash
yarpscope --index "(0 1 2 3 4 5)" --color "(Red Green Blue LightRed LightGreen LightBlue)"
```
```bash
yarp connect /Physicsbot/measures:o /yarpscope tcp+recv.portmonitor+type.dll+file.BottleExtract+index.5+subindex.0+subsubindex.0
```
