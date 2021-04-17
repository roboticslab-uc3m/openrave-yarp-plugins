// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <vector>

#include <ColorDebug.h>

// ------------------- IPositionDirect Related --------------------------------

bool roboticslab::YarpOpenraveControlboard::setPosition(int j, double ref)
{
    //CD_INFO("\n");

    //-- Check if we are in positionDirect mode.
    if( controlModes[j] != VOCAB_CM_POSITION_DIRECT )
    {
        CD_ERROR("Will not positionMove as not in positionDirect mode\n");
        return false;
    }

    OpenRAVE::dReal dofTargetRads = degToRadIfNotPrismatic(j, ref);  // ref comes in exposed

    //-- Store the targets
    manipulatorTargetRads[ j ] = dofTargetRads;

    //-- Perform immediate movement (thread safe?)
    std::vector<OpenRAVE::dReal> tmp;
    tmp.push_back(dofTargetRads);
    pcontrols[j]->SetDesired(tmp);

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setPositions(const int n_joint, const int *joints, const double *refs)
{
    //CD_INFO("\n");
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= setPosition(joints[i],refs[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setPositions(const double *refs)
{
    //CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setPosition(i,refs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefPosition(const int joint, double *ref)
{
    //CD_INFO("\n");

    //-- Check if we are in positionDirect mode.
    if( controlModes[joint] != VOCAB_CM_POSITION_DIRECT )
    {
        CD_ERROR("Not in positionDirect mode\n");
        return false;
    }

    *ref = radToDegIfNotPrismatic(joint, manipulatorTargetRads[ joint ]);
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefPositions(double *refs)
{
    //CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getRefPosition(i,&refs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefPositions(const int n_joint, const int *joints, double *refs)
{
    //CD_INFO("\n");
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= getRefPosition(joints[i],&refs[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------
