// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <ColorDebug.h>

// ------------------- IForceControl Related ------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefTorques(double *t){
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefTorque(int j, double *t) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefTorques(const double *t) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefTorque(int j, double t) {
    CD_INFO("joint: %d, refTorque: %f.\n",j,t);
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefTorques(const int n_joint, const int *joints, const double *t) {
    CD_INFO("joint: %d.\n",n_joint);
    bool ok = true;
    for(int j=0; j<n_joint; j++)
    {
        ok &= this->setRefTorque(joints[j],t[j]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setMotorTorqueParams(int j, const yarp::dev::MotorTorqueParameters params) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTorque(int j, double *t) {
    //CD_INFO("joint: %d.\n",j);  //-- Way too verbose
    *t = 0;
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTorques(double *t) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTorqueRange(int j, double *min, double *max) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTorqueRanges(double *min, double *max) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_MAJOR != 3
bool roboticslab::YarpOpenraveControlboard::getBemfParam(int j, double *bemf) {
    CD_INFO("joint: %d.\n",j);

    yarp::dev::MotorTorqueParameters params;

    if (!getMotorTorqueParams(j, &params))
    {
        return false;
    }

    *bemf = params.bemf;

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setBemfParam(int j, double bemf) {
    CD_INFO("joint: %d, bemf: %f.\n",j,bemf);

    yarp::dev::MotorTorqueParameters params;

    if (!getMotorTorqueParams(j, &params))
    {
        return false;
    }

    params.bemf = bemf;

    return setMotorTorqueParams(j, params);
}

// -----------------------------------------------------------------------------
#endif // YARP_VERSION_MAJOR != 3
