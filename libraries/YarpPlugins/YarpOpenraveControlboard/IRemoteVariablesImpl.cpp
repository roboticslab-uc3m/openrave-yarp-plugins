// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <limits>

#include <yarp/os/LogStream.h>

// ------------------- IRemoteVariables Related ------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRemoteVariable(std::string key, yarp::os::Bottle& val)
{
    yTrace() << key;
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRemoteVariable(std::string key, const yarp::os::Bottle& val)
{
    yTrace() << key << val;
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRemoteVariablesList(yarp::os::Bottle* listOfKeys)
{
    yTrace();
    listOfKeys->clear();
    // Place each key in its own list so that clients can just call check('<key>') or !find('<key>').isNull().
    return true;
}

// -----------------------------------------------------------------------------
