// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboardCollision.hpp"

// ------------------- IForceControl Related ------------------------------------


bool roboticslab::YarpOpenraveControlboardCollision::getRefTorques(double *t){
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getRefTorque(int j, double *t) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setRefTorques(const double *t) {
    CD_INFO("\n");
    return true;
}


// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setRefTorque(int j, double t) {
    CD_INFO("joint: %d, refTorque: %f.\n",j,t);
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getBemfParam(int j, double *bemf) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setBemfParam(int j, double bemf) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setTorquePid(int j, const yarp::dev::Pid &pid) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getTorque(int j, double *t) {
    //CD_INFO("joint: %d.\n",j);  //-- Way too verbose
    *t = 0;
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getTorques(double *t) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getTorqueRange(int j, double *min, double *max) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getTorqueRanges(double *min, double *max) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setTorquePids(const yarp::dev::Pid *pids) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setTorqueErrorLimit(int j, double limit) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setTorqueErrorLimits(const double *limits) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getTorqueError(int j, double *err) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getTorqueErrors(double *errs) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getTorquePidOutput(int j, double *out) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getTorquePidOutputs(double *outs) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getTorquePid(int j, yarp::dev::Pid *pid) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getTorquePids(yarp::dev::Pid *pids){
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getTorqueErrorLimit(int j, double *limit) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getTorqueErrorLimits(double *limits) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::resetTorquePid(int j) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::disableTorquePid(int j) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::enableTorquePid(int j) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setTorqueOffset(int j, double v) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------
