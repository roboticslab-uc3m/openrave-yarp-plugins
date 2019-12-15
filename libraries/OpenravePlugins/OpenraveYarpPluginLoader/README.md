# OpenraveYarpPluginLoader

Enables loading one or several YARP plugins in OpenRAVE, typically from [../../libraries/YarpPlugins/](../../YarpPlugins/).

First open a:
```bash
yarp server
```

## Note on Invocation
There are 2 ways to invoke `OpenraveYarpPluginLoader`:
- Command Line (CLI): Note that the string with parameters passed from the CLI to `OpenraveYarpPluginLoader` are parsed within its [`int main(const std::string& cmd)`](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/blob/examples-improve/libraries/OpenravePlugins/OpenraveYarpPluginLoader/OpenraveYarpPluginLoader.cpp#L57-L96) function. This `main()` function is a strange animal within OpenRAVE plugins as commented at [#30](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/30#issuecomment-306237545), which is affected by several issues, namely [#59](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/59) and [#60](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/60).
- Python: Requires Python `openravepy` module.

## YarpOpenraveControlboard

Use `OpenraveYarpPluginLoader` to replicate any example of [../../YarpPlugins/YarpOpenraveControlboard](../../YarpPlugins/YarpOpenraveControlboard#yarpopenravecontrolboard).

### Example 1
The [first example](../../YarpPlugins/YarpOpenraveControlboard#example-1) becomes (note that `name` and `view` parameters are no longer required):
```bash
openrave --module OpenraveYarpPluginLoader "env data/lab1.env.xml open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 0"
```
The equivalent Python script can be found at: [../../../examples/python/openraveYarpPluginLoader-controlboard.py](../../../examples/python/openraveYarpPluginLoader-controlboard.py)

Then follow the commands of the [first example](../../YarpPlugins/YarpOpenraveControlboard#example-1).

### Example 2
Now you can even use `allRobots` and/or `allManipulators` (example using the environment loaded by OpenRAVE):
```bash
openrave data/lab1.env.xml --module OpenraveYarpPluginLoader "open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --allRobots --allManipulators"
```
The equivalent Python script can be found at: [../../../examples/python/openraveYarpPluginLoader-controlboard-allManipulators.py](../../../examples/python/openraveYarpPluginLoader-controlboard-allManipulators.py)

### Example 3
We can also do funky commands like the following, with concatenated `env` and several `open` commands:
```bash
# Requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)
openrave --module OpenraveYarpPluginLoader "env openrave/teo/teo.robot.xml open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 0 open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 2"
```


## YarpOpenraveGrabber

Use `OpenraveYarpPluginLoader` to replicate any example of [../../YarpPlugins/YarpOpenraveGrabber](../../YarpPlugins/YarpOpenraveGrabber#yarpopenravegrabber).

### Example 1
The [first example](../../YarpPlugins/YarpOpenraveGrabber#example-1) becomes (note that `name` and `view` parameters are no longer required):
```bash
openrave --module OpenraveYarpPluginLoader "env data/lab1.env.xml open --device grabberDual --subdevice YarpOpenraveGrabber --robotIndex 0 --sensorIndex 0"
```
The equivalent Python script can be found at: [../../../examples/python/openraveYarpPluginLoader-grabber.py](../../../examples/python/openraveYarpPluginLoader-grabber.py)

Then follow the commands of the [first example](../../YarpPlugins/YarpOpenraveGrabber#example-1).


## YarpOpenraveRGBDSensor

Use `OpenraveYarpPluginLoader` to replicate any example of [../../YarpPlugins/YarpOpenraveRGBDSensor](../../YarpPlugins/YarpOpenraveRGBDSensor#yarpopenravergbdsensor).


## YarpOpenraveRobotManager

Use `OpenraveYarpPluginLoader` to replicate any example of [../../YarpPlugins/YarpOpenraveRobotManager](../../YarpPlugins/YarpOpenraveRobotManager#yarpopenraverobotmanager).
