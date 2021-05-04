// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveSimulation.hpp"

#include <yarp/os/LogStream.h>

namespace roboticslab
{

// ------------------ ISimulation Related ----------------------------------------

bool YarpOpenraveSimulation::step(double value)
{
    yTrace() << value;
    penv->StepSimulation(value);
    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveSimulation::start(double value)
{
    yTrace() << value;
    penv->StartSimulation(value);
    return true;
}

// ----------------------------------------------------------------------------


bool YarpOpenraveSimulation::stop()
{
    yTrace();
    penv->StopSimulation();
    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveSimulation::getSimulationRawPointerValue(yarp::os::Value& value)
{
    yTrace() << value;
    yarp::os::Value v(&penv, sizeof(OpenRAVE::EnvironmentBasePtr));
    value = v;
    yDebug() << "penvValue:" << value.toString();
    return true;
}

// ----------------------------------------------------------------------------

}
