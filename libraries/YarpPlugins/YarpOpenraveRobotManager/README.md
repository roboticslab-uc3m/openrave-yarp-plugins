# YarpOpenraveRobotManager

First open a:
```bash
yarp server
```

- Example 1: Open `YarpOpenraveRobotManager` device with `mapping_room.env.xml` environment with `ODE`, view, apply to robot index `0` call it `/robot`:
   ```bash
   yarpdev --device YarpOpenraveRobotManager --env /usr/local/share/robotDevastation-openrave-models/contexts/openrave/ecro/mapping_room.env.xml --physics ode --view --robotIndex 0 --name /robot
   ```
