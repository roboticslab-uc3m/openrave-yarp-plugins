// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlBoard.hpp"

#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------ IEncoders Related -----------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::resetEncoder(int j)
#else
bool YarpOpenraveControlBoard::resetEncoder(int j)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "resetEncoder: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    return setEncoder(j, 0.0);
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::resetEncoders()
#else
bool YarpOpenraveControlBoard::resetEncoders()
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
        ok &= resetEncoder(i);
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setEncoder(int j, double val)
#else
bool YarpOpenraveControlBoard::setEncoder(int j, double val)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "setEncoder: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setEncoders(const double * vals)
#else
bool YarpOpenraveControlBoard::setEncoders(const double * vals)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
        ok &= setEncoder(i, vals[i]);
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getEncoder(int j, double * v)
#else
bool YarpOpenraveControlBoard::getEncoder(int j, double * v)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "getEncoder: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    *v = radToDegIfNotPrismatic(j, vectorOfJointPtr[j]->GetValue(0));

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getEncoders(double * encs)
#else
bool YarpOpenraveControlBoard::getEncoders(double * encs)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
        ok &= getEncoder(i, &encs[i]);
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getEncoderSpeed(int j, double * sp)
#else
bool YarpOpenraveControlBoard::getEncoderSpeed(int j, double * sp)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "getEncoderSpeed: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    // Make it easy, give the current reference speed.
    *sp = 0;  // begins to look like we should use semaphores.
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getEncoderSpeeds(double * spds)
#else
bool YarpOpenraveControlBoard::getEncoderSpeeds(double * spds)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
        ok &= getEncoderSpeed(i, &spds[i]);
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getEncoderAcceleration(int j, double * spds)
#else
bool YarpOpenraveControlBoard::getEncoderAcceleration(int j, double * spds)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "getEncoderAcceleration: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    // Make it easy, give the current reference acceleration.
    *spds = 0;  // begins to look like we should use semaphores.
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getEncoderAccelerations(double * accs)
#else
bool YarpOpenraveControlBoard::getEncoderAccelerations(double * accs)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
        ok &= getEncoderAcceleration(i, &accs[i]);
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getEncodersTimed(double * encs, double * time)
#else
bool YarpOpenraveControlBoard::getEncodersTimed(double * encs, double * time)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
        ok &= getEncoderTimed(i, &(encs[i]), &(time[i]));
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getEncoderTimed(int j, double * encs, double * time)
#else
bool YarpOpenraveControlBoard::getEncoderTimed(int j, double * encs, double * time)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "getEncoderTimed: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    getEncoder(j, encs);
    *time = yarp::os::Time::now();

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------
