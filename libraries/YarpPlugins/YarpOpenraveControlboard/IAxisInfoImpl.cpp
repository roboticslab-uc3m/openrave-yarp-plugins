// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <ColorDebug.h>

// ------------------ IAxisInfo Related -----------------------------------------

bool roboticslab::YarpOpenraveControlboard::getAxisName(int axis, std::string& name)
{
    CD_INFO("\n");
    if ((unsigned int)axis>axes) return false;
    name = vectorOfJointPtr[axis]->GetName();
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getJointType(int axis, yarp::dev::JointTypeEnum& type)
{
    CD_INFO("\n");
    if ((unsigned int)axis>axes) return false;

    OpenRAVE::RobotBase::JointPtr jointPtr = vectorOfJointPtr[axis];
    if( jointPtr->IsRevolute(0) )
        type = yarp::dev::JointTypeEnum::VOCAB_JOINTTYPE_REVOLUTE;
    else if( jointPtr->IsPrismatic(0) )
        type = yarp::dev::JointTypeEnum::VOCAB_JOINTTYPE_PRISMATIC;
    else
        type = yarp::dev::JointTypeEnum::VOCAB_JOINTTYPE_UNKNOWN;

    return true;
}

// -----------------------------------------------------------------------------
