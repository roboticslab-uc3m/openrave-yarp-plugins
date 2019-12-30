// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPEN_PORT_PERIODIC_WRITE_HPP__
#define __OPEN_PORT_PERIODIC_WRITE_HPP__

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Port.h>

class OpenraveYarpPluginLoader;

class OpenPortPeriodicWrite : yarp::os::PeriodicThread, public yarp::os::Port
{
public:
    OpenPortPeriodicWrite();
    void setOpenraveYarpPluginLoaderPtr(OpenraveYarpPluginLoader *value) { openraveYarpPluginLoaderPtr = value; }
private:
    OpenraveYarpPluginLoader* openraveYarpPluginLoaderPtr;
    virtual void run() override;
};

#endif // __OPEN_PORT_PERIODIC_WRITE_HPP__
