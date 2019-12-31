## openrave-yarp-plugins: Installation from Source Code

First install the dependencies:
- [Install CMake 3.12+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-cmake.md/)
- [Install YCM 0.11+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-ycm.md/)
- [Install YARP 3.2+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-yarp.md/)
- [Install OpenRAVE 0.9+](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-openrave.md/)
- [Install Boost](https://github.com/roboticslab-uc3m/installation-guides/blob/master/install-boost.md/) (implicitly required by OpenRAVE)
  - Compiled components: Boost.Thread
  - Header-only components: Boost.Bind, Boost.SmartPtr
- [Install color-debug](https://github.com/roboticslab-uc3m/color-debug)

# Install openrave-yarp-plugins on Ubuntu (working on all tested versions)

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

For CMake `find_package(ROBOTICSLAB_OPENRAVE_YARP_PLUGINS REQUIRED)`, you may also be interested in adding the following to your `~/.bashrc` or `~/.profile`:
```bash
export ROBOTICSLAB_OPENRAVE_YARP_PLUGINS_DIR=$HOME/repos/openrave-yarp-plugins/build  # Points to where OPENRAVE_YARP_PLUGINSConfig.cmake is generated upon running CMake
```

For additional options use `ccmake` instead of `cmake`.

Libraries with known dependencies:
- [../libraries/YarpPlugins/YarpOpenraveRobotManager](../libraries/YarpPlugins/YarpOpenraveRobotManager/README.md#note-on-installation)
