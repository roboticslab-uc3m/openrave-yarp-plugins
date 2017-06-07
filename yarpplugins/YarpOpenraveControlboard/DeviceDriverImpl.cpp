// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

// ------------------- DeviceDriver Related ------------------------------------

bool roboticslab::YarpOpenraveControlboard::open(yarp::os::Searchable& config) {

    //CD_DEBUG("penv: %p\n",*((const OpenRAVE::EnvironmentBase**)(config.find("penv").asBlob())));
    penv = *((OpenRAVE::EnvironmentBasePtr*)(config.find("penv").asBlob()));

    int robotIndex = config.check("robotIndex",-1,"robotIndex").asInt();
    if( robotIndex < 0 )  // a.k.a. -1 one line above
    {
        CD_ERROR("Please review robotIndex.\n");
        return false;
    }
    int manipulatorIndex = config.check("manipulatorIndex",-1,"manipulatorIndex").asInt();
    if( manipulatorIndex < 0 )  // a.k.a. -1 one line above
    {
        CD_ERROR("Please review manipulatorIndex.\n");
        return false;
    }

    std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
    penv->GetRobots(vectorOfRobotPtr);
    probot = vectorOfRobotPtr[robotIndex];

    dEncRaw.resize( probot->GetDOF() );
    std::fill(dEncRaw.begin(), dEncRaw.end(), 0);

    std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = probot->GetManipulators();
    manipulatorIDs = vectorOfManipulatorPtr[manipulatorIndex]->GetArmIndices();

    axes = manipulatorIDs.size();

    for(int i=0; i<axes; i++)
    {
        OpenRAVE::RobotBase::JointPtr jointPtr = probot->GetJointFromDOFIndex(manipulatorIDs[i]);
        vectorOfJointPtr.push_back(jointPtr);
    }

    for(int i=0; i<axes; i++)
    {
        std::vector<OpenRAVE::dReal> vLowerLimit;
        std::vector<OpenRAVE::dReal> vUpperLimit;
        vectorOfJointPtr[i]->GetLimits(vLowerLimit,vUpperLimit);
        CD_INFO("Limits %d: [%f,%f]\n",i,vLowerLimit[0]*180.0/M_PI,vUpperLimit[0]*180.0/M_PI);
    }


    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::close() {
    printf("[YarpOpenraveControlboard] close()\n");
    return true;
}

// -----------------------------------------------------------------------------
