// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <yarp/os/LogStream.h>

// ------------------ IEncoders Related -----------------------------------------

bool roboticslab::YarpOpenraveControlboard::resetEncoder(int j)
{
    yTrace() << j;
    if ((unsigned int)j>axes) return false;
    return setEncoder(j,0.0);
  }

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::resetEncoders()
{
    yTrace();
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= resetEncoder(i);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setEncoder(int j, double val)
{
    yTrace() << j << val;
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setEncoders(const double *vals)
{
    yTrace();
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setEncoder(i,vals[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getEncoder(int j, double *v)
{
    yTrace() << j;
    *v = radToDegIfNotPrismatic(j, vectorOfJointPtr[j]->GetValue(0) );

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getEncoders(double *encs)
{
    yTrace();
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getEncoder(i,&encs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getEncoderSpeed(int j, double *sp)
{
    yTrace() << j;
    // Make it easy, give the current reference speed.
    *sp = 0;  // begins to look like we should use semaphores.
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getEncoderSpeeds(double *spds)
{
    yTrace();
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getEncoderSpeed(i,&spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getEncoderAcceleration(int j, double *spds)
{
    yError() << "getEncoderAcceleration() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getEncoderAccelerations(double *accs)
{
    //yError() << "getEncoderAccelerations() not implemented"; // too verbose
    return false;
}

// -----------------------------------------------------------------------------
