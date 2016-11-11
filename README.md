# openrave-yarp-plugins
OpenRAVE plugins to interface OpenRAVE with YARP.

The main approach is to develop OpenRAVE plugins, which internally load YARP plugins that open ports thanks to their network wrappers. Technically, the OpenRAVE plugin opens a general purpose network wrapper as a "device" YARP plugin (contrcontrolboardwrapper2, grabber...), with a "subdevice" YARP plugin from this repository. The OpenRAVE plugin passes a C-style pointer to the `OpenRAVE::Environment` and whatever other information the "subdevice" YARP plugin requires through a `yarp::os::Property` dictionary.
