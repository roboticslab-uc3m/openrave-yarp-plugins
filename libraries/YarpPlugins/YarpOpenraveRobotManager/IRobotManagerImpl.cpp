// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRobotManager.hpp"

namespace roboticslab
{

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveRobotManager::moveForward(int velocity)
{
    CD_DEBUG("\n");

    switch (mode)
    {
    case TRANSFORM_IDEALCONTROLLER:
    {
        //pcontrol->SetDesired(values,transform);

        OpenRAVE::Vector H_0_robot = probot->GetTransform().trans;
        CD_DEBUG("Robot at: %f %f %f\n", H_0_robot.x, H_0_robot.y, H_0_robot.z);

        //-- Our own ConfigurationSpecification
        OpenRAVE::ConfigurationSpecification oneDofConfigurationSpecification;

        //-- Add the linear interpolation tag to the joint_values group
        OpenRAVE::ConfigurationSpecification::Group joint_values;
        //std::string joint_valuesName("joint_values ");
        std::string joint_valuesName("affine_transform  ");
        joint_valuesName.append(robotName);
        joint_valuesName.append(" ");
        std::stringstream ss;
        //ss << manipulatorIDs[ j ];
        //ss << "0 ";
        ss << OpenRAVE::DOF_X;
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
        for (size_t i = 0; i < oneDofConfigurationSpecification._vgroups.size(); i++)
        {
            OpenRAVE::ConfigurationSpecification::Group g = oneDofConfigurationSpecification._vgroups[i];
            CD_DEBUG("[%d] %s, %d, %d, %s\n",i,g.name.c_str(), g.offset, g.dof, g.interpolation.c_str());
        }

        OpenRAVE::TrajectoryBasePtr ptraj = OpenRAVE::RaveCreateTrajectory(penv,"");

        ptraj->Init(oneDofConfigurationSpecification);

        OpenRAVE::dReal dofCurrentRads = H_0_robot.x;
        //OpenRAVE::dReal dofCurrentRads = vectorOfJointPtr[j]->GetValue(0);

        OpenRAVE::dReal dofTargetRads = dofCurrentRads + velocity;

        OpenRAVE::dReal dofTime = 2.0;
        //OpenRAVE::dReal dofTime = abs( ( dofTargetRads - dofCurrentRads ) / ( degToRadIfNotPrismatic(j,refSpeeds[j]) ) ); // Time in seconds

        //CD_DEBUG("[%d] abs(target-current)/vel = abs(%f-%f)/%f = %f [s]\n",j,ref,radToDegIfNotPrismatic(j,dofCurrentRads),refSpeeds[ j ],dofTime);

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
        pcontrol->SetPath(ptraj);

        break;
    }
    case FOUR_WHEEL_IDEALVELOCITYCONTROLLER:
    {
        std::vector<OpenRAVE::dReal> values(4, velocity);
        pcontrol->SetDesired(values);
        break;
    }
    default:
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::moveBackwards(int velocity)
{
    CD_DEBUG("\n");
    std::vector<OpenRAVE::dReal> values(4, -velocity);
    pcontrol->SetDesired(values);
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::turnLeft(int velocity)
{
    CD_DEBUG("\n");
    std::vector<OpenRAVE::dReal> values(4);
    values[0] = -velocity;
    values[1] = velocity;
    values[2] = -velocity;
    values[3] = velocity;
    pcontrol->SetDesired(values);
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::turnRight(int velocity)
{
    CD_DEBUG("\n");
    std::vector<OpenRAVE::dReal> values(4);
    values[0] = velocity;
    values[1] = -velocity;
    values[2] = velocity;
    values[3] = -velocity;
    pcontrol->SetDesired(values);
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::stopMovement()
{
    CD_DEBUG("\n");
    std::vector<OpenRAVE::dReal> values(4, 0.0);
    pcontrol->SetDesired(values);
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::tiltUp(int velocity)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::tiltDown(int velocity)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::panLeft(int velocity)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::panRight(int velocity)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::stopCameraMovement()
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::connect()
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::disconnect()
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::test()
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

void YarpOpenraveRobotManager::setEnabled(bool enabled)
{
    CD_DEBUG("\n");
    return;
}

// -----------------------------------------------------------------------------

void YarpOpenraveRobotManager::onDestroy()
{
    CD_DEBUG("\n");
    return;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab
