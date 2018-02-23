# YarpOpenraveGrabber

First open a:
```bash
yarp server
```

- Example 1 (requires [robotDevastation-openrave-models](https://github.com/roboticslab-uc3m/robotDevastation-openrave-models)): Open `YarpOpenraveGrabber` device with `mapping_room.env.xml` environment, view, apply to robot index `0` and sensor index `1` and call it `/camera`:
   ```bash
   yarpdev --device YarpOpenraveGrabber --env /usr/local/share/robotDevastation-openrave-models/contexts/openrave/ecro/mapping_room.env.xml --view --robotIndex 0 --sensorIndex 1 --name /camera
   ```
