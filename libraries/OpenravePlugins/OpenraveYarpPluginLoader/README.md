# OpenraveYarpPluginLoader

First open a:
```bash
yarp server
```

Note that `OpenraveYarpPluginLoader` uses OpenRAVE plugins `main()`, affected by [#59](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/59) and [#60](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/60). This affects the CLI/`bash` (non-Python) examples.
   
- Example 1 (requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)): Open `teoSim` environment via OpenRAVE, call with OpenraveYarpPluginLoader with robot 0 and `--allManipulators` that takes care of names:
   ```bash
   openrave /usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml --module OpenraveYarpPluginLoader "open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --allManipulators"
   ```
   Then communicate via:
   ```bash
   yarp rpc /teoSim/[kinematic chain name]/rpc:i
   ```

- Example 2 (requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)): We can do funky commands like the following, where `open` acts as a delimiter:
   ```bash
   openrave /usr/local/share/teo-openrave-models/contexts/openrave/teo/teo.robot.xml --module OpenraveYarpPluginLoader "open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 0 open --device controlboardwrapper2 --subdevice YarpOpenraveControlboard --robotIndex 0 --manipulatorIndex 2"
   ```

- Example 3 (requires [teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)):If you prefer to use Python, the Python script equivalent to `teoSim` such that it is not affected by the above issues is contained in the `example` directory:
   ```bash
   python ~/repos/openrave-yarp-plugins/example/python/openraveYarpPluginLoader-controlboard-allManipulators.py
   ```
