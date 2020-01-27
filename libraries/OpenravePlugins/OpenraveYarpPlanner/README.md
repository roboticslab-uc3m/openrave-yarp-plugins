# OpenraveYarpPlanner

Enables loading one or several YARP plugins in OpenRAVE, typically from [../../libraries/YarpPlugins/](../../YarpPlugins/).

First open a:
```bash
yarp server
```


## Note on Invocation
There are 2 ways to invoke `OpenraveYarpPlanner`:
- Command Line (CLI): Note that the string with parameters passed from the CLI to `OpenraveYarpPlanner` are parsed within its [`int main(const std::string& cmd)`](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/blob/examples-improve/libraries/OpenravePlugins/OpenraveYarpPlanner/OpenraveYarpPlanner.cpp#L57-L96) function. This `main()` function is a strange animal within OpenRAVE plugins as commented at [#30](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/30#issuecomment-306237545), which is affected by several issues, namely [#59](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/59) and [#60](https://github.com/roboticslab-uc3m/openrave-yarp-plugins/issues/60).
- Python: Requires Python `openravepy` module.


## Example 1
```bash
openrave --module OpenraveYarpPlanner
```
