#Change this for your PLUGIN folder
export FORCESENSOR_PATH=$HOME/grasp/workspace/OpenGRASP/OpenRAVE_Plugins/sensors/ForceSensor

export OPENRAVE_DATA=$OPENRAVE_DATA:$FORCESENSOR_PATH/examples
export OPENRAVE_PLUGINS=$OPENRAVE_PLUGINS:$FORCESENSOR_PATH/lib
export PYTHONPATH=$PYTHONPATH:`openrave-config --python-dir`
