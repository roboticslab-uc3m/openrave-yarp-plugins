// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <yarp/os/LogStream.h>

// ------------------- IForceControl Related ------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefTorques(double *t)
{
    yError() << "getRefTorques() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefTorque(int j, double *t)
{
    yError() << "getRefTorque() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefTorques(const double *t)
{
    yError() << "setRefTorques() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefTorque(int j, double t)
{
    yError() << "setRefTorque() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefTorques(const int n_joint, const int *joints, const double *t)
{
    yError() << "setRefTorques() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params)
{
    yError() << "getMotorTorqueParams() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setMotorTorqueParams(int j, const yarp::dev::MotorTorqueParameters params)
{
    yError() << "setMotorTorqueParams() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTorque(int j, double *t)
{
    yError() << "getTorque() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTorques(double *t)
{
    yError() << "getTorques() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTorqueRange(int j, double *min, double *max)
{
    yError() << "getTorqueRange() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTorqueRanges(double *min, double *max)
{
    yError() << "getTorqueRanges() not implemented";
    return false;
}

// -----------------------------------------------------------------------------
