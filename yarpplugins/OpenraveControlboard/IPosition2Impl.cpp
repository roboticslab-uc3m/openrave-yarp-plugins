// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveControlboard.hpp"

// ------------------- IPositionControl2 Related --------------------------------

bool teo::OpenraveControlboard::positionMove(const int n_joint, const int *joints, const double *refs)
{
    CD_DEBUG("\n");
    // must implement mask!
    return positionMove(refs);
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::relativeMove(const int n_joint, const int *joints, const double *deltas)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::checkMotionDone(const int n_joint, const int *joints, bool *flags)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setRefSpeeds(const int n_joint, const int *joints, const double *spds)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getRefSpeeds(const int n_joint, const int *joints, double *spds)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::stop(const int n_joint, const int *joints)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTargetPosition(const int joint, double *ref)
{
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTargetPositions(double *refs)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTargetPositions(const int n_joint, const int *joints, double *refs)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------
