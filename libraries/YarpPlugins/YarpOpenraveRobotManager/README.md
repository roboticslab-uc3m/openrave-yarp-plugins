# YarpOpenraveRobotManager

First open a:
```bash
yarp server
```

## Note on Invocation
These examples use the `yarpdev` executable to load `YarpOpenraveRobotManager` directly. However, you can find more interesting examples loading `YarpOpenraveRobotManager` via `OpenraveYarpPluginLoader` at [../../OpenravePlugins/OpenraveYarpPluginLoader#robotmanager](../../OpenravePlugins/OpenraveYarpPluginLoader#yarpopenraverobotmanager).

## Example 1
Open `YarpOpenraveRobotManager` device with `mapping_room.env.xml` environment with `ODE`, view, apply to robot index `0` call it `/robot`:
```bash
# requires [asrob-uc3m/robotDevastation-openrave-models](https://github.com/asrob-uc3m/robotDevastation-openrave-models)
yarpdev --device YarpOpenraveRobotManager --env openrave/mapping_room.env.xml --physics ode --robotIndex 0 --view --name /ecroSim
```

Then communicate via:
```
yarp rpc /ecroSim/rpc:s
```

For instance:
```bash
movf 10.0
```
