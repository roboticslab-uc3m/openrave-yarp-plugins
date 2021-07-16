// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveSimulation.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------ ISimulation Related ----------------------------------------

bool YarpOpenraveSimulation::step(double value)
{
    yCTrace(YORS) << value;
    penv->StepSimulation(value);
    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveSimulation::start(double value)
{
    yCTrace(YORS) << value;
    penv->StartSimulation(value);
    return true;
}

// ----------------------------------------------------------------------------


bool YarpOpenraveSimulation::stop()
{
    yCTrace(YORS);
    penv->StopSimulation();
    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveSimulation::getSimulationRawPointerValue(yarp::os::Value& value)
{
    yCTrace(YORS) << value.toString();
    yarp::os::Value v(&penv, sizeof(OpenRAVE::EnvironmentBasePtr));
    value = v;
    return true;
}

// ----------------------------------------------------------------------------
