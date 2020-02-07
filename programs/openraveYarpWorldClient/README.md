# openraveYarpWorldClient

Example invocation:

Terminal 1:
```bash
yarp server
```

Terminal 2:
```bash
openrave data/lab1.env.xml --module OpenraveYarpWorld
```

Terminal 3:
```bash
openraveYarpWorldClient --file data/mug1.kinbody.xml --pos 0.5 0.5 1.5
```
