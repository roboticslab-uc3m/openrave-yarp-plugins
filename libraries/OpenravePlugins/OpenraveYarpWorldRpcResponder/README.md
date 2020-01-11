# OpenraveYarpWorldRpcResponder

Enables interacting with and OpenRAVE environment via YARP

## Example Invocation

First open a:
```bash
yarp server
```

Then:
```bash
openrave data/lab1.env.xml --module OpenraveYarpWorldRpcResponder
```
The equivalent Python script can be found at: [../../../examples/python/openraveYarpWorldRpcResponder.py](../../../examples/python/openraveYarpWorldRpcResponder.py)

Then communicate via:
```
yarp rpc /OpenraveYarpWorldRpcResponder/rpc:s
```

For instance:
```bash
help
```

Or:
```bash
info
```

More examples:
- `world mk box 0.1 0.1 0.1 0 0 1.5` (a `sbox` would be static; activate Physics Engine ODE and Set Gravity -Z!)
- `world grab (manipulator) (obj) (num) 0/1`
- `world whereis tcp (manipulator)`
- `world whereis obj (name)`
