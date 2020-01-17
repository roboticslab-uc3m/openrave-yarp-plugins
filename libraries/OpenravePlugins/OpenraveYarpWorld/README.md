# OpenraveYarpWorld

Enables interacting with and OpenRAVE environment via YARP

## Example Invocation

First open a:
```bash
yarp server
```

Then:
```bash
openrave data/lab1.env.xml --module OpenraveYarpWorld
```
The equivalent Python script can be found at: [../../../examples/python/openraveYarpWorldRpcResponder.py](../../../examples/python/openraveYarpWorldRpcResponder.py)

Then communicate via:
```
yarp rpc /OpenraveYarpWorld/rpc:s
```

For instance, get help:
```bash
help
```

Get a list of information on the environment existing elements:
```bash
list
```

A `world mk box/sbox (three params for size) (three params for pos)` example to create a dynamic box (an `sbox` would be static), which is interesting if you activate Physics Engine ODE and Set Gravity -Z!
```bash
world mk box 0.1 0.1 0.1 0 0 1.5
```

A `world mk cyl/scyl (radius height) (three params for pos)` example to create a static cylinder:
```bash
world mk scyl 0.1 0.1 0 0.5 1.5
```

A `world mk sph/ssph (radius) (three params for pos)` example to create a static sphere:
```bash
world mk ssph 0.1 0.5 0 1.5
```

A `world mk file (absolute path)` example:
```bash
world mk file data/mug1.kinbody.xml
```

Now move the mug:
```bash
world set file_0 0 0 1
```

A `world get (name)` example:
```bash
world get file_0
```

A `world grab (manipulatorName) (objName) 0/1` example (replace `arm` for `rightArm` if you are with `teoSim`):
```bash
world grab arm box_0 1
```

A `world fk (robotName) (manipulatorName)` example:
```bash
world fk BarrettWAM arm
```

