# YarpOpenraveControlboard

First open yarp server:
```bash
yarpserver
```

- Example 1: Open `YarpOpenraveControlboard` device with `teoSim` environment, apply to robot 0 manipulator 2 and call it `/teoSim/rightArm`:
   ```bash
   yarpdev --device YarpOpenraveControlboard --env /usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml   --view --robotIndex 0 --manipulatorIndex 2 --name /teoSim/rightArm
   ```
   Then communicate via `yarp rpc /teoSim/rightArm/rpc:i`.

- Example 2: Open `teoSim` environment via OpenRAVE, call with OpenraveYarpPluginLoader with robot 0 and `--allManipulators` that takes care of names:
   ```bash
   openrave /usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml --module OpenraveYarpPluginLoader "open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --allManipulators"
   ```
   Then communicate via `yarp rpc /teoSim/rightArm/rpc:i` or other.
