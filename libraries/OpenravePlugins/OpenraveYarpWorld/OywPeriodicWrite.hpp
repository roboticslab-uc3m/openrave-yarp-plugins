// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OPEN_PORT_PERIODIC_WRITE_HPP__
#define __OPEN_PORT_PERIODIC_WRITE_HPP__

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Port.h>

class OpenraveYarpWorld;

/**
 * @ingroup OpenraveYarpPluginLoader
 * @brief Publishes open YARP plugin list.
 */
class OywPeriodicWrite : yarp::os::PeriodicThread, public yarp::os::Port
{
public:
    OywPeriodicWrite();
    void setOpenraveYarpWorldPtr(OpenraveYarpWorld *value) { openraveYarpWorldPtr = value; }
private:
    OpenraveYarpWorld* openraveYarpWorldPtr;
    virtual void run() override;
};

#endif // __OPEN_PORT_PERIODIC_WRITE_HPP__
