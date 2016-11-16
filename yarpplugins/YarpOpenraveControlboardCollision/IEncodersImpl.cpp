// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboardCollision.hpp"

// ------------------ IEncoders Related -----------------------------------------

bool teo::YarpOpenraveControlboardCollision::resetEncoder(int j) {
    CD_INFO("\n");
    if ((unsigned int)j>axes) return false;
    return setEncoder(j,0.0);
  }

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::resetEncoders() {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= resetEncoder(i);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setEncoder(int j, double val) {  // encExposed = val;
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setEncoders(const double *vals) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setEncoder(i,vals[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getEncoder(int j, double *v) {
    //CD_INFO("\n");  //-- Way too verbose
    return iEncoders->getEncoder(j,v);
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getEncoders(double *encs) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getEncoder(i,&encs[i]);
    return iEncoders->getEncoders(encs);
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getEncoderSpeed(int j, double *sp) {
    //CD_INFO("\n");  //-- Way too verbose
    // Make it easy, give the current reference speed.
    *sp = 0;  // begins to look like we should use semaphores.
    return iEncoders->getEncoderSpeed(j, sp);
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getEncoderSpeeds(double *spds) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getEncoderSpeed(i,&spds[i]);
    return iEncoders->getEncoders(spds);
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getEncoderAcceleration(int j, double *spds) {
    //CD_INFO("\n");  //-- Way too verbose
    return false;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getEncoderAccelerations(double *accs) {
    CD_INFO("\n");
    return false;
}

// -----------------------------------------------------------------------------

