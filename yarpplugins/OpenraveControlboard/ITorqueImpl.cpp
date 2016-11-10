// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveControlboard.hpp"

// ------------------- IForceControl Related ------------------------------------


bool teo::OpenraveControlboard::setTorqueMode() {
    CD_INFO("setTorqueMode()\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getRefTorques(double *t){
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getRefTorque(int j, double *t) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setRefTorques(const double *t) {
    CD_INFO("\n");
    return true;
}


// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setRefTorque(int j, double t) {
    CD_INFO("joint: %d, refTorque: %f.\n",j,t);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getBemfParam(int j, double *bemf) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setBemfParam(int j, double bemf) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setTorquePid(int j, const yarp::dev::Pid &pid) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTorque(int j, double *t) {
    //CD_INFO("joint: %d.\n",j);  //-- Way too verbose
    *t = 0;
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTorques(double *t) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTorqueRange(int j, double *min, double *max) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTorqueRanges(double *min, double *max) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setTorquePids(const yarp::dev::Pid *pids) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setTorqueErrorLimit(int j, double limit) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setTorqueErrorLimits(const double *limits) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTorqueError(int j, double *err) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTorqueErrors(double *errs) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTorquePidOutput(int j, double *out) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTorquePidOutputs(double *outs) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTorquePid(int j, yarp::dev::Pid *pid) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTorquePids(yarp::dev::Pid *pids){
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTorqueErrorLimit(int j, double *limit) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getTorqueErrorLimits(double *limits) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::resetTorquePid(int j) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::disableTorquePid(int j) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::enableTorquePid(int j) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setTorqueOffset(int j, double v) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------
