# openrave-yarp-plugins: Installation from Source Code

First install the dependencies:
- [Install CMake 3.19+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-cmake.md)
- [Install YCM 0.11+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-ycm.md)
- [Install YARP 3.11+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-yarp.md)
- [Install OpenRAVE 0.115+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-openrave.md)
- [Install Boost](https://github.com/roboticslab-uc3m/installation-guides/blob/master/docs/install-boost.md) (implicitly required by OpenRAVE)
    - Compiled components: Boost.Thread
    - Header-only components: Boost.Bind, Boost.SmartPtr

### Components with known additional/specific dependencies
- [../libraries/YarpPlugins/YarpOpenraveAnalogSensors](../libraries/YarpPlugins/YarpOpenraveAnalogSensors#requirements)
- [../libraries/YarpPlugins/YarpOpenraveRobotManager](../libraries/YarpPlugins/YarpOpenraveRobotManager#requirements)

## Install openrave-yarp-plugins on Ubuntu (working on all tested versions)

Our software integrates the previous dependencies. Note that you will be prompted for your password upon using `sudo` a couple of times:

```bash
cd  # go home
mkdir -p repos; cd repos  # create $HOME/repos if it does not exist; then, enter it
git clone https://github.com/roboticslab-uc3m/openrave-yarp-plugins.git  # Download openrave-yarp-plugins software from the repository
cd openrave-yarp-plugins; mkdir build; cd build; cmake ..  # Configure the openrave-yarp-plugins software
make -j$(nproc)  # Compile
sudo make install  # Install :-)
sudo ldconfig  # Just in case...
cd  # go home
```

For additional options, use `ccmake` instead of `cmake`.
