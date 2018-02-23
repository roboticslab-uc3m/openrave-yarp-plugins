# YarpOpenraveControlboard

First open a:
```bash
yarp server
```

- Example 1: Open `YarpOpenraveControlboard` device with `data/lab1.env.xml` environment, view, apply to robot index `0` and manipulator index `0` and call it `/robot`:
   ```bash
   yarpdev --device YarpOpenraveControlboard --name /robot --env data/lab1.env.xml --view --robotIndex 0 --manipulatorIndex 0
   ```
   Then communicate via:
   ```
   yarp rpc /robot/rpc:i
   ```

- Example 2 (requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)): Open `YarpOpenraveControlboard` device with `teoSim` environment, view, apply to robot `0` manipulator `2` and call it `/teoSim/rightArm`:
   ```bash
   yarpdev --device YarpOpenraveControlboard --env /usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml   --view --robotIndex 0 --manipulatorIndex 2 --name /teoSim/rightArm
   ```
   Then communicate via:
   ```bash
   yarp rpc /teoSim/rightArm/rpc:i
   ```
