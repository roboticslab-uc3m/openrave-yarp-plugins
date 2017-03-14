// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

// ------------------- IPositionControl2 Related --------------------------------

bool teo::YarpOpenraveControlboard::positionMove(const int n_joint, const int *joints, const double *refs)
{
    CD_DEBUG("\n");
    // must implement mask!
    return positionMove(refs);
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::relativeMove(const int n_joint, const int *joints, const double *deltas)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::checkMotionDone(const int n_joint, const int *joints, bool *flags)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::setRefSpeeds(const int n_joint, const int *joints, const double *spds)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getRefSpeeds(const int n_joint, const int *joints, double *spds)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::stop(const int n_joint, const int *joints)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getTargetPosition(const int joint, double *ref)
{
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getTargetPositions(double *refs)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getTargetPositions(const int n_joint, const int *joints, double *refs)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------
