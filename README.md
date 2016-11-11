# openrave-yarp-plugins
OpenRAVE plugins to interface OpenRAVE with YARP.

The main approach is to develop OpenRAVE plugins, which internally load YARP plugins that open ports thanks to their network wrappers. Technically, the OpenRAVE plugin opens a `yarp::dev::PolyDriver` with a YARP plugin that is a general purpose network wrapper  (contrcontrolboardwrapper2, grabber...) as a "device" in the `yarp::os::Property` that is passed on the `yarp::dev::PolyDriver` `open(yarp::os::Searchable &config)` (note that `Property` is a `Searchable`), with a YARP plugin from this repository as a "subdevice". The OpenRAVE plugin passes a C-style pointer to the `OpenRAVE::Environment` and whatever other information the "subdevice" YARP plugin requires through the `yarp::os::Property` dictionary.
