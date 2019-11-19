// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <ColorDebug.h>

// ------------------- IControlMode Related ------------------------------------

bool roboticslab::YarpOpenraveControlboard::getControlMode(int j, int *mode) {
    //CD_DEBUG("Doing nothing.\n");  //-- Way too verbose.

    *mode = controlModes[j];

    return true;
}

// -----------------------------------------------------------------------------


bool roboticslab::YarpOpenraveControlboard::getControlModes(int *modes) {
    //CD_DEBUG("\n");  //-- Way too verbose.
    bool ok = true;
    for(unsigned int i=0; i < axes; i++)
        ok &= getControlMode(i,&(modes[i]));
    return ok;
}

// ---------------------- IControlMode2 Related  ---------------------------------

bool roboticslab::YarpOpenraveControlboard::getControlModes(const int n_joint, const int *joints, int *modes)
{
    CD_DEBUG("\n");

    bool ok = true;
    for(unsigned int i=0; i < n_joint; i++)
        ok &= getControlMode(joints[i],&modes[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setControlMode(const int j, const int mode)
{
    CD_DEBUG("(%d, %d)\n",j,mode);

    controlModes[j] = mode;

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setControlModes(const int n_joint, const int *joints, int *modes)
{
    CD_DEBUG("(%d)\n",n_joint);

    bool ok = true;
    for(int j=0; j<n_joint; j++)
    {
        ok &= this->setControlMode(joints[j],modes[j]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setControlModes(int *modes)
{
    CD_DEBUG("\n");

    bool ok = true;
    for(unsigned int i=0; i<axes; i++)
        ok &= setControlMode(i,modes[i]);
    return ok;
}

// -----------------------------------------------------------------------------
