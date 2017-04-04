## openrave-yarp-plugins: Installation from Source Code

First install the dependencies:
- [Install CMake](https://github.com/roboticslab-uc3m/installation-guides/blob/develop/install-cmake.md)
- [Install YARP](https://github.com/roboticslab-uc3m/installation-guides/blob/develop/install-yarp.md)
- [Install OpenRAVE](https://github.com/roboticslab-uc3m/installation-guides/blob/develop/install-openrave.md)

### Install openrave-yarp-plugins on Ubuntu (working on all tested versions)

Our software integrates the previous dependencies. Note that you will be prompted for your password upon using `sudo` a couple of times:

```bash
cd  # go home
mkdir -p repos; cd repos  # make $HOME/repos if it does not exist; then, enter it
git clone --recursive https://github.com/roboticslab-uc3m/openrave-yarp-plugins.git  # Download openrave-yarp-plugins software from the repository; Use --recursive to get embedded repositories (technically, git submodules)
cd openrave-yarp-plugins; mkdir build; cd build; cmake ..  # Configure the openrave-yarp-plugins software
make  # Compile
sudo make install  # Install :-)
cp ../scripts/gnome/openrave-yarp-plugins.desktop $HOME/Desktop
```

For CMake `find_package(ROBOTICSLAB_OPENRAVE_YARP_PLUGINS REQUIRED)`, you may also be interested in adding the following to your `~/.bashrc` or `~/.profile`:
```bash
export ROBOTICSLAB_OPENRAVE_YARP_PLUGINS_DIR=$HOME/repos/openrave-yarp-plugins/build  # Points to where TEOConfig.cmake is generated upon running CMake
```

For additional options use ccmake instead of cmake.

