// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPEN_PORT_PERIODIC_WRITE_HPP__
#define __OPEN_PORT_PERIODIC_WRITE_HPP__

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Port.h>

class OpenraveYarpPluginLoader;

/**
 * @ingroup OpenraveYarpPluginLoader
 * @brief Publishes open YARP plugin list.
 */
class OywPeriodicWrite : yarp::os::PeriodicThread, public yarp::os::Port
{
public:
    OywPeriodicWrite();
    void setOpenraveYarpPluginLoaderPtr(OpenraveYarpPluginLoader *value) { openraveYarpPluginLoaderPtr = value; }
private:
    OpenraveYarpPluginLoader* openraveYarpPluginLoaderPtr;
    virtual void run() override;
};

#endif // __OPEN_PORT_PERIODIC_WRITE_HPP__
