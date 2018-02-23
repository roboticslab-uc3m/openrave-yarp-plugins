# YarpOpenraveControlboard

First open yarp server:
```bash
yarpserver
```

- Example 1: Open `teoSim` with `--allManipulators`:
```bash
openrave /usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml --module OpenraveYarpPluginLoader "open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --allManipulators"
   ```

Example 2: Open `/teoSim/rightArm`:
```bash
yarpdev --device YarpOpenraveControlboard --name /teoSim/rightArm --env /usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml  --robotIndex 0 --manipulatorIndex 2 --view
```
