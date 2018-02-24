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

    std::string modeStr = config.check("mode",yarp::os::Value("no mode"),"robot mode ('4wd' or 'transform')").asString();
    if (modeStr == "4wd")
    {
        mode = FOUR_WHEEL_IDEALVELOCITYCONTROLLER;
    }
    else if (modeStr == "transform")
    {
        mode = TRANSFORM_IDEALCONTROLLER;
    }
    else
    {
        CD_ERROR("Unknown mode '%s' (use '4wd' or 'transform').\n",modeStr.c_str());
        return false;
    }
    CD_DEBUG("Found --mode parameter: %s.\n", modeStr.c_str());

    //-- Create the controller, make sure to lock environment!
    {
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment



        switch (mode)
        {
        case TRANSFORM_IDEALCONTROLLER:
        {
            pcontrol = OpenRAVE::RaveCreateController(penv,"idealcontroller");  // idealcontroller, odevelocity, idealvelocitycontroller
            probot->SetActiveDOFs(std::vector<int>(), OpenRAVE::DOF_X|OpenRAVE::DOF_Y|OpenRAVE::DOF_RotationAxis,OpenRAVE::Vector(0,0,1));
            break;
        }
        case FOUR_WHEEL_IDEALVELOCITYCONTROLLER:
        {
            pcontrol = OpenRAVE::RaveCreateController(penv,"idealvelocitycontroller");  // idealcontroller, odevelocity, idealvelocitycontroller
            break;
        }
        default:
            return false;
        }

        std::vector<int> dofindices( probot->GetActiveDOF() );  // Was GetDOF, but now we want the active ones
        for(int i = 0; i < probot->GetActiveDOF(); ++i)
        {
            CD_DEBUG("%d\n",i)
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
