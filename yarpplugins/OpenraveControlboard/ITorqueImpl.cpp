// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "FakeControlboardOR.hpp"

// ------------------- IForceControl Related ------------------------------------


bool teo::FakeControlboardOR::setTorqueMode() {
    CD_INFO("setTorqueMode()\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getRefTorques(double *t){
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getRefTorque(int j, double *t) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::setRefTorques(const double *t) {
    CD_INFO("\n");
    return true;
}


// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::setRefTorque(int j, double t) {
    CD_INFO("joint: %d, refTorque: %f.\n",j,t);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getBemfParam(int j, double *bemf) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::setBemfParam(int j, double bemf) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::setTorquePid(int j, const yarp::dev::Pid &pid) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getTorque(int j, double *t) {
    //CD_INFO("joint: %d.\n",j);  //-- Way too verbose
    *t = 0;
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getTorques(double *t) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getTorqueRange(int j, double *min, double *max) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getTorqueRanges(double *min, double *max) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::setTorquePids(const yarp::dev::Pid *pids) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::setTorqueErrorLimit(int j, double limit) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::setTorqueErrorLimits(const double *limits) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getTorqueError(int j, double *err) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getTorqueErrors(double *errs) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getTorquePidOutput(int j, double *out) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getTorquePidOutputs(double *outs) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getTorquePid(int j, yarp::dev::Pid *pid) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getTorquePids(yarp::dev::Pid *pids){
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getTorqueErrorLimit(int j, double *limit) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::getTorqueErrorLimits(double *limits) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::resetTorquePid(int j) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::disableTorquePid(int j) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::enableTorquePid(int j) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::setTorqueOffset(int j, double v) {
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------
