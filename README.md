# openrave-yarp-plugins
OpenRAVE plugins to interface OpenRAVE with YARP.

The main approach is to develop OpenRAVE plugins (contained in the [openraveplugins](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/tree/develop/openraveplugins) folder), which internally load YARP plugins (contained in the [yarpplugins](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/tree/develop/yarpplugins) folder) that open ports thanks to their network wrappers. Technically, the OpenRAVE plugin contains one or many `yarp::dev::PolyDriver` and calls  `open(yarp::os::Searchable &config)` passing a `yarp::os::Property` (note that `Property` is a `Searchable`) with the following contents:
- "device": A YARP plugin that is a general purpose network wrapper  (contrcontrolboardwrapper2, grabber...).
- "subdevice": A YARP plugin from this repository (implements controlboard, grabber... functionality).
- "penv": A C-style pointer to the `OpenRAVE::Environment` to be used by the "subdevice".
- Plus, whatever other information the "subdevice" YARP plugin requires (e.g. which manipulator of which robot it will control).

![Block Diagram](doc/readme/Diagram.png)

# Tutorials: (How to use the openrave-yarp-plugins for collision avoidance with teoSim)
The following commands explain how to use the openrave-yarp-plugins for collision avoidance using teoSim as the remote robot. Note that here, teoSim can be replaced for the real robot.

```bash
yarpserver
# new terminal 
teoSim
# new terminal
cd; cd repos/openrave-yarp-plugins/build/lib
python ../../examples/openrave-YarpRobot.py
# wait for the system to load the padding model
# Then the robot can be commanded via yarp with:
yarp rpc /safe/teoSim/[kinematic chain name]/rpc:i
```
