// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboardCollision.hpp"

// ------------------- DeviceDriver Related ------------------------------------

bool teo::YarpOpenraveControlboardCollision::open(yarp::os::Searchable& config) {

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

    std::vector<OpenRAVE::RobotBasePtr> vectorOfRobotPtr;
    penv->GetRobots(vectorOfRobotPtr);
    probot = vectorOfRobotPtr[robotIndex];

    dEncRaw.resize( probot->GetDOF() );
    std::fill(dEncRaw.begin(), dEncRaw.end(), 0);

    std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = probot->GetManipulators();
    manipulatorIDs = vectorOfManipulatorPtr[manipulatorIndex]->GetArmIndices();

    axes = manipulatorIDs.size();

    //-- connect to remote robot
    std::string remoteStr = config.check("remote",yarp::os::Value(DEFAULT_REMOTE),"remote robot").asString();
    CD_INFO("remote: %s [%s]\n",remoteStr.c_str(),DEFAULT_REMOTE);
    yarp::os::Property options;
    options.put("device","remote_controlboard");
    std::string YarpOpenraveControlboardStr("/YarpOpenraveControlboardCollision");
    options.put("local",YarpOpenraveControlboardStr+remoteStr);
    options.put("remote",remoteStr);  //-- Hard-code for now
    remoteDevice.open(options);
    if( ! remoteDevice.isValid() ) {
        CD_ERROR("robot remote not valid: %s.\n",remoteStr.c_str());
        return false;
    }
    if( ! remoteDevice.view(iEncoders) ) {
        CD_ERROR("Could not view iEncoders in: %s.\n",remoteStr.c_str());
        return false;
    }
    if( ! remoteDevice.view(iPositionControl) ) {
        CD_ERROR("Could not view iPositionControl in: %s.\n",remoteStr.c_str());
        return false;
    }
    if( ! remoteDevice.view(iControlLimits2) ) {
        CD_ERROR("Could not view iControlLimits2 in: %s.\n",remoteStr.c_str());
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::close() {
    printf("[YarpOpenraveControlboardCollision] close()\n");
    return true;
}

// -----------------------------------------------------------------------------
