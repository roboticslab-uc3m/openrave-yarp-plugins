// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "FakeControlboard.hpp"

// ------------------- IControlLimits Related ------------------------------------

bool teo::FakeControlboard::setPositionMode(int j) {
    CD_INFO("(%d)\n",j);
    if (modePosVel==0) return true;  // Simply return true if we were already in pos mode.
    // Do anything additional before setting flag to pos...
    if(!stop(j)) {
        fprintf(stderr,"[FakeControlboard] warning: setPositionMode() return false; failed to stop joint %d\n",j);
        return false;
    }
    modePosVel = 0;
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboard::setVelocityMode(int j) {
    CD_INFO("(%d)\n",j);
    modePosVel = 1;  // Set flag to vel.
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboard::setTorqueMode(int j)  {
    CD_INFO("(%d)\n",j);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboard::setImpedancePositionMode(int j) {
    CD_INFO("(%d)\n",j);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboard::setImpedanceVelocityMode(int j) {
    CD_INFO("(%d)\n",j);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboard::setOpenLoopMode(int j) {
    CD_INFO("(%d)\n",j);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboard::getControlMode(int j, int *mode) {
    // CD_DEBUG("\n");  //-- Way too verbose.
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


bool teo::FakeControlboard::getControlModes(int *modes) {
    CD_DEBUG("\n");
    bool ok = true;
    for(unsigned int i=0; i < axes; i++)
        ok &= getControlMode(i,&(modes[i]));
    return ok;
}

// -----------------------------------------------------------------------------
