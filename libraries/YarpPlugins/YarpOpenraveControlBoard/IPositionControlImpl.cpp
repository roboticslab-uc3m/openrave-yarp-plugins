// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlBoard.hpp"

#include <sstream>
#include <string>
#include <vector>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------- IPositionControl Related --------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getAxes(std::size_t & ax)
{
    ax = axes;
    return yarp::dev::ReturnValue_ok;
}
#else
bool YarpOpenraveControlBoard::getAxes(int * ax)
{
    *ax = axes;
    return true;
}
#endif

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::positionMove(int j, double ref)
#else
bool YarpOpenraveControlBoard::positionMove(int j, double ref)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "positionMove: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    //-- Check if we are in position mode.
    if (controlModes[j] != VOCAB_CM_POSITION)
    {
        yCError(YORCB) << "Will not positionMove() as joint" << j << "not in positionMode";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_not_ready;
#else
        return false;
#endif
    }

    OpenRAVE::dReal dofTargetRads = degToRadIfNotPrismatic(j, ref); // ref comes in exposed

    //-- Store the targets
    manipulatorTargetRads[j] = dofTargetRads;

    //-- But do not move if no velocity
    if (refSpeeds[j] == 0)
    {
        yCDebug(YORCB, "[%d] (refSpeeds[ j ] == 0) => Avoid division by 0 => Just act like blocked joint, return true", j);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_ok;
#else
        return true;
#endif
    }

    {
        OpenRAVE::EnvironmentLock lock(penv->GetMutex()); // lock environment

        //-- Check and do immediate movement if appropriate.
        //-- In fact, OpenRAVE would actually do the extra-fast movement but warn at all times.
        double min, max;
        getVelLimits(j, &min, &max);

        if (refSpeeds[j] > max)
        {
            std::vector<OpenRAVE::dReal> tmp;
            tmp.push_back(dofTargetRads);
            pcontrols[j]->SetDesired(tmp);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
            return yarp::dev::ReturnValue_ok;
#else
            return true;
#endif
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

        OpenRAVE::dReal dofTime = std::abs((dofTargetRads - dofCurrentRads) / degToRadIfNotPrismatic(j, refSpeeds[j])); // Time in seconds

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

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::positionMove(const double * refs)
#else
bool YarpOpenraveControlBoard::positionMove(const double * refs)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
        ok &= positionMove(i, refs[i]);
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok ? yarp::dev::ReturnValue_ok : yarp::dev::ReturnValue_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::positionMove(int n_joint, const int * joints, const double * refs)
#else
bool YarpOpenraveControlBoard::positionMove(int n_joint, const int * joints, const double * refs)
#endif
{
    bool ok = true;

    for (int i = 0; i < n_joint; i++)
    {
        ok &= positionMove(joints[i], refs[i]);
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok ? yarp::dev::ReturnValue_ok : yarp::dev::ReturnValue_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::relativeMove(int j, double delta)
#else
bool YarpOpenraveControlBoard::relativeMove(int j, double delta)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "relativeMove: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    double v = radToDegIfNotPrismatic(j, vectorOfJointPtr[j]->GetValue(0) );

    return positionMove(j, v + delta);
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::relativeMove(const double * deltas)
#else
bool YarpOpenraveControlBoard::relativeMove(const double * deltas)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
        ok &= relativeMove(i, deltas[i]);
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok ? yarp::dev::ReturnValue_ok : yarp::dev::ReturnValue_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::relativeMove(int n_joint, const int * joints, const double * deltas)
#else
bool YarpOpenraveControlBoard::relativeMove(int n_joint, const int * joints, const double * deltas)
#endif
{
    bool ok = true;

    for (int i = 0; i < n_joint; i++)
    {
        ok &= relativeMove(joints[i], deltas[i]);
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok ? yarp::dev::ReturnValue_ok : yarp::dev::ReturnValue_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::checkMotionDone(int j, bool & flag)
#else
bool YarpOpenraveControlBoard::checkMotionDone(int j, bool * flag)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "checkMotionDone: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_input_out_of_bounds;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    flag = pcontrols[j]->IsDone();
    return yarp::dev::ReturnValue_ok;
#else
    *flag = pcontrols[j]->IsDone();
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::checkMotionDone(bool & flag)
#else
bool YarpOpenraveControlBoard::checkMotionDone(bool * flag)
#endif
{
    bool done = true;

    for (unsigned int j = 0; j < axes; j++)
    {
        bool tmpDone;
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        checkMotionDone(j, tmpDone);
#else
        checkMotionDone(j, &tmpDone);
#endif
        done &= tmpDone;
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    flag = done;
    return yarp::dev::ReturnValue_ok;
#else
    *flag = done;
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::checkMotionDone(const std::vector<int> & joints, bool & flag)
#else
bool YarpOpenraveControlBoard::checkMotionDone(int n_joint, const int * joints, bool * flag)
#endif
{
    bool ok = true;
    bool done = true;

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    for (int i = 0; i < joints.size(); i++)
#else
    for (int i = 0; i < n_joint; i++)
#endif
    {
        bool tmpDone;
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        ok &= checkMotionDone(joints[i], tmpDone);
#else
        ok &= checkMotionDone(joints[i], &tmpDone);
#endif
        done &= tmpDone;
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    flag = done;
    return yarp::dev::ReturnValue_ok;
#else
    *flag = done;
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setTrajSpeed(int j, double sp)
#else
bool YarpOpenraveControlBoard::setRefSpeed(int j, double sp)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "setTrajSpeed: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    double min, max;
    getVelLimits(j, &min, &max);

    if (sp > max)
    {
        yCWarning(YORCB) << "Setting" << sp << "refSpeed above" << max << "maxVelLimit, all joint" << j << "movements will be immediate";
    }

    refSpeeds[j] = sp;
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setTrajSpeeds(const double * spds)
#else
bool YarpOpenraveControlBoard::setRefSpeeds(const double * spds)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        ok &= setTrajSpeed(i, spds[i]);
#else
        ok &= setRefSpeed(i, spds[i]);
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok ? yarp::dev::ReturnValue_ok : yarp::dev::ReturnValue_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setTrajSpeeds(int n_joint, const int * joints, const double * spds)
#else
bool YarpOpenraveControlBoard::setRefSpeeds(int n_joint, const int * joints, const double * spds)
#endif
{
    bool ok = true;

    for (int i = 0; i < n_joint; i++)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        ok &= setTrajSpeed(joints[i], spds[i]);
#else
        ok &= setRefSpeed(joints[i], spds[i]);
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok ? yarp::dev::ReturnValue_ok : yarp::dev::ReturnValue_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setTrajAcceleration(int j, double acc)
#else
bool YarpOpenraveControlBoard::setRefAcceleration(int j, double acc)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "setTrajAcceleration: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    yCError(YORCB) << "setTrajAcceleration() not implemented";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_error_not_implemented_by_device;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setTrajAccelerations(const double * accs)
#else
bool YarpOpenraveControlBoard::setRefAccelerations(const double * accs)
#endif
{
    yCError(YORCB) << "setTrajAccelerations() not implemented";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_error_not_implemented_by_device;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setTrajAccelerations(int n_joint, const int * joints, const double * accs)
#else
bool YarpOpenraveControlBoard::setRefAccelerations(int n_joint, const int * joints, const double * accs)
#endif
{
    bool ok = true;

    for (int i = 0; i < n_joint; i++)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        ok &= setTrajAcceleration(joints[i], accs[i]);
#else
        ok &= setRefAcceleration(joints[i], accs[i]);
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok ? yarp::dev::ReturnValue_ok : yarp::dev::ReturnValue_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getTrajSpeed(int j, double *ref)
#else
bool YarpOpenraveControlBoard::getRefSpeed(int j, double *ref)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "getTrajSpeed: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    *ref = refSpeeds[j];
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getTrajSpeeds(double * spds)
#else
bool YarpOpenraveControlBoard::getRefSpeeds(double * spds)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        ok &= getTrajSpeed(i, &spds[i]);
#else
        ok &= getRefSpeed(i, &spds[i]);
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok ? yarp::dev::ReturnValue_ok : yarp::dev::ReturnValue_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getTrajSpeeds(int n_joint, const int * joints, double * spds)
#else
bool YarpOpenraveControlBoard::getRefSpeeds(int n_joint, const int * joints, double * spds)
#endif
{
    bool ok = true;

    for (int i = 0; i < n_joint; i++)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        ok &= getTrajSpeed(joints[i], &spds[i]);
#else
        ok &= getRefSpeed(joints[i], &spds[i]);
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok ? yarp::dev::ReturnValue_ok : yarp::dev::ReturnValue_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getTrajAcceleration(int j, double * acc)
#else
bool YarpOpenraveControlBoard::getRefAcceleration(int j, double * acc)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "getTrajAcceleration: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    yCError(YORCB) << "getTrajAcceleration() not implemented";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_error_not_implemented_by_device;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getTrajAccelerations(double * accs)
#else
bool YarpOpenraveControlBoard::getRefAccelerations(double * accs)
#endif
{
    yCError(YORCB) << "getTrajAccelerations() not implemented";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_error_not_implemented_by_device;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getTrajAccelerations(int n_joint, const int * joints, double * accs)
#else
bool YarpOpenraveControlBoard::getRefAccelerations(int n_joint, const int * joints, double * accs)
#endif
{
    bool ok = true;

    for (int i = 0; i < n_joint; i++)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        ok &= getTrajAcceleration(joints[i], &accs[i]);
#else
        ok &= getRefAcceleration(joints[i], &accs[i]);
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok ? yarp::dev::ReturnValue_ok : yarp::dev::ReturnValue_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::stop(int j)
#else
bool YarpOpenraveControlBoard::stop(int j)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "stop: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    OpenRAVE::dReal dofCurrentRads = vectorOfJointPtr[j]->GetValue(0);
    std::vector<OpenRAVE::dReal> tmp;
    tmp.push_back(dofCurrentRads);
    pcontrols[j]->SetDesired(tmp);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_ok;
#else
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::stop()
#else
bool YarpOpenraveControlBoard::stop()
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
        ok &= stop(i);
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok ? yarp::dev::ReturnValue_ok : yarp::dev::ReturnValue_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::stop(int n_joint, const int * joints)
#else
bool YarpOpenraveControlBoard::stop(int n_joint, const int * joints)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < n_joint; i++)
    {
        ok &= stop(joints[i]);
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok ? yarp::dev::ReturnValue_ok : yarp::dev::ReturnValue_error_method_failed;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getTargetPosition(int j, double * ref)
#else
bool YarpOpenraveControlBoard::getTargetPosition(int j, double * ref)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "getTargetPosition: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue_error_input_out_of_bounds;
#else
        return false;
#endif
    }

    yCError(YORCB) << "getTargetPosition() not implemented";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_error_not_implemented_by_device;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getTargetPositions(double * refs)
#else
bool YarpOpenraveControlBoard::getTargetPositions(double * refs)
#endif
{
    yCError(YORCB) << "getTargetPositions() not implemented";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_error_not_implemented_by_device;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getTargetPositions(int n_joint, const int * joints, double * refs)
#else
bool YarpOpenraveControlBoard::getTargetPositions(int n_joint, const int * joints, double * refs)
#endif
{
    yCError(YORCB) << "getTargetPositions() not implemented";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue_error_not_implemented_by_device;
#else
    return false;
#endif
}

// -----------------------------------------------------------------------------
