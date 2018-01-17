// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboardCollision.hpp"

// ------------------ IVelocity2 Related ----------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::velocityMove(const int n_joint, const int *joints, const double *spds)
{
    CD_DEBUG("\n");
    // must implement mask!
    return velocityMove(spds);
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getRefVelocity(const int joint, double *vel)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getRefVelocities(double *vels)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getRefVelocities(const int n_joint, const int *joints, double *vels)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setVelPid(int j, const yarp::dev::Pid &pid)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setVelPids(const yarp::dev::Pid *pids)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getVelPid(int j, yarp::dev::Pid *pid)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getVelPids(yarp::dev::Pid *pids)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

