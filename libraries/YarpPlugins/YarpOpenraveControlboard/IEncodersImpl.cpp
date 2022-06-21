// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------ IEncoders Related -----------------------------------------

bool YarpOpenraveControlboard::resetEncoder(int j)
{
    yCTrace(YORCB) << j;
    if ((unsigned int)j > axes) return false;
    return setEncoder(j, 0.0);
  }

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::resetEncoders()
{
    yCTrace(YORCB);
    bool ok = true;
    for (unsigned int i = 0; i < axes; i++)
        ok &= resetEncoder(i);
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::setEncoder(int j, double val)
{
    yCTrace(YORCB) << j << val;
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::setEncoders(const double *vals)
{
    yCTrace(YORCB);
    bool ok = true;
    for (unsigned int i = 0; i < axes; i++)
        ok &= setEncoder(i, vals[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::getEncoder(int j, double *v)
{
    yCTrace(YORCB) << j;
    *v = radToDegIfNotPrismatic(j, vectorOfJointPtr[j]->GetValue(0));

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::getEncoders(double *encs)
{
    yCTrace(YORCB);
    bool ok = true;
    for (unsigned int i = 0; i < axes; i++)
        ok &= getEncoder(i, &encs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::getEncoderSpeed(int j, double *sp)
{
    yCTrace(YORCB) << j;
    // Make it easy, give the current reference speed.
    *sp = 0;  // begins to look like we should use semaphores.
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::getEncoderSpeeds(double *spds)
{
    yCTrace(YORCB);
    bool ok = true;
    for (unsigned int i = 0; i < axes; i++)
        ok &= getEncoderSpeed(i, &spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::getEncoderAcceleration(int j, double *spds)
{
    yCError(YORCB) << "getEncoderAcceleration() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::getEncoderAccelerations(double *accs)
{
    //yCError(YORCB) << "getEncoderAccelerations() not implemented"; // too verbose
    return false;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::getEncodersTimed(double *encs, double *time)
{
    yCTrace(YORCB);
    bool ok = true;
    for (unsigned int i = 0; i < axes; i++)
        ok &= getEncoderTimed(i, &(encs[i]), &(time[i]));
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::getEncoderTimed(int j, double *encs, double *time)
{
    yCTrace(YORCB) << j;

    getEncoder(j, encs);
    *time = yarp::os::Time::now();

    return true;
}

// -----------------------------------------------------------------------------
