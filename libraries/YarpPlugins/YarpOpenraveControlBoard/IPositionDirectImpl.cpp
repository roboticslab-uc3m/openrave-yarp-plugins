// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlBoard.hpp"

#include <vector>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------- IPositionDirect Related --------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setPosition(int j, double ref)
#else
bool YarpOpenraveControlBoard::setPosition(int j, double ref)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "setPosition: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    //-- Check if we are in positionDirect mode.
    if (controlModes[j] != VOCAB_CM_POSITION_DIRECT)
    {
        yCError(YORCB) << "Will not setPosition() as joint" << j << "not in positionDirect mode";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
#else
        return false;
#endif
    }

    OpenRAVE::dReal dofTargetRads = degToRadIfNotPrismatic(j, ref);  // ref comes in exposed

    //-- Store the targets
    manipulatorTargetRads[j] = dofTargetRads;

    //-- Perform immediate movement (thread safe?)
    std::vector<OpenRAVE::dReal> tmp;
    tmp.push_back(dofTargetRads);
    pcontrols[j]->SetDesired(tmp);

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setPositions(const double * refs)
#else
bool YarpOpenraveControlBoard::setPositions(const double * refs)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
        ok &= setPosition(i, refs[i]);
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
yarp::dev::ReturnValue YarpOpenraveControlBoard::setPositions(int n_joint, const int * joints, const double * refs)
#else
bool YarpOpenraveControlBoard::setPositions(int n_joint, const int * joints, const double * refs)
#endif
{
    bool ok = true;

    for (int i = 0; i < n_joint; i++)
    {
        ok &= setPosition(joints[i], refs[i]);
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
yarp::dev::ReturnValue YarpOpenraveControlBoard::getRefPosition(int j, double * ref)
#else
bool YarpOpenraveControlBoard::getRefPosition(int j, double * ref)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "getRefPosition: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    //-- Check if we are in positionDirect mode.
    if (controlModes[j] != VOCAB_CM_POSITION_DIRECT)
    {
        yCError(YORCB) << "Not in positionDirect mode at joint" << j;
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
#else
        return false;
#endif
    }

    *ref = radToDegIfNotPrismatic(j, manipulatorTargetRads[j]);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getRefPositions(double * refs)
#else
bool YarpOpenraveControlBoard::getRefPositions(double * refs)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
        ok &= getRefPosition(i, &refs[i]);
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
yarp::dev::ReturnValue YarpOpenraveControlBoard::getRefPositions(int n_joint, const int * joints, double * refs)
#else
bool YarpOpenraveControlBoard::getRefPositions(int n_joint, const int * joints, double * refs)
#endif
{
    bool ok = true;

    for (int i = 0; i < n_joint; i++)
    {
        ok &= getRefPosition(joints[i], &refs[i]);
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
