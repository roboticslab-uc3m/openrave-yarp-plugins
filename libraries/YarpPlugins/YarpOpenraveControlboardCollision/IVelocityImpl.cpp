// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboardCollision.hpp"

// ------------------ IVelocity Related ----------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::velocityMove(int j, double sp) {  // velExposed = sp;
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::velocityMove(const double *sp) {
    CD_INFO("\n");
    printf("[YarpOpenraveControlboardCollision] Vel:");
    CD_INFO("Doing nothing.");
    printf("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= velocityMove(i,sp[i]);
    return ok;
}

// ----------------------------------------------------------------------------
