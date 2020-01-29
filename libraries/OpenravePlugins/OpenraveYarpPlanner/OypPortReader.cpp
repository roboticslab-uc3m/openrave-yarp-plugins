// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>

#include <ColorDebug.h>

#include "OpenraveYarpPlanner.hpp"

#include "OypPortReader.hpp"

// -----------------------------------------------------------------------------

const yarp::conf::vocab32_t roboticslab::OypPortReader::VOCAB_OK = yarp::os::createVocab('o','k');
const yarp::conf::vocab32_t roboticslab::OypPortReader::VOCAB_FAILED = yarp::os::createVocab('f','a','i','l');

// -----------------------------------------------------------------------------

bool roboticslab::OypPortReader::checkIfString(yarp::os::Bottle& request, int index, yarp::os::Bottle& response)
{
    if (request.get(index).isString())
        return true;
    response.addVocab(VOCAB_FAILED);
    std::stringstream ss;
    ss << "expected type string but got wrong data type at ";
    ss << index;
    response.addString(ss.str());
    ss << "\n";
    CD_ERROR(ss.str().c_str(), index);
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::OypPortReader::tryToSetActiveManipulator(const std::string& robot, const std::string& manipulator, yarp::os::Bottle& response)
{
    OpenRAVE::RobotBasePtr wantActiveRobotPtr = openraveYarpPlannerPtr->GetEnv()->GetRobot(robot);
    if(!wantActiveRobotPtr)
    {
        CD_ERROR("Could not find robot: %s.\n", robot.c_str());
        response.addVocab(VOCAB_FAILED);
        return false;
    }

    try
    {
        wantActiveRobotPtr->SetActiveManipulator(manipulator);
    }
    catch (const std::exception& ex)
    {
        CD_ERROR("Caught openrave_exception: %s\n", ex.what());
        response.addVocab(VOCAB_FAILED);
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::OypPortReader::read(yarp::os::ConnectionReader& in)
{
    yarp::os::Bottle request, response;
    if (!request.read(in)) return false;
    CD_DEBUG("Request: %s\n", request.toString().c_str());
    yarp::os::ConnectionWriter *out = in.getWriter();
    if (out==NULL) return true;

    if ( request.get(0).asString() == "help" ) //-- help
    {
        response.addString("Available commands: help, plan, goto");
        return response.write(*out);
    }
    else if ( request.get(0).asString() == "plan" ) //-- plan
    {
        if (!checkIfString(request, 1, response))
            return response.write(*out);
        if (!checkIfString(request, 2, response))
            return response.write(*out);
        if (!tryToSetActiveManipulator(request.get(1).asString(), request.get(2).asString(), response))
            return response.write(*out);

        OpenRAVE::RobotBasePtr robotPtr = openraveYarpPlannerPtr->GetEnv()->GetRobot(request.get(1).asString());
        OpenRAVE::RobotBase::ManipulatorPtr manipulatorPtr = robotPtr->GetManipulator(request.get(2).asString());
        std::vector<int> manipulatorIDs = manipulatorPtr->GetArmIndices();

        OpenRAVE::ModuleBasePtr pbasemanip = RaveCreateModule(openraveYarpPlannerPtr->GetEnv(),"basemanipulation");
        openraveYarpPlannerPtr->GetEnv()->Add(pbasemanip,true,request.get(1).asString()); // load the module
        {
            OpenRAVE::EnvironmentMutex::scoped_lock lock(openraveYarpPlannerPtr->GetEnv()->GetMutex()); // lock environment

            std::stringstream cmdin, cmdout;
            cmdin << "MoveManipulator goal ";

            for(int jointIdx=0; jointIdx<request.size()-3; jointIdx++)
            {
                double value = request.get(jointIdx+3).asFloat64();
                OpenRAVE::RobotBase::JointPtr jointPtr = robotPtr->GetJointFromDOFIndex(manipulatorIDs[jointIdx]);
                if( jointPtr->IsRevolute(0) )
                    value *= M_PI / 180.0;
                cmdin << value;
                cmdin << " ";
            }
            CD_DEBUG("%s\n", cmdin.str().c_str());

            if( !pbasemanip->SendCommand(cmdout,cmdin) )
            {
                response.addVocab(VOCAB_FAILED);
                response.write(*out);
                return true;
            }
        }

        response.addVocab(VOCAB_OK);
        return response.write(*out);
    }
    else if ( request.get(0).asString() == "goto" ) //-- goto
    {

        response.addVocab(VOCAB_OK);
        return response.write(*out);
    }

    response.addVocab(VOCAB_FAILED);
    response.addString("unknown command");
    return response.write(*out);
}

// -----------------------------------------------------------------------------
