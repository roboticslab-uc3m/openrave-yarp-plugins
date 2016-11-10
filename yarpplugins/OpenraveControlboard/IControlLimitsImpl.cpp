// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveControlboard.hpp"

// ------------------- IControlLimits Related ------------------------------------

bool teo::OpenraveControlboard::setLimits(int axis, double min, double max) {
    CD_INFO("\n");
    if(axis>=int(axes)) return false;
    minLimit[axis] = min;
    maxLimit[axis] = max;
    printf("[OpenraveControlboard] Range of axis %d set to: %f to %f\n",axis,min,max);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getLimits(int axis, double *min, double *max) {
    CD_INFO("\n");
    if(axis>=int(axes)) return false;
    *min = minLimit[axis];
    *max = maxLimit[axis];
    printf("[OpenraveControlboard] Range of axis %d read: %f to %f.\n",axis,*min,*max);
    return true;
}

// -----------------------------------------------------------------------------

