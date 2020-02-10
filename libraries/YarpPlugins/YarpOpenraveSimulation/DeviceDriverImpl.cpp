// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveSimulation.hpp"

#include <string>
#include <vector>

#include <yarp/os/Value.h>

#include <ColorDebug.h>

namespace roboticslab
{

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveSimulation::open(yarp::os::Searchable& config)
{
    CD_DEBUG("config: %s\n",config.toString().c_str());

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
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab
