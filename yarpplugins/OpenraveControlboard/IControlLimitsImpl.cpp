// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveControlboard.hpp"

// ------------------- IControlLimits Related ------------------------------------

bool teo::OpenraveControlboard::setLimits(int axis, double min, double max) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getLimits(int axis, double *min, double *max) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------


