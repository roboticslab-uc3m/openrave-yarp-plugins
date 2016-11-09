// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "FakeControlboardOR.hpp"

// ------------------- DeviceDriver Related ------------------------------------

bool teo::FakeControlboardOR::open(yarp::os::Searchable& config) {

    printf("here1\n");

    //CD_DEBUG("penv: %p\n",*((const OpenRAVE::EnvironmentBase**)(config.find("penv").asBlob())));
    penv = *((OpenRAVE::EnvironmentBase**)(config.find("penv").asBlob()));

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

    printf("here2\n");

    std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
    penv->GetRobots(vectorOfRobotPtr);
    probot = vectorOfRobotPtr[robotIndex];

    printf("here3\n");

    dEncRaw.resize( probot->GetDOF() );
    std::fill(dEncRaw.begin(), dEncRaw.end(), 0);

    printf("here4\n");

    std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = probot->GetManipulators();
    manipulatorIDs = vectorOfManipulatorPtr[manipulatorIndex]->GetArmIndices();

    printf("here5\n");

    axes = manipulatorIDs.size();

    printf("here6\n");

    return true;
}

// -----------------------------------------------------------------------------

bool teo::FakeControlboardOR::close() {
    printf("[FakeControlboardOR] close()\n");
    return true;
}

// -----------------------------------------------------------------------------
