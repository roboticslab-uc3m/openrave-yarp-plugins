// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

// ------------------ IEncoders Related -----------------------------------------

bool teo::YarpOpenraveControlboard::resetEncoder(int j) {
    CD_INFO("\n");
    if ((unsigned int)j>axes) return false;
    return setEncoder(j,0.0);
  }

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::resetEncoders() {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= resetEncoder(i);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::setEncoder(int j, double val) {  // encExposed = val;
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::setEncoders(const double *vals) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setEncoder(i,vals[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getEncoder(int j, double *v) {
    //CD_INFO("\n");  //-- Way too verbose
    *v = 0;
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getEncoders(double *encs) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getEncoder(i,&encs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getEncoderSpeed(int j, double *sp) {
    //CD_INFO("\n");  //-- Way too verbose
    // Make it easy, give the current reference speed.
    *sp = 0;  // begins to look like we should use semaphores.
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getEncoderSpeeds(double *spds) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getEncoderSpeed(i,&spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getEncoderAcceleration(int j, double *spds) {
    //CD_INFO("\n");  //-- Way too verbose
    return false;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboard::getEncoderAccelerations(double *accs) {
    CD_INFO("\n");
    return false;
}

// -----------------------------------------------------------------------------

