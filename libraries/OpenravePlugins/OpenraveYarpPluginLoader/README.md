# OpenraveYarpPluginLoader

First open a:
```bash
yarp server
```

## Command Line (CLI) Invocation
Note that `OpenraveYarpPluginLoader` uses OpenRAVE plugins `main()`, affected by [#59](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/59) and [#60](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/60). This affects the CLI/`bash` (non-Python) examples.
   
### Example 1
Open `teoSim` environment via OpenRAVE, call with OpenraveYarpPluginLoader with robot index `0` and `--allManipulators` that takes care of names:
```bash
# requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)): 
openrave /usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml --module OpenraveYarpPluginLoader "open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --allManipulators"
```

Then communicate via:
```bash
yarp rpc /teoSim/[kinematic chain name]/rpc:i
```

### Example 2
We can do funky commands like the following, where `open` acts as a delimiter:
```bash
# Requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)
openrave /usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml --module OpenraveYarpPluginLoader "open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 0 open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 2"
```

## Python Invocation
The Python script equivalent to `teoSim` such that it is not affected by the above issues is contained in the `example` directory:
```bash
# Requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)
python ~/repos/openrave-yarp-plugins/examples/python/openraveYarpPluginLoader-controlboard-allManipulators.py
```
