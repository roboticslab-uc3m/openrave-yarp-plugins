// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlBoard.hpp"

#include <sstream>
#include <string>
#include <vector>

#include <openrave/config.h>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------- IPositionControl Related --------------------------------

bool YarpOpenraveControlBoard::getAxes(int *ax)
{
    *ax = axes;
    yCInfo(YORCB) << "Reporting" << axes << "axes are present";
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::positionMove(int j, double ref)
{
    yCTrace(YORCB) << j << ref;

    //-- Check if we are in position mode.
    if (controlModes[j] != VOCAB_CM_POSITION)
    {
        yCError(YORCB) << "Will not positionMove() as joint" << j << "not in positionMode";
        return false;
    }

    OpenRAVE::dReal dofTargetRads = degToRadIfNotPrismatic(j, ref); // ref comes in exposed

    //-- Store the targets
    manipulatorTargetRads[j] = dofTargetRads;

    //-- But do not move if no velocity
    if (refSpeeds[j] == 0)
    {
        yCDebug(YORCB, "[%d] (refSpeeds[ j ] == 0) => Avoid division by 0 => Just act like blocked joint, return true", j);
        return true;
    }

    {
#if OPENRAVE_VERSION >= OPENRAVE_VERSION_COMBINED(0, 68, 0)
        OpenRAVE::EnvironmentLock lock(penv->GetMutex()); // lock environment
#else
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment
#endif

        //-- Check and do immediate movement if appropriate.
        //-- In fact, OpenRAVE would actually do the extra-fast movement but warn at all times.
        double min, max;
        getVelLimits(j, &min, &max);

        if (refSpeeds[j] > max)
        {
            std::vector<OpenRAVE::dReal> tmp;
            tmp.push_back(dofTargetRads);
            pcontrols[j]->SetDesired(tmp);
            return true;
        }

        //--- Console output robot active DOF
        //std::vector<int> activeDOFIndices = probot->GetActiveDOFIndices();
        //for(size_t i=0; i<activeDOFIndices.size(); i++)
        //{
        //    yCDebug(YORCB, "activeDOFIndices[%d]: %d",i,activeDOFIndices[i]);
        //}

        //-- Could get the activeConfigurationSpecification from the robot, and modify it
        //OpenRAVE::ConfigurationSpecification activeConfigurationSpecification = probot->GetActiveConfigurationSpecification();
        //activeConfigurationSpecification.GetGroupFromName("joint_values").interpolation = "linear";

        //-- Our own ConfigurationSpecification
        OpenRAVE::ConfigurationSpecification oneDofConfigurationSpecification;

        //-- Add the linear interpolation tag to the joint_values group
        OpenRAVE::ConfigurationSpecification::Group joint_values;
        std::string joint_valuesName("joint_values ");
        joint_valuesName.append(robotName);
        joint_valuesName.append(" ");
        std::stringstream ss;
        ss << manipulatorIDs[j];
        joint_valuesName.append(ss.str());
        joint_values.name = joint_valuesName;
        joint_values.offset = 0;
        joint_values.dof = 1;
        joint_values.interpolation = "linear";
        oneDofConfigurationSpecification.AddGroup(joint_values);

        //-- Add a required deltatime group
        //-- Perhaps also could be done via: int timeoffset = spec.AddDeltaTimeGroup();
        OpenRAVE::ConfigurationSpecification::Group deltatime;
        deltatime.name = "deltatime";
        deltatime.offset = 1;
        deltatime.dof = 1;
        deltatime.interpolation = "";
        oneDofConfigurationSpecification.AddGroup(deltatime);

        OpenRAVE::ConfigurationSpecification::Group iswaypoint;
        iswaypoint.name = "iswaypoint";
        iswaypoint.offset = 2;
        iswaypoint.dof = 1;
        iswaypoint.interpolation = "next";
        oneDofConfigurationSpecification.AddGroup(iswaypoint);

        //-- Console output of the manually adjusted ConfigurationSpecification
        //for (size_t i = 0; i < oneDofConfigurationSpecification._vgroups.size(); i++)
        //{
        //    OpenRAVE::ConfigurationSpecification::Group g = oneDofConfigurationSpecification._vgroups[i];
        //    yCDebug(YORCB, "[%d] %s, %d, %d, %s",i,g.name.c_str(), g.offset, g.dof, g.interpolation.c_str());
        //}

        OpenRAVE::TrajectoryBasePtr ptraj = OpenRAVE::RaveCreateTrajectory(penv,"");

        ptraj->Init(oneDofConfigurationSpecification);

        OpenRAVE::dReal dofCurrentRads = vectorOfJointPtr[j]->GetValue(0);

        OpenRAVE::dReal dofTime = std::abs(( dofTargetRads - dofCurrentRads ) / degToRadIfNotPrismatic(j, refSpeeds[j])); // Time in seconds

        yCDebug(YORCB, "[%d] abs(target-current)/vel = abs(%f-%f)/%f = %f [s]", j, ref,radToDegIfNotPrismatic(j, dofCurrentRads), refSpeeds[j], dofTime);

        //-- ptraj[0] with positions it has now, with: 0 deltatime, 1 iswaypoint
        std::vector<OpenRAVE::dReal> dofCurrentFull(3);
        dofCurrentFull[0] = dofCurrentRads;  // joint_values
        dofCurrentFull[1] = 0;           // deltatime
        dofCurrentFull[2] = 1;           // iswaypoint
        ptraj->Insert(0, dofCurrentFull);

        //-- ptraj[1] with position targets, with: 1 deltatime, 1 iswaypoint
        std::vector<OpenRAVE::dReal> dofTargetFull(3);
        dofTargetFull[0] = dofTargetRads;  // joint_values
        dofTargetFull[1] = dofTime;    // deltatime
        dofTargetFull[2] = 1;          // iswaypoint
        ptraj->Insert(1, dofTargetFull);

        //-- SetPath makes the controller perform the trajectory
        pcontrols[j]->SetPath(ptraj);
    }

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::positionMove(const double *refs)
{
    yCTrace(YORCB);
    bool ok = true;
    for (unsigned int i = 0; i < axes; i++)
        ok &= positionMove(i, refs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::relativeMove(int j, double delta)
{
    yCTrace(YORCB) << j << delta;

    double v = radToDegIfNotPrismatic(j, vectorOfJointPtr[j]->GetValue(0) );

    return positionMove(j, v + delta);
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::relativeMove(const double *deltas)
{
    yCTrace(YORCB);
    bool ok = true;
    for (unsigned int i = 0; i < axes; i++)
        ok &= relativeMove(i, deltas[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::checkMotionDone(int j, bool *flag)
{
    yCTrace(YORCB) << j;
    *flag = pcontrols[j]->IsDone();
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::checkMotionDone(bool *flag)
{
    yCTrace(YORCB);
    bool done = true;
    for (unsigned int j = 0; j < axes; j++)
    {
        bool tmpDone;
        checkMotionDone(j, &tmpDone);
        done &= tmpDone;
    }
    *flag = done;
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::setRefSpeed(int j, double sp)
{
    yCTrace(YORCB) << j << sp;
    double min, max;
    getVelLimits(j, &min, &max);

    if (sp > max)
    {
        yCWarning(YORCB) << "Setting" << sp << "refSpeed above" << max << "maxVelLimit, all joint" << j << "movements will be immediate";
    }

    refSpeeds[j] = sp;
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::setRefSpeeds(const double *spds)
{
    yCTrace(YORCB);
    bool ok = true;
    for (unsigned int i = 0; i < axes; i++)
        ok &= setRefSpeed(i, spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::setRefAcceleration(int j, double acc)
{
    yCError(YORCB) << "setRefAcceleration() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::setRefAccelerations(const double *accs)
{
    yCError(YORCB) << "setRefAccelerations() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::getRefSpeed(int j, double *ref)
{
    yCTrace(YORCB) << j;
    *ref = refSpeeds[j];
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::getRefSpeeds(double *spds)
{
    yCTrace(YORCB);
    bool ok = true;
    for (unsigned int i = 0; i < axes; i++)
        ok &= getRefSpeed(i, &spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::getRefAcceleration(int j, double *acc)
{
    yCError(YORCB) << "getRefAcceleration() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::getRefAccelerations(double *accs)
{
    yCError(YORCB) << "getRefAccelerations() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::stop(int j)
{
    yCTrace(YORCB) << j;
    OpenRAVE::dReal dofCurrentRads = vectorOfJointPtr[j]->GetValue(0);
    std::vector<OpenRAVE::dReal> tmp;
    tmp.push_back(dofCurrentRads);
    pcontrols[j]->SetDesired(tmp);
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::stop()
{
    yCTrace(YORCB);
    bool ok = true;
    for (unsigned int i = 0; i < axes; i++)
        ok &= stop(i);
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::positionMove(const int n_joint, const int *joints, const double *refs)
{
    yCTrace(YORCB) << n_joint;
    bool ok = true;
    for (int i = 0; i < n_joint; i++)
    {
        ok &= positionMove(joints[i], refs[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::relativeMove(const int n_joint, const int *joints, const double *deltas)
{
    yCTrace(YORCB) << n_joint;
    bool ok = true;
    for (int i = 0; i < n_joint; i++)
    {
        ok &= relativeMove(joints[i], deltas[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::checkMotionDone(const int n_joint, const int *joints, bool *flag)
{
    yCTrace(YORCB) << n_joint;
    bool ok = true;
    bool done = true;
    for (int i = 0; i < n_joint; i++)
    {
        bool tmpDone;
        ok &= checkMotionDone(joints[i], &tmpDone);
        done &= tmpDone;
    }
    *flag = done;
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::setRefSpeeds(const int n_joint, const int *joints, const double *spds)
{
    yCTrace(YORCB) << n_joint;
    bool ok = true;
    for (int i = 0; i < n_joint; i++)
    {
        ok &= setRefSpeed(joints[i], spds[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
    yCTrace(YORCB) << n_joint;
    bool ok = true;
    for (int i = 0; i < n_joint; i++)
    {
        ok &= setRefAcceleration(joints[i], accs[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::getRefSpeeds(const int n_joint, const int *joints, double *spds)
{
    yCTrace(YORCB) << n_joint;
    bool ok = true;
    for (int i = 0; i < n_joint; i++)
    {
        ok &= getRefSpeed(joints[i], &spds[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    yCTrace(YORCB) << n_joint;
    bool ok = true;
    for (int i = 0; i < n_joint; i++)
    {
        ok &= getRefAcceleration(joints[i], &accs[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::stop(const int n_joint, const int *joints)
{
    yCTrace(YORCB) << n_joint;
    bool ok = true;
    for (unsigned int i = 0; i < n_joint; i++)
        ok &= stop(joints[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::getTargetPosition(const int joint, double *ref)
{
    yCError(YORCB) << "getTargetPosition() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::getTargetPositions(double *refs)
{
    yCError(YORCB) << "getTargetPositions() not implemented";
    return false;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveControlBoard::getTargetPositions(const int n_joint, const int *joints, double *refs)
{
    yCError(YORCB) << "getTargetPositions() not implemented";
    return false;
}

// -----------------------------------------------------------------------------
