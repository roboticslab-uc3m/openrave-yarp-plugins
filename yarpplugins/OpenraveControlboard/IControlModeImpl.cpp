// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveControlboard.hpp"

// ------------------- IControlLimits Related ------------------------------------

bool teo::OpenraveControlboard::setPositionMode(int j) {
    CD_INFO("(%d)\n",j);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setVelocityMode(int j) {
    CD_INFO("(%d)\n",j);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setTorqueMode(int j)  {
    CD_INFO("(%d)\n",j);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setImpedancePositionMode(int j) {
    CD_INFO("(%d)\n",j);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setImpedanceVelocityMode(int j) {
    CD_INFO("(%d)\n",j);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setOpenLoopMode(int j) {
    CD_INFO("(%d)\n",j);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getControlMode(int j, int *mode) {
    //CD_DEBUG("NOTHING TO DO\n");  //-- Way too verbose.
    return true;
}

// -----------------------------------------------------------------------------


bool teo::OpenraveControlboard::getControlModes(int *modes) {
    CD_DEBUG("\n");
    bool ok = true;
    for(unsigned int i=0; i < axes; i++)
        ok &= getControlMode(i,&(modes[i]));
    return ok;
}

// -----------------------------------------------------------------------------
