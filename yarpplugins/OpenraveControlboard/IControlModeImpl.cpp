// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveControlboard.hpp"

// ------------------- IControlLimits Related ------------------------------------

bool teo::OpenraveControlboard::setPositionMode(int j) {
    CD_INFO("(%d)\n",j);
    modePosVel = 0;
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setVelocityMode(int j) {
    CD_INFO("(%d)\n",j);
    modePosVel = 1;  // Set flag to vel.
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
    //CD_DEBUG("\n");  //-- Way too verbose.
    if(modePosVel == 0)
        *mode = VOCAB_CM_POSITION;
    else if (modePosVel == 1)
        *mode = VOCAB_CM_VELOCITY;
    else
    {
        CD_ERROR("Currently unsupported mode.\n");
        return false;
    }
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
