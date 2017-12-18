// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <algorithm>  // Thanks: https://notfaq.wordpress.com/2007/08/04/cc-convert-string-to-upperlower-case/

namespace roboticslab
{

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveControlboard::open(yarp::os::Searchable& config)
{
    CD_DEBUG("config: %s\n",config.toString().c_str());

    if ( ! configureEnvironment(config) )
        return false;

    if ( ! configureOpenravePlugins(config) )
        return false;

    if ( ! configureRobot(config) )
        return false;

    int manipulatorIndex = config.check("manipulatorIndex",-1,"manipulatorIndex").asInt();
    double genRefSpeed = config.check("genRefSpeed",DEFAULT_GEN_REF_SPEED,"general ref speed").asDouble();

    std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = probot->GetManipulators();
    if(manipulatorIndex >= vectorOfManipulatorPtr.size())
    {
        CD_ERROR("manipulatorIndex %d >= vectorOfManipulatorPtr.size() %d, not loading yarpPlugin.\n",manipulatorIndex,vectorOfManipulatorPtr.size());
        return false;
    }
    else if (manipulatorIndex < 0)
    {
        CD_ERROR("manipulatorIndex %d < 0, not loading yarpPlugin.\n",manipulatorIndex);
        return false;
    }

    manipulatorIDs = vectorOfManipulatorPtr[manipulatorIndex]->GetArmIndices();

    axes = manipulatorIDs.size();
    manipulatorTargetRads.resize( axes, 0.0 );
    controlModes.resize( axes, VOCAB_CM_POSITION );
    refSpeeds.resize( axes, genRefSpeed );

    for(size_t i=0; i<manipulatorIDs.size(); i++)
    {
        OpenRAVE::RobotBase::JointPtr jointPtr = probot->GetJointFromDOFIndex(manipulatorIDs[i]);
        vectorOfJointPtr.push_back(jointPtr);
        CD_DEBUG("Get JointPtr for manipulatorIDs[%d]: %d\n",i,manipulatorIDs[i]);
    }

    //-- Create the controller, make sure to lock environment!
    {
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment

        std::vector<int> activeDOFIndices = probot->GetActiveDOFIndices();
        //--- Console output robot active DOF
        //for(size_t i=0; i<activeDOFIndices.size(); i++)
        //{
        //    CD_DEBUG("activeDOFIndices[%d]: %d\n",i,activeDOFIndices[i]);
        //}

        //-- Convert robot controller to multi if not already.
        OpenRAVE::ControllerBasePtr pcontrol = probot->GetController();
        CD_DEBUG("pcontrol: %p, %s\n",pcontrol.get(),pcontrol->GetXMLId().c_str());
        //-- Doing case insensitive check because defaults to IdealController but idealcontroller exists
        std::string controllerName( pcontrol->GetXMLId() );
        std::transform(controllerName.begin(), controllerName.end(), controllerName.begin(), ::tolower);
        if( controllerName == "idealcontroller" )
        {
            CD_INFO("Detected idealcontroller, switch to genericmulticontroller.\n");
            pcontrol = OpenRAVE::RaveCreateMultiController(penv);
            probot->SetController(pcontrol,activeDOFIndices,1);  // idealcontroller -> genericmulticontroller
        }
        else if( controllerName == "genericmulticontroller")
        {
            CD_INFO("Detected genericmulticontroller, which will be used.\n");
        }
        else
        {
            CD_ERROR("Non-treated controller case. Bye!\n");
            return false;
        }
        CD_DEBUG("pcontrol: %p, %s\n",pcontrol.get(),pcontrol->GetXMLId().c_str());

        //-- Safe to assume we have a multicontroller, store for usage.
        multi = boost::dynamic_pointer_cast< OpenRAVE::MultiControllerBase >(pcontrol);

        for(size_t i=0; i<manipulatorIDs.size(); i++)
        {
            OpenRAVE::ControllerBasePtr controlFromMulti = multi->GetController( manipulatorIDs[i] );
            if( !! controlFromMulti )
            {
                CD_DEBUG("EXPERIMENTAL: Using existing individual controller for manipulatorIDs[%d]: %d (%s)\n",i,manipulatorIDs[i],controlFromMulti->GetXMLId().c_str());
                pcontrols.push_back(controlFromMulti);
                continue;
            }
            OpenRAVE::ControllerBasePtr pindivcontrol = OpenRAVE::RaveCreateController(penv,"idealcontroller");  // idealcontroller, odevelocity, idealvelocitycontroller
            std::vector<int> tmpIndices;
            tmpIndices.push_back( manipulatorIDs[i] );
            CD_DEBUG("Attach individual controller for manipulatorIDs[%d]: %d\n",i,tmpIndices[0]);
            multi->AttachController(pindivcontrol, tmpIndices, 0);
            pcontrols.push_back(pindivcontrol);
        }

        penv->StopSimulation();
        penv->StartSimulation(0.01);

        //-- Console output of the robot ConfigurationSpecification
        //OpenRAVE::ConfigurationSpecification activeConfigurationSpecification = probot->GetActiveConfigurationSpecification();
        //for (size_t i = 0; i < activeConfigurationSpecification._vgroups.size(); i++)
        //{
        //    CD_DEBUG("%d, %s, %s\n",i,activeConfigurationSpecification._vgroups[i].name.c_str(), activeConfigurationSpecification._vgroups[i].interpolation.c_str());
        //}

    }

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::close()
{
    CD_INFO("\n");
    return true;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab
