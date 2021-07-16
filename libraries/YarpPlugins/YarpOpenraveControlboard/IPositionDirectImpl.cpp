// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <vector>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------- IPositionDirect Related --------------------------------

bool YarpOpenraveControlboard::setPosition(int j, double ref)
{
    yCTrace(YORCB) << j << ref;

    //-- Check if we are in positionDirect mode.
    if( controlModes[j] != VOCAB_CM_POSITION_DIRECT )
    {
        yCError(YORCB) << "Will not setPosition() as joint" << j << "not in positionDirect mode";
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

bool YarpOpenraveControlboard::setPositions(const int n_joint, const int *joints, const double *refs)
{
    yCTrace(YORCB) << n_joint;
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= setPosition(joints[i],refs[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::setPositions(const double *refs)
{
    yCTrace(YORCB);
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setPosition(i,refs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::getRefPosition(const int joint, double *ref)
{
    yCTrace(YORCB) << joint;

    //-- Check if we are in positionDirect mode.
    if( controlModes[joint] != VOCAB_CM_POSITION_DIRECT )
    {
        yCError(YORCB) << "Not in positionDirect mode at joint" << joint;
        return false;
    }

    *ref = radToDegIfNotPrismatic(joint, manipulatorTargetRads[ joint ]);
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::getRefPositions(double *refs)
{
    yCTrace(YORCB);
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getRefPosition(i,&refs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::getRefPositions(const int n_joint, const int *joints, double *refs)
{
    yCTrace(YORCB) << n_joint;
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= getRefPosition(joints[i],&refs[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------
