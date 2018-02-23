# OpenraveYarpPluginLoader

First open a:
```bash
yarp server
```

- Example 1 (requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)): Open `teoSim` environment via OpenRAVE, call with OpenraveYarpPluginLoader with robot 0 and `--allManipulators` that takes care of names:
   ```bash
   openrave /usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml --module OpenraveYarpPluginLoader "open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --allManipulators"
   ```
   Then communicate via:
   ```bash
   yarp rpc /teoSim/[kinematic chain name]/rpc:i
   ```
