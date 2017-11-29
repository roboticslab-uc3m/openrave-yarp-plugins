// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

// ------------------- IPositionControl2 Related --------------------------------

bool roboticslab::YarpOpenraveControlboard::positionMove(const int n_joint, const int *joints, const double *refs)
{
    CD_INFO("\n");
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= positionMove(joints[i],refs[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::relativeMove(const int n_joint, const int *joints, const double *deltas)
{
    CD_INFO("\n");
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= relativeMove(joints[i],deltas[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::checkMotionDone(const int n_joint, const int *joints, bool *flags)
{
    CD_ERROR("Not implemented yet.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefSpeeds(const int n_joint, const int *joints, const double *spds)
{
    CD_INFO("\n");
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= setRefSpeed(joints[i],spds[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
    CD_INFO("\n");
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= setRefAcceleration(joints[i],accs[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefSpeeds(const int n_joint, const int *joints, double *spds)
{
    CD_ERROR("Not implemented yet.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    CD_ERROR("Not implemented yet.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::stop(const int n_joint, const int *joints)
{
    CD_ERROR("Not implemented yet.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTargetPosition(const int joint, double *ref)
{
    CD_ERROR("Not implemented yet.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTargetPositions(double *refs)
{
    CD_ERROR("Not implemented yet.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTargetPositions(const int n_joint, const int *joints, double *refs)
{
    CD_ERROR("Not implemented yet.\n");
    return true;
}

// -----------------------------------------------------------------------------
