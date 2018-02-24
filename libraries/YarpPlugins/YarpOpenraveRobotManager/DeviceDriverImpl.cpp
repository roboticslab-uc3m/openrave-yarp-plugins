// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRobotManager.hpp"

namespace roboticslab
{

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveRobotManager::open(yarp::os::Searchable& config)
{
    CD_DEBUG("config: %s\n",config.toString().c_str());

    if ( ! configureEnvironment(config) )
        return false;

    if ( ! configureOpenravePlugins(config) )
        return false;

    if ( ! configureRobot(config) )
        return false;

    std::string robotModeStr = config.check("robotMode",yarp::os::Value("no mode"),"robot mode").asString();
    if (robotModeStr == "4wd")
    {
        robotMode = FOUR_WHEEL_IDEALVELOCITYCONTROLLER;
    }
    else if (robotModeStr == "transform")
    {
        robotMode = TRANSFORM_IDEALPOSITIONCONTROLLER;
    }
    else
    {
        CD_ERROR("Unknown mode '%s'.\n",robotModeStr.c_str());
        return false;
    }

    //-- Create the controller, make sure to lock environment!
    {
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment

        pcontrol = OpenRAVE::RaveCreateController(penv,"idealvelocitycontroller");  // idealcontroller, odevelocity, idealvelocitycontroller

        std::vector<int> dofindices( probot->GetDOF() );
        for(int i = 0; i < probot->GetDOF(); ++i)
        {
            dofindices[i] = i;
        }

        probot->SetController(pcontrol,dofindices,0);
    }

    penv->StopSimulation();
    penv->StartSimulation(0.01);

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::close()
{
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab
