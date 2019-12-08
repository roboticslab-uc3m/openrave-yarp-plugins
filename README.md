# openrave-yarp-plugins

OpenRAVE plugins to interface OpenRAVE with YARP.

Link to Doxygen generated documentation: http://robots.uc3m.es/dox-openrave-yarp-plugins

## Installation

Installation instructions for installing from source can be found [here](doc/openrave-yarp-plugins-install.md).

## Contributing

#### Posting Issues

1. Read [CONTRIBUTING.md](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/blob/master/CONTRIBUTING.md)
2. [Post an issue / Feature request / Specific documentation request](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues)

#### Fork & Pull Request

1. [Fork the repository](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/fork)
2. Create your feature branch (`git checkout -b my-new-feature`) off the `develop` branch, following the [GitFlow git workflow](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow)
3. Commit your changes
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request

## Status

[![Build Status (Linux)](https://travis-ci.com/roboticslab-uc3m/openrave-yarp-plugins.svg?branch=develop)](https://travis-ci.com/roboticslab-uc3m/openrave-yarp-plugins)

[![Coverage Status](https://coveralls.io/repos/roboticslab-uc3m/openrave-yarp-plugins/badge.svg)](https://coveralls.io/r/roboticslab-uc3m/openrave-yarp-plugins)

[![Issues](https://img.shields.io/github/issues/roboticslab-uc3m/openrave-yarp-plugins.svg?label=Issues)](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues)

## Description

The main approach is to develop OpenRAVE plugins (contained in the [libraries/OpenravePlugins/](libraries/OpenravePlugins/) folder), which internally load YARP plugins (contained in the [libraries/YarpPlugins/](libraries/YarpPlugins/) folder) that open ports thanks to their network wrappers. This can be seen in the following figure.

![Architecture Block Diagram](doc/fig/architecture.png)

Technically, the OpenRAVE plugin can directly open YARP ports, or contain one or many `yarp::dev::PolyDriver`. In the latter, it calls `yarp::dev::PolyDriver::open(yarp::os::Searchable &config)` passing a `yarp::os::Property` (note that `Property` is a `Searchable`), typically with the following contents:
- "device": A YARP plugin that is a general purpose network wrapper and thus opens YARP ports (`controlboardwrapper2`, `grabber`...).
- "subdevice": A YARP plugin from this repository (implementing `controlboard`, `grabber`... functionality).
- "penv": A C-style pointer to the `OpenRAVE::Environment` to be used by the "subdevice".
- "name": Can be extracted from the `OpenRAVE::Environment` and can be used for the port names opened by the "device" too.
- Plus, whatever other information the "subdevice" YARP plugin requires (e.g. which `robotIndex` and/or `manipulatorIndex` for control).

## Tutorials

### How to use openrave-yarp-plugins for collision avoidance of a simulated robot
The following commands explain how to use the openrave-yarp-plugins for collision avoidance, using the previous instance of openrave-yarp-plugins as the remote robot.

```bash
# new terminal to open the simulated robot
python examples/python/openraveYarpPluginLoader-controlboard-allManipulators.py

# new terminal to open the collision avoidance simulator 
# NOTE: Expect LONG wait to load the padding model
python examples/python/openraveYarpPluginLoader-controlboard-collision-sim.py

# Then the robot can be commanded via yarp with:
yarp rpc /safe/teoSim/[kinematic chain name]/rpc:i
```

### How to use openrave-yarp-plugins for collision avoidance of a real robot
The following commands explain how to use the openrave-yarp-plugins for collision avoidance, using a real remote robot.

```bash
# new terminal
python examples/python/openraveYarpPluginLoader-controlboard-collision-real.py

# wait for the system to load the padding model
# Then the robot can be commanded via yarp with:
yarp rpc /safe/teo/[kinematic chain name]/rpc:i
```

### How to extract a .pp model from ConvexDecomposition
The following commands explain how to use openrave to create a 3D model (.pp) of the collision space that openrave uses to calculate collisions. Whereas openarave uses it continuously, here we can save the 3D files to use them.

```bash
# new terminal
openrave.py --database convexdecomposition --robot=/usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml #--padding=PADDING --maxHullVertices=MAXHULLVERTICES --mergeThresholdPercent=MERGETHRESHOLDPERCENT
```
At the time to generate the .pp file different params can be set. In our case the most relevants are the **padding**, the **maxHullVertices** and the **mergeThresholdPercent**. **padding** represents the distance from the real plane to the one generated. The last two used to reduce the number of triangles generated in the model).
  
More options can be found [here](http://openrave.org/docs/0.8.0/openravepy/databases.convexdecomposition/).

```bash
# To check the results, execute the following command. It is worth noting that, the triangle count is not correct, this was contrasted using other tool.
openrave.py --database convexdecomposition --robot=/usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml --show
```

To change the file type, [this](https://github.com/roboticslab-uc3m/tools/tree/develop/programs/openraveppToSTL) can be used.

## Similar and Related Projects
- http://www.iearobotics.com/wiki/index.php?title=OpenRave_y_robots_modulares
