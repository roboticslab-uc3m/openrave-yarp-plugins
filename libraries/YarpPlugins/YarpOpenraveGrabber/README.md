# YarpOpenraveGrabber

First open a:
```bash
yarp server
```
- Example 1: Open `YarpOpenraveGrabber` device with `data/testwamcamera.env.xml` environment, view, apply to robot index `0` and sensor index `1` and call it `/camera/img:o`:
   ```bash
   yarpdev --device YarpOpenraveGrabber --env data/testwamcamera.env.xml --view --robotIndex 0 --sensorIndex 0 --name /camera/img:o
   ```
   Open a viewer:
   ```bash
   yarpview --name /yarpview/camera/img:i
   ```
   Connect the viewer:
   ```bash
   yarp connect /camera/img:o /yarpview/camera/img:i
   ```

- Example 2 (requires [robotDevastation-openrave-models](https://github.com/roboticslab-uc3m/robotDevastation-openrave-models)): Open `YarpOpenraveGrabber` device with `mapping_room.env.xml` environment, view, apply to robot index `0` and sensor index `1` and call it `/camera/img:o`:
   ```bash
   yarpdev --device YarpOpenraveGrabber --env /usr/local/share/robotDevastation-openrave-models/contexts/openrave/ecro/mapping_room.env.xml --view --robotIndex 0 --sensorIndex 1 --name /camera/img:o
   ```
   Open a viewer:
   ```bash
   yarpview --name /yarpview/camera/img:i
   ```
   Connect the viewer:
   ```bash
   yarp connect /camera/img:o /yarpview/camera/img:i
   ```
