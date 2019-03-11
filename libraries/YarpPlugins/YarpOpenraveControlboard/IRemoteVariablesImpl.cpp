// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <limits>

#include <ColorDebug.h>

// ------------------- IRemoteVariables Related ------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRemoteVariable(std::string key, yarp::os::Bottle& val)
{
    CD_DEBUG("%s\n", key.c_str());

    val.clear();

    if (key == "ptModeMs")
    {
        val.addInt(std::numeric_limits<int>::max());
    }

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRemoteVariable(std::string key, const yarp::os::Bottle& val)
{
    CD_DEBUG("%s\n", key.c_str());
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRemoteVariablesList(yarp::os::Bottle* listOfKeys)
{
    CD_DEBUG("\n");
    listOfKeys->clear();
    listOfKeys->addString("ptModeMs");
    return true;
}

// -----------------------------------------------------------------------------
