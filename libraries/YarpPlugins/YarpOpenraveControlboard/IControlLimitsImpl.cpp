// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <utility> // std::pair
#include <vector>

#include <yarp/os/LogStream.h>

// ------------------- IControlLimits Related ------------------------------------

bool roboticslab::YarpOpenraveControlboard::setLimits(int axis, double min, double max)
{
    std::vector<OpenRAVE::dReal> vLowerLimit;
    std::vector<OpenRAVE::dReal> vUpperLimit;

    //-- Our joints always have ony 1 DoF, therefore safe to use only [0].
    vLowerLimit.push_back( degToRadIfNotPrismatic(axis,min) );
    vUpperLimit.push_back( degToRadIfNotPrismatic(axis,max) );

    vectorOfJointPtr[axis]->SetLimits(vLowerLimit,vUpperLimit);

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getLimits(int axis, double *min, double *max)
{
    //-- At least via RPC, YARP already protects for out-of-bound queries.

    std::vector<OpenRAVE::dReal> vLowerLimit;
    std::vector<OpenRAVE::dReal> vUpperLimit;
    vectorOfJointPtr[axis]->GetLimits(vLowerLimit,vUpperLimit);

    //-- Our joints always have ony 1 DoF, therefore safe to use only [0].
    *min = radToDegIfNotPrismatic(axis,vLowerLimit[0]);
    *max = radToDegIfNotPrismatic(axis,vUpperLimit[0]);

    yInfo("Limits %d: [%f, %f]",axis,*min,*max);

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setVelLimits(int axis, double min, double max)
{
    yWarning() << "min not used, upstream hard-coded to -(max)";

    if( max < refSpeeds[ axis ] )
    {
        yWarning() << "Setting" << max << "maxVelLimit below" << refSpeeds[axis] <<"refSpeed, all joint" << axis << "movements will be immediate";
    }

    std::vector<OpenRAVE::dReal> vUpperLimitVel;
    vUpperLimitVel.push_back( degToRadIfNotPrismatic(axis,max) );

    vectorOfJointPtr[axis]->SetVelocityLimits(vUpperLimitVel);

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getVelLimits(int axis, double *min, double *max)
{
    std::pair<OpenRAVE::dReal, OpenRAVE::dReal> velLimits = vectorOfJointPtr[axis]->GetVelocityLimit();
    *min = radToDegIfNotPrismatic(axis, velLimits.first);  // This lower limit is in fact hard-coded upstream to -(max).
    *max = radToDegIfNotPrismatic(axis, velLimits.second);

    return true;
}

// -----------------------------------------------------------------------------
