// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

// ------------------ IVelocity Related ----------------------------------------

bool roboticslab::YarpOpenraveControlboard::velocityMove(int j, double sp) {  // velExposed = sp;

    //-- Check if we are in position mode.
    if( controlModes[j] != VOCAB_CM_VELOCITY )
    {
        CD_ERROR("Will not velocityMove as not in velocityMode\n");
        return false;
    }

    std::stringstream sout,ss; ss << "setvelocity ";
    for(size_t i = 0; i < manipulatorIDs.size(); ++i) {
        ss << sp << " ";
    }

    if( ! pcontrol->SendCommand(sout,ss) ) {
        CD_ERROR("Failed to send velocity command\n");
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::velocityMove(const double *sp) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= velocityMove(i,sp[i]);
    return ok;
}

// ----------------------------------------------------------------------------

