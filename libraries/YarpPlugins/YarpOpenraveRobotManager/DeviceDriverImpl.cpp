// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRobotManager.hpp"

#include <string>
#include <vector>

#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveRobotManager::open(yarp::os::Searchable& config)
{
    yCDebug(YORRM) << "Config:" << config.toString();

    if ( ! configureEnvironment(config) )
        return false;

    if ( ! configureOpenravePlugins(config) )
        return false;

    if ( ! configureRobot(config) )
        return false;

    std::string modeStr = config.check("mode",yarp::os::Value("transform"),"robot mode: '4wd'|'transform'").asString();

    if (modeStr == "transform")
    {
        mode = TRANSFORM_IDEALCONTROLLER;
    }
    else if (modeStr == "4wd")
    {
        mode = FOUR_WHEEL_IDEALVELOCITYCONTROLLER;
    }
    else
    {
        yCError(YORRM) << "Unknown mode" << modeStr << "(use '4wd' or 'transform')";
        return false;
    }

    yCDebug(YORRM) << "Found --mode parameter:" << modeStr;

    //-- Create the controller, make sure to lock environment!
    {
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment

        switch (mode)
        {
        case TRANSFORM_IDEALCONTROLLER:
        {
            pcontrol = OpenRAVE::RaveCreateController(penv,"idealcontroller");  // idealcontroller, odevelocity, idealvelocitycontroller
            probot->SetActiveDOFs(std::vector<int>(), OpenRAVE::DOF_X|OpenRAVE::DOF_Y|OpenRAVE::DOF_RotationAxis,OpenRAVE::Vector(0,0,1));

            probot->SetController(pcontrol,std::vector<int>(),OpenRAVE::DOF_X|OpenRAVE::DOF_Y|OpenRAVE::DOF_RotationAxis);
            break;
        }
        case FOUR_WHEEL_IDEALVELOCITYCONTROLLER:
        {
            pcontrol = OpenRAVE::RaveCreateController(penv,"idealvelocitycontroller");  // idealcontroller, odevelocity, idealvelocitycontroller
            std::vector<int> dofindices( probot->GetDOF() );  // Note: GetDOF vs GetActiveDOF
            for(int i = 0; i < dofindices.size(); ++i)
            {
                dofindices[i] = i;
            }

            probot->SetController(pcontrol,dofindices,0);
            break;
        }
        default:
            return false;
        }

    }

    penv->StopSimulation();
    penv->StartSimulation(0.001);

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::close()
{
    return true;
}

// -----------------------------------------------------------------------------
