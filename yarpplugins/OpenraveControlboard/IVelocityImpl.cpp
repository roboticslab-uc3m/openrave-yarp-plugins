// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveControlboard.hpp"

// ------------------ IVelocity Related ----------------------------------------

bool teo::OpenraveControlboard::setVelocityMode() {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::velocityMove(int j, double sp) {  // velExposed = sp;
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::velocityMove(const double *sp) {
    CD_INFO("\n");
    printf("[OpenraveControlboard] Vel:");
    CD_INFO("NOTHING TO DO");
    printf("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= velocityMove(i,sp[i]);
    return ok;
}

// ----------------------------------------------------------------------------

