// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveSimulation.hpp"

#include <ColorDebug.h>

namespace roboticslab
{

// ------------------ ISimulation Related ----------------------------------------

bool YarpOpenraveSimulation::step(double value)
{
    CD_DEBUG("\n");
    penv->StepSimulation(value);
    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveSimulation::start(double value)
{
    CD_DEBUG("\n");
    penv->StartSimulation(value);
    return true;
}

// ----------------------------------------------------------------------------


bool YarpOpenraveSimulation::stop()
{
    CD_DEBUG("\n");
    penv->StopSimulation();
    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveSimulation::getSimulationRawPointerValue(yarp::os::Value& value)
{
    CD_DEBUG("\n");
    yarp::os::Value v(&penv, sizeof(OpenRAVE::EnvironmentBasePtr));
    value = v;
    CD_DEBUG("penvValue: %s\n", value.toString().c_str());
    return true;
}

// ----------------------------------------------------------------------------

}
