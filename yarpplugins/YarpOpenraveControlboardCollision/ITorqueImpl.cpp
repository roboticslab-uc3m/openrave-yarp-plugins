// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboardCollision.hpp"

// ------------------- IForceControl Related ------------------------------------


bool teo::YarpOpenraveControlboardCollision::setTorqueMode() {
    CD_INFO("setTorqueMode()\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getRefTorques(double *t){
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getRefTorque(int j, double *t) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setRefTorques(const double *t) {
    CD_INFO("\n");
    return true;
}


// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setRefTorque(int j, double t) {
    CD_INFO("joint: %d, refTorque: %f.\n",j,t);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getBemfParam(int j, double *bemf) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setBemfParam(int j, double bemf) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setTorquePid(int j, const yarp::dev::Pid &pid) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getTorque(int j, double *t) {
    //CD_INFO("joint: %d.\n",j);  //-- Way too verbose
    *t = 0;
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getTorques(double *t) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getTorqueRange(int j, double *min, double *max) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getTorqueRanges(double *min, double *max) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setTorquePids(const yarp::dev::Pid *pids) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setTorqueErrorLimit(int j, double limit) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setTorqueErrorLimits(const double *limits) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getTorqueError(int j, double *err) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getTorqueErrors(double *errs) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getTorquePidOutput(int j, double *out) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getTorquePidOutputs(double *outs) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getTorquePid(int j, yarp::dev::Pid *pid) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getTorquePids(yarp::dev::Pid *pids){
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getTorqueErrorLimit(int j, double *limit) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getTorqueErrorLimits(double *limits) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::resetTorquePid(int j) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::disableTorquePid(int j) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::enableTorquePid(int j) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setTorqueOffset(int j, double v) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------
