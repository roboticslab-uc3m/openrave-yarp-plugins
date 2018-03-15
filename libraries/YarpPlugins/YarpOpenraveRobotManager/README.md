# YarpOpenraveRobotManager

First open a:
```bash
yarp server
```

- Example 1 (requires [robotDevastation-openrave-models](https://github.com/roboticslab-uc3m/robotDevastation-openrave-models)): Open `YarpOpenraveRobotManager` device with `mapping_room.env.xml` environment with `ODE`, view, apply to robot index `0` call it `/robot`:
   ```bash
   yarpdev --device YarpOpenraveRobotManager --env /usr/local/share/robotDevastation-openrave-models/contexts/openrave/ecro/mapping_room.env.xml --physics ode --view --robotIndex 0 --name /robot
   ```
   Then communicate via:
   ```
   yarp rpc /robot/rpc:s
   ```
