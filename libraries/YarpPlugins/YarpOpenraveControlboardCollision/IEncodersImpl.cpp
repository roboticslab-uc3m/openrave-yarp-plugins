// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboardCollision.hpp"

// ------------------ IEncoders Related -----------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::resetEncoder(int j) {
    CD_INFO("\n");
    if ((unsigned int)j>axes) return false;
    return setEncoder(j,0.0);
  }

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::resetEncoders() {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= resetEncoder(i);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setEncoder(int j, double val) {  // encExposed = val;
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setEncoders(const double *vals) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setEncoder(i,vals[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getEncoder(int j, double *v) {
    //CD_INFO("\n");  //-- Way too verbose
    return iEncoders->getEncoder(j,v);
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getEncoders(double *encs) {
    CD_INFO("\n");
    return iEncoders->getEncoders(encs);
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getEncoderSpeed(int j, double *sp) {
    //CD_INFO("\n");  //-- Way too verbose
    // Make it easy, give the current reference speed.
    return iEncoders->getEncoderSpeed(j, sp);
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getEncoderSpeeds(double *spds) {
    CD_INFO("\n");
    return iEncoders->getEncoders(spds);
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getEncoderAcceleration(int j, double *spds) {
    //CD_INFO("\n");  //-- Way too verbose
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getEncoderAccelerations(double *accs) {
    CD_INFO("\n");
    return false;
}

// -----------------------------------------------------------------------------

