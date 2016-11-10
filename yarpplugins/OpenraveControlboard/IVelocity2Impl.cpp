// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveControlboard.hpp"

// ------------------ IVelocity2 Related ----------------------------------------

bool teo::OpenraveControlboard::velocityMove(const int n_joint, const int *joints, const double *spds)
{
    CD_DEBUG("\n");
    // must implement mask!
    return velocityMove(spds);
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getRefVelocity(const int joint, double *vel)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getRefVelocities(double *vels)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getRefVelocities(const int n_joint, const int *joints, double *vels)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setVelPid(int j, const yarp::dev::Pid &pid)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setVelPids(const yarp::dev::Pid *pids)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getVelPid(int j, yarp::dev::Pid *pid)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getVelPids(yarp::dev::Pid *pids)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

