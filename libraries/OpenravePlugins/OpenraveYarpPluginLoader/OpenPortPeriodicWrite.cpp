// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Bottle.h>

#include "OpenPortPeriodicWrite.hpp"
#include "OpenraveYarpPluginLoader.hpp"

// -----------------------------------------------------------------------------

OpenPortPeriodicWrite::OpenPortPeriodicWrite() : yarp::os::PeriodicThread(1.0)
{
    PeriodicThread::start();
}

// -----------------------------------------------------------------------------

void OpenPortPeriodicWrite::run()
{
    if(!Port::isOpen())
        return;

    yarp::os::Bottle info;
    openraveYarpPluginLoaderPtr->addYarpPluginsLists(info);

    if(0 == info.size())
        return;

    Port::write(info);
}

// -----------------------------------------------------------------------------
