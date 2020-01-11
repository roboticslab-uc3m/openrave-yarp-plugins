// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <sstream>
#include <string>
#include <vector>

#include <ColorDebug.h>

// ------------------- IPositionControl Related --------------------------------

bool roboticslab::YarpOpenraveControlboard::getAxes(int *ax) {
    *ax = axes;
    CD_INFO("Reporting %d axes are present\n", *ax);
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::positionMove(int j, double ref) {  // encExposed = ref;
    //CD_INFO("\n");

    //-- Check if we are in position mode.
    if( controlModes[j] != VOCAB_CM_POSITION )
    {
        CD_ERROR("Will not positionMove as not in positionMode (%d)\n",j);
        return false;
    }

    OpenRAVE::dReal dofTargetRads = degToRadIfNotPrismatic(j, ref);  // ref comes in exposed

    //-- Store the targets
    manipulatorTargetRads[ j ] = dofTargetRads;

    //-- But do not move if no velocity
    if( refSpeeds[ j ] == 0 )
    {
        CD_DEBUG("[%d] (refSpeeds[ j ] == 0) => Avoid division by 0 => Just act like blocked joint, return true.\n",j);
        return true;
    }

    {
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment

        //-- Check and do immediate movement if appropriate.
        //-- In fact, OpenRAVE would actually do the extra-fast movement but warn at all times.
        double min,max;
        getVelLimits( j, &min, &max );
        if( refSpeeds[ j ] > max )
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
        //    CD_DEBUG("activeDOFIndices[%d]: %d\n",i,activeDOFIndices[i]);
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
        ss << manipulatorIDs[ j ];
        joint_valuesName.append(ss.str());
        joint_values.name = joint_valuesName;
        joint_values.offset = 0;
        joint_values.dof = 1;
        joint_values.interpolation = "linear";
        oneDofConfigurationSpecification.AddGroup(joint_values);

        //-- Add a required deltatime group
        //-- Perhaps also could be done via: int timeoffset = spec.AddDeltaTimeGroup();
        OpenRAVE::ConfigurationSpecification::Group deltatime;
        deltatime.name="deltatime";
        deltatime.offset=1;
        deltatime.dof=1;
        deltatime.interpolation="";
        oneDofConfigurationSpecification.AddGroup(deltatime);

        OpenRAVE::ConfigurationSpecification::Group iswaypoint;
        iswaypoint.name="iswaypoint";
        iswaypoint.offset=2;
        iswaypoint.dof=1;
        iswaypoint.interpolation="next";
        oneDofConfigurationSpecification.AddGroup(iswaypoint);

        //-- Console output of the manually adjusted ConfigurationSpecification
        //for (size_t i = 0; i < oneDofConfigurationSpecification._vgroups.size(); i++)
        //{
        //    OpenRAVE::ConfigurationSpecification::Group g = oneDofConfigurationSpecification._vgroups[i];
        //    CD_DEBUG("[%d] %s, %d, %d, %s\n",i,g.name.c_str(), g.offset, g.dof, g.interpolation.c_str());
        //}

        OpenRAVE::TrajectoryBasePtr ptraj = OpenRAVE::RaveCreateTrajectory(penv,"");

        ptraj->Init(oneDofConfigurationSpecification);

        OpenRAVE::dReal dofCurrentRads = vectorOfJointPtr[j]->GetValue(0);

        OpenRAVE::dReal dofTime = std::abs( ( dofTargetRads - dofCurrentRads ) / ( degToRadIfNotPrismatic(j,refSpeeds[j]) ) ); // Time in seconds

        CD_DEBUG("[%d] abs(target-current)/vel = abs(%f-%f)/%f = %f [s]\n",j,ref,radToDegIfNotPrismatic(j,dofCurrentRads),refSpeeds[ j ],dofTime);

        //-- ptraj[0] with positions it has now, with: 0 deltatime, 1 iswaypoint
        std::vector<OpenRAVE::dReal> dofCurrentFull(3);
        dofCurrentFull[0] = dofCurrentRads;  // joint_values
        dofCurrentFull[1] = 0;           // deltatime
        dofCurrentFull[2] = 1;           // iswaypoint
        ptraj->Insert(0,dofCurrentFull);

        //-- ptraj[1] with position targets, with: 1 deltatime, 1 iswaypoint
        std::vector<OpenRAVE::dReal> dofTargetFull(3);
        dofTargetFull[0] = dofTargetRads;  // joint_values
        dofTargetFull[1] = dofTime;    // deltatime
        dofTargetFull[2] = 1;          // iswaypoint
        ptraj->Insert(1,dofTargetFull);

        //-- SetPath makes the controller perform the trajectory
        pcontrols[j]->SetPath(ptraj);

    }

    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::positionMove(const double *refs) {  // encExposed = refs;
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= positionMove(i,refs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::relativeMove(int j, double delta) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::relativeMove(const double *deltas) {  // encExposed = deltas + encExposed
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::checkMotionDone(int j, bool *flag) {
    //CD_INFO("\n");
    *flag = pcontrols[j]->IsDone();
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::checkMotionDone(bool *flag) {
    CD_INFO("\n");
    bool done = true;
    for(unsigned int j=0;j<axes;j++)
    {
        bool tmpDone;
        checkMotionDone(j,&tmpDone);
        done &= tmpDone;
    }
    *flag = done;
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefSpeed(int j, double sp) {
    CD_INFO("\n");
    double min,max;
    getVelLimits( j, &min, &max );
    if( sp > max )
    {
        CD_WARNING("Setting %f refSpeed above %f maxVelLimit. All joint %d movements will be immediate.\n",sp,max,j);
    }
    refSpeeds[j] = sp;
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefSpeeds(const double *spds) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setRefSpeed(i,spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefAcceleration(int j, double acc) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefAccelerations(const double *accs) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setRefAcceleration(i,accs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefSpeed(int j, double *ref) {
    CD_INFO("\n");
    *ref = refSpeeds[j];
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefSpeeds(double *spds) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getRefSpeed(i,&spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefAcceleration(int j, double *acc) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefAccelerations(double *accs) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getRefAcceleration(i,&accs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::stop(int j) {
    CD_INFO("\n");
    OpenRAVE::dReal dofCurrentRads = vectorOfJointPtr[j]->GetValue(0);
    std::vector<OpenRAVE::dReal> tmp;
    tmp.push_back(dofCurrentRads);
    pcontrols[j]->SetDesired(tmp);
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::stop() {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= stop(i);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::positionMove(const int n_joint, const int *joints, const double *refs)
{
    CD_INFO("\n");
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= positionMove(joints[i],refs[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::relativeMove(const int n_joint, const int *joints, const double *deltas)
{
    CD_INFO("\n");
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= relativeMove(joints[i],deltas[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::checkMotionDone(const int n_joint, const int *joints, bool *flag)
{
    //CD_INFO("\n");
    bool ok = true;
    bool done = true;
    for(int i=0;i<n_joint;i++)
    {
        bool tmpDone;
        ok &= checkMotionDone(joints[i],&tmpDone);
        done &= tmpDone;
    }
    *flag = done;
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefSpeeds(const int n_joint, const int *joints, const double *spds)
{
    CD_INFO("\n");
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= setRefSpeed(joints[i],spds[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
    CD_INFO("\n");
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= setRefAcceleration(joints[i],accs[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefSpeeds(const int n_joint, const int *joints, double *spds)
{
    CD_INFO("\n");
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= getRefSpeed(joints[i],&spds[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    CD_INFO("\n");
    bool ok = true;
    for(int i=0;i<n_joint;i++)
    {
        ok &= getRefAcceleration(joints[i],&accs[i]);
    }
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::stop(const int n_joint, const int *joints)
{
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<n_joint;i++)
        ok &= stop(joints[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTargetPosition(const int joint, double *ref)
{
    CD_ERROR("Not implemented yet.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTargetPositions(double *refs)
{
    CD_ERROR("Not implemented yet.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getTargetPositions(const int n_joint, const int *joints, double *refs)
{
    CD_ERROR("Not implemented yet.\n");
    return true;
}

// -----------------------------------------------------------------------------
