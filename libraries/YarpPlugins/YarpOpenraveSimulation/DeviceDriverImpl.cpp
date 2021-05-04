// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveSimulation.hpp"

#include <string>
#include <vector>

#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

namespace roboticslab
{

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveSimulation::open(yarp::os::Searchable& config)
{
    yDebug() << "YarpOpenraveSimulation config:" << config.toString();

    if ( ! configureEnvironment(config) )
        return false;

    if ( ! configureOpenravePlugins(config) )
        return false;

    if ( ! configureRobot(config) )
        return false;

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveSimulation::close()
{
    return true;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab
