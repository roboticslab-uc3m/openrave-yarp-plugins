# openraveYarpWorldClientMesh

In order to perform surface reconstruction, this application depends on the *YarpCloudUtils*
library available in [roboticslab-uc3m/vision](https://github.com/roboticslab-uc3m/vision).

Example invocation:

Terminal 1:
```bash
yarp server
```

Terminal 2 (requires [roboticslab-uc3m/vision](https://github.com/roboticslab-uc3m/vision)):
```bash
yarpdev --context sensors --from asus-xtion.ini
```

Terminal 3 (requires [roboticslab-uc3m/teo-openrave-models](https://github.com/roboticslab-uc3m/teo-openrave-models)):
```bash
openrave /usr/local/share/teo-openrave-models/openrave/teo_lacqueyFetch.robot.xml --module OpenraveYarpWorld
```

Terminal 4:
```bash
openraveYarpWorldClientMesh --robot teoSim --sensor xtion --remote /xtion
```
