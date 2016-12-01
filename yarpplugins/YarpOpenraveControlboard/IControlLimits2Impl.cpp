// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

// ------------------- IControlLimits Related ------------------------------------

bool teo::YarpOpenraveControlboard::setLimits(int axis, double min, double max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getLimits(int axis, double *min, double *max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::setVelLimits(int axis, double min, double max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getVelLimits(int axis, double *min, double *max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------
