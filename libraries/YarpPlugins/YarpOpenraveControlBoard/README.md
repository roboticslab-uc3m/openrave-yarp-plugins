# YarpOpenraveControlBoard

First open a:
```bash
yarp server
```

## Note on Invocation
These examples use the `yarpdev` executable to load `YarpOpenraveControlBoard` directly. However, you can find more interesting examples loading `YarpOpenraveControlBoard` via `OpenraveYarpPluginLoader` at [../../OpenravePlugins/OpenraveYarpPluginLoader#yarpopenravecontrolboard](../../OpenravePlugins/OpenraveYarpPluginLoader#yarpopenravecontrolboard).

## Example 1
Open `YarpOpenraveControlBoard` device with `data/lab1.env.xml` environment, view, apply to robot index `0` and manipulator index `0` (`name` and `view` are required):

```bash
yarpdev --device YarpOpenraveControlBoard --env data/lab1.env.xml --robotIndex 0 --manipulatorIndex 0 --name /BarrettWAM/arm --view
```

Then communicate via:
```
yarp rpc /BarrettWAM/arm/rpc:i
```

For instance:
```bash
set pos 1 45
```

## Example 2
We can also load environments with our favorite heroes, such as:
```bash
# Requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)
yarpdev --device YarpOpenraveControlBoard --env openrave/teo/teo.robot.xml --robotIndex 0 --manipulatorIndex 2 --name /teoSim/rightArm --view
```

Then communicate via:
```bash
yarp rpc /teoSim/rightArm/rpc:i
```

For instance:
```bash
set pos 0 -45
```
