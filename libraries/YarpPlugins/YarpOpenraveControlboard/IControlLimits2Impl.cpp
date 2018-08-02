// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <utility> // std::pair
#include <vector>

#include <ColorDebug.h>

// ------------------- IControlLimits Related ------------------------------------

bool roboticslab::YarpOpenraveControlboard::setLimits(int axis, double min, double max) {

    std::vector<OpenRAVE::dReal> vLowerLimit;
    std::vector<OpenRAVE::dReal> vUpperLimit;

    //-- Our joints always have ony 1 DoF, therefore safe to use only [0].
    vLowerLimit.push_back( degToRadIfNotPrismatic(axis,min) );
    vUpperLimit.push_back( degToRadIfNotPrismatic(axis,max) );

    vectorOfJointPtr[axis]->SetLimits(vLowerLimit,vUpperLimit);

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getLimits(int axis, double *min, double *max) {

    //-- At least via RPC, YARP already protects for out-of-bound queries.

    std::vector<OpenRAVE::dReal> vLowerLimit;
    std::vector<OpenRAVE::dReal> vUpperLimit;
    vectorOfJointPtr[axis]->GetLimits(vLowerLimit,vUpperLimit);

    //-- Our joints always have ony 1 DoF, therefore safe to use only [0].
    *min = radToDegIfNotPrismatic(axis,vLowerLimit[0]);
    *max = radToDegIfNotPrismatic(axis,vUpperLimit[0]);

    CD_INFO("Limits %d: [%f, %f]\n",axis,*min,*max);

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setVelLimits(int axis, double min, double max) {
    CD_WARNING("min not used, upstream hard-coded to -(max)\n");

    if( max < refSpeeds[ axis ] )
    {
        CD_WARNING("Setting %f maxVelLimit below %f refSpeed. All joint %d movements will be immediate.\n",max,refSpeeds[ axis ],axis);
    }

    std::vector<OpenRAVE::dReal> vUpperLimitVel;
    vUpperLimitVel.push_back( degToRadIfNotPrismatic(axis,max) );

    vectorOfJointPtr[axis]->SetVelocityLimits(vUpperLimitVel);

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getVelLimits(int axis, double *min, double *max) {

    //*min = 0;  // This would be a lower limit hard-coded to 0.
    //*max = radToDegIfNotPrismatic(axis,vectorOfJointPtr[axis]->GetMaxVel());
    //CD_INFO("(GetMaxVel) Velocity Limits %d: [%f, %f]\n",axis,*min,*max);

    std::pair<OpenRAVE::dReal, OpenRAVE::dReal> velLimits = vectorOfJointPtr[axis]->GetVelocityLimit();
    *min = radToDegIfNotPrismatic(axis, velLimits.first);  // This lower limit is in fact hard-coded upstream to -(max).
    *max = radToDegIfNotPrismatic(axis, velLimits.second);

    //CD_INFO("(GetVelocityLimit) Velocity Limits %d: [%f, %f]\n",axis,*min,*max);

    return true;
}

// -----------------------------------------------------------------------------
