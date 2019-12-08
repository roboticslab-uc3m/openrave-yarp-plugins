# YarpOpenraveRobotManager

First open a:
```bash
yarp server
```

## Example 1
Open `YarpOpenraveRobotManager` device with `mapping_room.env.xml` environment with `ODE`, view, apply to robot index `0` call it `/robot`:
```bash
# requires [robotDevastation-openrave-models](https://github.com/asrob-uc3m/robotDevastation-openrave-models)
yarpdev --device YarpOpenraveRobotManager --env /usr/local/share/robotDevastation-openrave-models/contexts/openrave/ecro/mapping_room.env.xml --physics ode --robotIndex 0 --view --name /robot
```

Then communicate via:
```
yarp rpc /robot/rpc:s
```

For instance:
```bash
movf 10.0
```
