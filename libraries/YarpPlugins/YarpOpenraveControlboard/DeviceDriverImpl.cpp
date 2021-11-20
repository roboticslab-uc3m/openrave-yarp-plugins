// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <cstddef> // std::size_t

#include <algorithm> // Thanks: https://notfaq.wordpress.com/2007/08/04/cc-convert-string-to-upperlower-case/
#include <string>
#include <vector>

#include <yarp/conf/version.h>

#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

#include <boost/smart_ptr/shared_ptr.hpp>

#include "LogComponent.hpp"

using namespace roboticslab;

constexpr auto DEFAULT_GEN_REF_SPEED = 7.5;
constexpr auto NOT_SET = -1;

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveControlboard::open(yarp::os::Searchable& config)
{
#if !defined(YARP_VERSION_COMPARE) // < 3.6.0
    yCDebug(YORCB) << "Config:" << config.toString();
#endif

    if ( ! configureEnvironment(config) )
        return false;

    if ( ! configureOpenravePlugins(config) )
        return false;

    if ( ! configureRobot(config) )
        return false;

    int manipulatorIndex = config.check("manipulatorIndex",yarp::os::Value(NOT_SET),"manipulator index").asInt32();
    double genRefSpeed = config.check("genRefSpeed",yarp::os::Value(DEFAULT_GEN_REF_SPEED),"general reference speed [m/s] or [deg/s]").asFloat64();

    if (manipulatorIndex == NOT_SET)
    {
        yCError(YORCB) << "manipulatorIndex" << manipulatorIndex << "== NOT_SET, not loading yarpPlugin";
        return false;
    }
    std::vector<OpenRAVE::RobotBase::ManipulatorPtr> vectorOfManipulatorPtr = probot->GetManipulators();
    if( (manipulatorIndex >= vectorOfManipulatorPtr.size()) || (manipulatorIndex < 0) )
    {
        yCError(YORCB, "manipulatorIndex %d not within vectorOfManipulatorPtr of size() %zu, not loading yarpPlugin",manipulatorIndex,vectorOfManipulatorPtr.size());
        return false;
    }

    manipulatorIDs = vectorOfManipulatorPtr[manipulatorIndex]->GetArmIndices();

    axes = manipulatorIDs.size();
    manipulatorTargetRads.resize( axes, 0.0 );
    controlModes.resize( axes, VOCAB_CM_POSITION );
    refSpeeds.resize( axes, genRefSpeed );

    for(std::size_t i=0; i<manipulatorIDs.size(); i++)
    {
        OpenRAVE::RobotBase::JointPtr jointPtr = probot->GetJointFromDOFIndex(manipulatorIDs[i]);
        vectorOfJointPtr.push_back(jointPtr);
        yCDebug(YORCB, "Get JointPtr for manipulatorIDs[%zu]: %d (%s)",i,manipulatorIDs[i],jointPtr->GetName().c_str());
    }

    //-- Create the controller, make sure to lock environment!
    {
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment

        std::vector<int> activeDOFIndices = probot->GetActiveDOFIndices();
        //--- Console output robot active DOF
        //for(std::size_t i=0; i<activeDOFIndices.size(); i++)
        //{
        //    yCDebug(YORCB, "activeDOFIndices[%d]: %d",i,activeDOFIndices[i]);
        //}

        //-- Convert robot controller to multi if not already.
        OpenRAVE::ControllerBasePtr pcontrol = probot->GetController();
        yCDebug(YORCB) << "pcontrol:" << pcontrol.get() << pcontrol->GetXMLId();
        //-- Doing case insensitive check because defaults to IdealController but idealcontroller exists
        std::string controllerName( pcontrol->GetXMLId() );
        std::transform(controllerName.begin(), controllerName.end(), controllerName.begin(), ::tolower);
        if( controllerName == "idealcontroller" )
        {
            yCInfo(YORCB) << "Detected idealcontroller, switch to genericmulticontroller";
            pcontrol = OpenRAVE::RaveCreateMultiController(penv);
            probot->SetController(pcontrol,activeDOFIndices,1);  // idealcontroller -> genericmulticontroller
        }
        else if( controllerName == "genericmulticontroller")
        {
            yCInfo(YORCB) << "Detected genericmulticontroller, which will be used";
        }
        else
        {
            yCError(YORCB) << "Non-treated controller case";
            return false;
        }
        yCDebug(YORCB) << "pcontrol:" << pcontrol.get() << pcontrol->GetXMLId();

        //-- Safe to assume we have a multicontroller, store for usage.
        multi = boost::dynamic_pointer_cast< OpenRAVE::MultiControllerBase >(pcontrol);

        for(std::size_t i=0; i<manipulatorIDs.size(); i++)
        {
            OpenRAVE::ControllerBasePtr pIndivControlFromMulti = multi->GetController( manipulatorIDs[i] );
            if( !! pIndivControlFromMulti )
            {
                yCDebug(YORCB, "EXPERIMENTAL: Using existing individual controller for manipulatorIDs[%zu]: %i (%s)",i,manipulatorIDs[i],pIndivControlFromMulti->GetXMLId().c_str());
                pcontrols.push_back(pIndivControlFromMulti);
                continue;
            }
            OpenRAVE::ControllerBasePtr pIndivControl = OpenRAVE::RaveCreateController(penv,"idealcontroller");  // idealcontroller, odevelocity, idealvelocitycontroller
            std::vector<int> tmpIndices;
            tmpIndices.push_back( manipulatorIDs[i] );
            yCDebug(YORCB, "Attach individual controller for manipulatorIDs[%zu]: %i",i,tmpIndices[0]);
            multi->AttachController(pIndivControl, tmpIndices, 0);
            pcontrols.push_back(pIndivControl);
            /*std::stringstream cmd, res;
            cmd << "SetThrowExceptions 1";
            pIndivControl->SendCommand(res, cmd);*/
        }

        //-- Console output of the robot ConfigurationSpecification
        //OpenRAVE::ConfigurationSpecification activeConfigurationSpecification = probot->GetActiveConfigurationSpecification();
        //for (std::size_t i = 0; i < activeConfigurationSpecification._vgroups.size(); i++)
        //{
        //    yCDebug(YORCB, "%d, %s, %s",i,activeConfigurationSpecification._vgroups[i].name.c_str(), activeConfigurationSpecification._vgroups[i].interpolation.c_str());
        //}
    }

    penv->StopSimulation();
    penv->StartSimulation(0.01);

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlboard::close()
{
    return true;
}

// -----------------------------------------------------------------------------
