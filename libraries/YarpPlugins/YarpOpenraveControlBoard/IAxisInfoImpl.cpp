// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlBoard.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------ IAxisInfo Related -----------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getAxisName(int axis, std::string & name)
#else
bool YarpOpenraveControlBoard::getAxisName(int axis, std::string& name)
#endif
{
    if (axis < 0 || (unsigned int)axis > axes)
    {
        yCError(YORCB) << "getAxisName: axis" << axis << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    name = vectorOfJointPtr[axis]->GetName();
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getJointType(int axis, yarp::dev::JointTypeEnum & type)
#else
bool YarpOpenraveControlBoard::getJointType(int axis, yarp::dev::JointTypeEnum& type)
#endif
{
    if (axis < 0 || (unsigned int)axis > axes)
    {
        yCError(YORCB) << "getAxisName: axis" << axis << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    OpenRAVE::RobotBase::JointPtr jointPtr = vectorOfJointPtr[axis];

    if (jointPtr->IsRevolute(0))
    {
        type = yarp::dev::JointTypeEnum::VOCAB_JOINTTYPE_REVOLUTE;
    }
    else if (jointPtr->IsPrismatic(0))
    {
        type = yarp::dev::JointTypeEnum::VOCAB_JOINTTYPE_PRISMATIC;
    }
    else
    {
        type = yarp::dev::JointTypeEnum::VOCAB_JOINTTYPE_UNKNOWN;
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------
