// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/LogStream.h>

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
    ss << "expected type string but got wrong data type at " << index;
    response.addString(ss.str());
    yError(ss.str().c_str(), index);
    return false;
}

// -----------------------------------------------------------------------------

bool roboticslab::OypPortReader::tryToSetActiveManipulator(const std::string& robot, const std::string& manipulator, yarp::os::Bottle& response)
{
    OpenRAVE::RobotBasePtr wantActiveRobotPtr = openraveYarpPlannerPtr->GetEnv()->GetRobot(robot);
    if(!wantActiveRobotPtr)
    {
        yError() << "Could not find robot:" << robot;
        response.addVocab(VOCAB_FAILED);
        return false;
    }

    try
    {
        wantActiveRobotPtr->SetActiveManipulator(manipulator);
    }
    catch (const std::exception& ex)
    {
        yError() << "Caught openrave_exception:" << ex.what();
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
    yDebug() << "Request:" << request.toString();
    yarp::os::ConnectionWriter *out = in.getWriter();
    if (out==NULL) return true;

    if ( request.get(0).asString() == "help" ) //-- help
    {
        response.addString("Available commands: help, goto (robotName) (manipulatorName) (joint pos goals)");
        return response.write(*out);
    }
    else if ( request.get(0).asString() == "goto" ) //-- goto
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

        std::stringstream cmdin, cmdout;
        cmdin << "MoveManipulator outputtraj goal ";
        for(int jointIdx=0; jointIdx<request.size()-3; jointIdx++)
        {
            double value = request.get(jointIdx+3).asFloat64();
            OpenRAVE::RobotBase::JointPtr jointPtr = robotPtr->GetJointFromDOFIndex(manipulatorIDs[jointIdx]);
            if( jointPtr->IsRevolute(0) )
                value *= M_PI / 180.0;
            cmdin << value;
            cmdin << " ";
        }
        yDebug() << "cmd:" << cmdin.str();

        {
            OpenRAVE::EnvironmentMutex::scoped_lock lock(openraveYarpPlannerPtr->GetEnv()->GetMutex()); // lock environment

            if( !pbasemanip->SendCommand(cmdout,cmdin) )
            {
                response.addVocab(VOCAB_FAILED);
                response.write(*out);
                return true;
            }
        }
        yDebug() << "res:" << cmdout.str();

        response.addVocab(VOCAB_OK);

        OpenRAVE::TrajectoryBasePtr ptraj = RaveCreateTrajectory(openraveYarpPlannerPtr->GetEnv(),"");
        ptraj->deserialize(cmdout);
        yDebug() << "Duration:" << ptraj->GetDuration();
        for (double time=0.0; time < ptraj->GetDuration(); time+=0.01)
        {
           yarp::os::Bottle& waypointBottle = response.addList();
           waypointBottle.addFloat64(time);
           yarp::os::Bottle& waypointPosBottle = waypointBottle.addList();
           yarp::os::Bottle& waypointVelBottle = waypointBottle.addList();
           std::vector< OpenRAVE::dReal > point;
           ptraj->Sample(point, time);
           size_t numJoints = (point.size() - 2) / 2;
           auto && log = yDebug();
           log << time; // cmd timestamp

           for(int i=0; i<point.size()-2; i++) // skip timestamp and iswaypoint
           {
               if(i < numJoints)
               {
                   OpenRAVE::RobotBase::JointPtr jointPtr = robotPtr->GetJointFromDOFIndex(manipulatorIDs[i]);
                   if( jointPtr->IsRevolute(0) )
                       point[i] *= 180.0 / M_PI;
                   waypointPosBottle.addFloat64(point[i]);
               }
               else if(i < numJoints*2)
               {
                   OpenRAVE::RobotBase::JointPtr jointPtr = robotPtr->GetJointFromDOFIndex(manipulatorIDs[i-numJoints]);
                   if( jointPtr->IsRevolute(0) )
                       point[i] *= 180.0 / M_PI;
                   waypointVelBottle.addFloat64(point[i]);
               }
               log << point[i];
           }
        }

        return response.write(*out);
    }

    response.addVocab(VOCAB_FAILED);
    response.addString("unknown command");
    return response.write(*out);
}

// -----------------------------------------------------------------------------
