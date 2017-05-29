// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboardCollision.hpp"

// ------------------- IControlLimits Related ------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setLimits(int axis, double min, double max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getLimits(int axis, double *min, double *max)
{
    return iControlLimits2->getLimits(axis, min, max);
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setVelLimits(int axis, double min, double max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getVelLimits(int axis, double *min, double *max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------
