// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboardCollision.hpp"

// ------------------- IControlLimits Related ------------------------------------

bool teo::YarpOpenraveControlboardCollision::setLimits(int axis, double min, double max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getLimits(int axis, double *min, double *max)
{
    return iControlLimits2->getLimits(axis, min, max);
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setVelLimits(int axis, double min, double max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getVelLimits(int axis, double *min, double *max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------
