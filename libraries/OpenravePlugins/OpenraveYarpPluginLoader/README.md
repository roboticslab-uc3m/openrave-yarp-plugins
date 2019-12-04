# OpenraveYarpPluginLoader

First open a:
```bash
yarp server
```

## Command Line (CLI) Invocation
Note that the string with parameters passed from the CLI to `OpenraveYarpPluginLoader` are parsed within its [`int main(const std::string& cmd)`](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/blob/examples-improve/libraries/OpenravePlugins/OpenraveYarpPluginLoader/OpenraveYarpPluginLoader.cpp#L57-L96) function. This `main()` function is a strange animal within OpenRAVE plugins as commented at [#30](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/30#issuecomment-306237545), which is affected by several issues, namely [#59](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/59) and [#60](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/60).
   
### Example 1
To use `OpenraveYarpPluginLoader` with `--robotIndex 0` and `--allManipulators` that takes care of names:
```bash
openrave data/lab1.env.xml --module OpenraveYarpPluginLoader "open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --allManipulators"
```

Then communicate via:
```bash
yarp rpc /BarrettWAM/arm/rpc:i
```

For instance:
```bash
set pos 1 45
```


### Example 2
We can do funky commands like the following, with concatenated `env` and `open` commands:
```bash
# Requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)
openrave --module OpenraveYarpPluginLoader "load env openrave/teo/teo.robot.xml open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 0 open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 2"
```

## Python Invocation
The Python script equivalent to `teoSim` such that it is not affected by the above mentioned CLI issues is contained in the [examples/python/](../../../examples/python/) directory:
```bash
# Requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)
python ../../../examples/python/openraveYarpPluginLoader-controlboard-allManipulators.py
```
