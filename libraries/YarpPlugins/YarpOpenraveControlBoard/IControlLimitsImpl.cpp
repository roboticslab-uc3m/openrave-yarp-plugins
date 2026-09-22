// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlBoard.hpp"

#include <utility> // std::pair
#include <vector>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------- IControlLimits Related ------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setPosLimits(int axis, double min, double max)
#else
bool YarpOpenraveControlBoard::setLimits(int axis, double min, double max)
#endif
{
    if (axis < 0 || (unsigned int)axis > axes)
    {
        yCError(YORCB) << "getAxisName: axis" << axis << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    std::vector<OpenRAVE::dReal> vLowerLimit;
    std::vector<OpenRAVE::dReal> vUpperLimit;

    //-- Our joints always have ony 1 DoF, therefore safe to use only [0].
    vLowerLimit.push_back(degToRadIfNotPrismatic(axis, min));
    vUpperLimit.push_back(degToRadIfNotPrismatic(axis, max));

    vectorOfJointPtr[axis]->SetLimits(vLowerLimit, vUpperLimit);

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getPosLimits(int axis, double * min, double * max)
#else
bool YarpOpenraveControlBoard::getLimits(int axis, double * min, double * max)
#endif
{
    if (axis < 0 || (unsigned int)axis > axes)
    {
        yCError(YORCB) << "getAxisName: axis" << axis << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    //-- At least via RPC, YARP already protects for out-of-bound queries.

    std::vector<OpenRAVE::dReal> vLowerLimit;
    std::vector<OpenRAVE::dReal> vUpperLimit;
    vectorOfJointPtr[axis]->GetLimits(vLowerLimit, vUpperLimit);

    //-- Our joints always have ony 1 DoF, therefore safe to use only [0].
    *min = radToDegIfNotPrismatic(axis, vLowerLimit[0]);
    *max = radToDegIfNotPrismatic(axis, vUpperLimit[0]);

    yCInfo(YORCB, "Limits %d: [%f, %f]", axis, *min, *max);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setVelLimits(int axis, double min, double max)
#else
bool YarpOpenraveControlBoard::setVelLimits(int axis, double min, double max)
#endif
{
    if (axis < 0 || (unsigned int)axis > axes)
    {
        yCError(YORCB) << "getAxisName: axis" << axis << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    yCWarning(YORCB) << "min not used, upstream hard-coded to -(max)";

    if (max < refSpeeds[axis])
    {
        yCWarning(YORCB) << "Setting" << max << "maxVelLimit below" << refSpeeds[axis] << "refSpeed, all joint" << axis << "movements will be immediate";
    }

    std::vector<OpenRAVE::dReal> vUpperLimitVel;
    vUpperLimitVel.push_back(degToRadIfNotPrismatic(axis, max));

    vectorOfJointPtr[axis]->SetVelocityLimits(vUpperLimitVel);

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getVelLimits(int axis, double * min, double * max)
#else
bool YarpOpenraveControlBoard::getVelLimits(int axis, double * min, double * max)
#endif
{
    if (axis < 0 || (unsigned int)axis > axes)
    {
        yCError(YORCB) << "getAxisName: axis" << axis << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    auto [_min, _max] = vectorOfJointPtr[axis]->GetVelocityLimit();
    *min = radToDegIfNotPrismatic(axis, _min); // This lower limit is in fact hard-coded upstream to -(max).
    *max = radToDegIfNotPrismatic(axis, _max);

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------
