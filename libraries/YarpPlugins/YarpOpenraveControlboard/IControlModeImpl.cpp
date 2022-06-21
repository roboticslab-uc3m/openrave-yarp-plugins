// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------- IControlMode Related ------------------------------------

bool YarpOpenraveControlboard::getControlMode(int j, int *mode)
{
    *mode = controlModes[j];
    return true;
}

// -----------------------------------------------------------------------------


bool YarpOpenraveControlboard::getControlModes(int *modes)
{
    bool ok = true;
    for (unsigned int i = 0; i < axes; i++)
        ok &= getControlMode(i, &(modes[i]));
    return ok;
}

// ---------------------- IControlMode2 Related  ---------------------------------

bool YarpOpenraveControlboard::getControlModes(const int n_joint, const int *joints, int *modes)
{
    yCTrace(YORCB) << n_joint;

    bool ok = true;
    for (unsigned int i = 0; i < n_joint; i++)
        ok &= getControlMode(joints[i], &modes[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::setControlMode(const int j, const int mode)
{
    yCTrace(YORCB) << j << mode;

    controlModes[j] = mode;

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::setControlModes(const int n_joint, const int *joints, int *modes)
{
    yCTrace(YORCB) << n_joint;

    bool ok = true;
    for (int j = 0; j < n_joint; j++)
    {
        ok &= this->setControlMode(joints[j], modes[j]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::setControlModes(int *modes)
{
    yCTrace(YORCB);

    bool ok = true;
    for (unsigned int i = 0; i < axes; i++)
        ok &= setControlMode(i, modes[i]);
    return ok;
}

// -----------------------------------------------------------------------------
