// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __OYW_PERIODIC_WRITE_HPP__
#define __OYW_PERIODIC_WRITE_HPP__

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Port.h>

namespace roboticslab
{

class OpenraveYarpWorld;

/**
 * @ingroup OpenraveYarpWorld
 * @brief Publishes bodies.
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

} // namespace roboticslab

#endif // __OYW_PERIODIC_WRITE_HPP__
