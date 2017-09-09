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

    std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = probot->GetManipulators();
    manipulatorIDs = vectorOfManipulatorPtr[manipulatorIndex]->GetArmIndices();

    axes = manipulatorIDs.size();
    manipulatorTargets.resize( axes, 0.0 );

    for(size_t i=0; i<manipulatorIDs.size(); i++)
    {
        OpenRAVE::RobotBase::JointPtr jointPtr = probot->GetJointFromDOFIndex(manipulatorIDs[i]);
        vectorOfJointPtr.push_back(jointPtr);
    }

    //-- Create the controller, make sure to lock environment!
    {
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment

        pcontrol = OpenRAVE::RaveCreateController(penv,"idealcontroller");
        probot->SetController(pcontrol,manipulatorIDs,0); // control all manipulator joints
        penv->StopSimulation();
        penv->StartSimulation(0.001);
        probot->SetActiveDOFs(manipulatorIDs);
    }

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::close() {
    printf("[YarpOpenraveControlboard] close()\n");
    return true;
}

// -----------------------------------------------------------------------------
