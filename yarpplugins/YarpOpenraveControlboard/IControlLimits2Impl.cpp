// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

// ------------------- IControlLimits Related ------------------------------------

bool roboticslab::YarpOpenraveControlboard::setLimits(int axis, double min, double max) {

    std::vector<OpenRAVE::dReal> vLowerLimit;
    std::vector<OpenRAVE::dReal> vUpperLimit;

    //-- Our joints always have ony 1 DoF, therefore safe to use only [0].
    vLowerLimit.push_back(min*M_PI/180.0);
    vUpperLimit.push_back(max*M_PI/180.0);

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
    *min = vLowerLimit[0]*180.0/M_PI;
    *max = vUpperLimit[0]*180.0/M_PI;

    CD_INFO("Limits %d: [%f,%f]\n",axis,*min,*max);

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setVelLimits(int axis, double min, double max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getVelLimits(int axis, double *min, double *max) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------
