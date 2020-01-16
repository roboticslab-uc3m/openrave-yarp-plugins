// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Bottle.h>

#include "OpenraveYarpWorld.hpp"

#include "OywPeriodicWrite.hpp"

// -----------------------------------------------------------------------------

OywPeriodicWrite::OywPeriodicWrite() : yarp::os::PeriodicThread(1.0)
{
    PeriodicThread::start();
}

// -----------------------------------------------------------------------------

void OywPeriodicWrite::run()
{
    if(!Port::isOpen())
        return;

    yarp::os::Bottle info;
    openraveYarpWorldPtr->addWorldInfo(info);

    if(0 == info.size())
        return;

    Port::write(info);
}

// -----------------------------------------------------------------------------
