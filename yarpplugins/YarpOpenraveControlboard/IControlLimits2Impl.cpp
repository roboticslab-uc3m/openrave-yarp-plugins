// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

// ------------------- IControlLimits Related ------------------------------------

bool roboticslab::YarpOpenraveControlboard::setLimits(int axis, double min, double max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getLimits(int axis, double *min, double *max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setVelLimits(int axis, double min, double max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getVelLimits(int axis, double *min, double *max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------
