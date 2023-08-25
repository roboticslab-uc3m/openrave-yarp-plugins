// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlBoard.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------ IAxisInfo Related -----------------------------------------

bool YarpOpenraveControlBoard::getAxisName(int axis, std::string& name)
{
    yCTrace(YORCB);
    if ((unsigned int)axis > axes) return false;
    name = vectorOfJointPtr[axis]->GetName();
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::getJointType(int axis, yarp::dev::JointTypeEnum& type)
{
    yCTrace(YORCB);
    if ((unsigned int)axis > axes) return false;

    OpenRAVE::RobotBase::JointPtr jointPtr = vectorOfJointPtr[axis];

    if (jointPtr->IsRevolute(0))
        type = yarp::dev::JointTypeEnum::VOCAB_JOINTTYPE_REVOLUTE;
    else if (jointPtr->IsPrismatic(0))
        type = yarp::dev::JointTypeEnum::VOCAB_JOINTTYPE_PRISMATIC;
    else
        type = yarp::dev::JointTypeEnum::VOCAB_JOINTTYPE_UNKNOWN;

    return true;
}

// -----------------------------------------------------------------------------
