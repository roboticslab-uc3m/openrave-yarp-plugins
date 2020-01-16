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

Get information on the environment existing elements:
```bash
info
```

Create a dynamic box (an `sbox` would be static), which is interesting if you activate Physics Engine ODE and Set Gravity -Z!
```bash
world mk box 0.1 0.1 0.1 0 0 1.5
```

A `world grab (manipulator) (obj) (num) 0/1` example (replace `arm` for `rightArm` if you are with `teoSim`):
```bash
world grab arm obj box_1 1
```

More examples:
- `world whereis tcp (manipulator)`
- `world whereis obj (name)`
