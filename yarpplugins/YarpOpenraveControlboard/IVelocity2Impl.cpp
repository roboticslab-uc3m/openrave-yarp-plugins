// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

// ------------------ IVelocity2 Related ----------------------------------------

bool teo::YarpOpenraveControlboard::velocityMove(const int n_joint, const int *joints, const double *spds)
{
    CD_DEBUG("\n");
    // must implement mask!
    return velocityMove(spds);
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getRefVelocity(const int joint, double *vel)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getRefVelocities(double *vels)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getRefVelocities(const int n_joint, const int *joints, double *vels)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::setVelPid(int j, const yarp::dev::Pid &pid)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::setVelPids(const yarp::dev::Pid *pids)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getVelPid(int j, yarp::dev::Pid *pid)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getVelPids(yarp::dev::Pid *pids)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

