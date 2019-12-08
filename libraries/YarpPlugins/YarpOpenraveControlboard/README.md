# YarpOpenraveControlboard

First open a:
```bash
yarp server
```

## Command Line (CLI) Invocation

### Example 1
Open `YarpOpenraveControlboard` device with `data/lab1.env.xml` environment, view, apply to robot index `0` and manipulator index `0` (`name` and `view` are required):

```bash
yarpdev --device YarpOpenraveControlboard --env data/lab1.env.xml --robotIndex 0 --manipulatorIndex 0 --name /BarrettWAM/arm --view
```

Then communicate via:
```
yarp rpc /BarrettWAM/arm/rpc:i
```

For instance:
```bash
set pos 1 45
```

### Example 2
We can also load environments with our favorite heroes, such as:
```bash
# Requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)
yarpdev --device YarpOpenraveControlboard --env /usr/local/share/teo-openrave-models/openrave/teo/teo.robot.xml --robotIndex 0 --manipulatorIndex 2 --name /teoSim/rightArm --view
```

Then communicate via:
```bash
yarp rpc /teoSim/rightArm/rpc:i
```

For instance:
```bash
set pos 0 -45
```

### More Examples
You can find more examples with `YarpOpenraveControlboard` at [libraries/OpenravePlugins/OpenraveYarpPluginLoader](../../OpenravePlugins/OpenraveYarpPluginLoader).
