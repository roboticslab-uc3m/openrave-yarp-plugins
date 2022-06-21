// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRobotManager.hpp"

#include <cmath> // std::abs

#include <sstream>
#include <string>
#include <vector>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveRobotManager::moveForward(double value)
{
    yCTrace(YORRM) << value;

    switch (mode)
    {
    case TRANSFORM_IDEALCONTROLLER:
    {
        //pcontrol->SetDesired(values,transform);

        OpenRAVE::dReal refVelocity = 3.0;  // [m/s]
        OpenRAVE::dReal dofTime = std::abs( OpenRAVE::dReal(value)  / refVelocity ); // Time in seconds
        yCDebug(YORRM, "abs(increment/vel) = abs(%f/%f) = %f [s]",value,refVelocity,dofTime);

        OpenRAVE::Transform H_0_src = probot->GetTransform();
        yCDebug(YORRM) << "H_0_src:" << H_0_src.trans.x << H_0_src.trans.y << H_0_src.trans.z << "|" << H_0_src.rot.x << H_0_src.rot.y << H_0_src.rot.z << H_0_src.rot.w;
        //OpenRAVE::Vector H_0_src_axisAngle = OpenRAVE::geometry::axisAngleFromQuat(H_0_src.rot);
        //yCDebug(YORRM, "H_0_src_aa: %f %f %f %f", H_0_src_axisAngle.x, H_0_src_axisAngle.y, H_0_src_axisAngle.z, H_0_src_axisAngle.w);

        OpenRAVE::Transform H_src_dst;
        H_src_dst.trans.x = value;

        OpenRAVE::Transform H_0_dst = H_0_src * H_src_dst;
        yCDebug(YORRM) << "H_0_dst:" << H_0_dst.trans.x << H_0_dst.trans.y << H_0_dst.trans.z << "|" << H_0_dst.rot.x << H_0_dst.rot.y << H_0_dst.rot.z << H_0_dst.rot.w;
        //OpenRAVE::Vector H_0_dst_axisAngle = OpenRAVE::geometry::axisAngleFromQuat(H_0_dst.rot);
        //yCDebug(YORRM, "H_0_dst_aa: %f %f %f %f", H_0_dst_axisAngle.x, H_0_dst_axisAngle.y, H_0_dst_axisAngle.z, H_0_dst_axisAngle.w);

        //-- Our own ConfigurationSpecification
        //Alternative: OpenRAVE::ConfigurationSpecification sc = OpenRAVE::RaveGetAffineConfigurationSpecification(OpenRAVE::DOF_X);
        OpenRAVE::ConfigurationSpecification configurationSpecification;

        //-- Add the linear interpolation tag to the joint_values group
        OpenRAVE::ConfigurationSpecification::Group joint_values;
        std::string joint_valuesName("affine_transform  ");  // Analogous to "joint_values ".
        joint_valuesName.append(robotName);  // Checked that this is required even when examples put "__dummy__".
        joint_valuesName.append(" ");
        std::stringstream ss;
        //ss << manipulatorIDs[ j ];
        //ss << "0 ";
        ss << (OpenRAVE::DOF_X|OpenRAVE::DOF_Y|OpenRAVE::DOF_RotationAxis);
        joint_valuesName.append(ss.str());
        joint_values.name = joint_valuesName;
        joint_values.offset = 0;
        joint_values.dof = 3;
        joint_values.interpolation = "linear";
        configurationSpecification.AddGroup(joint_values);

        //-- Add a required deltatime group
        //-- Perhaps also could be done via: int timeoffset = spec.AddDeltaTimeGroup();
        OpenRAVE::ConfigurationSpecification::Group deltatime;
        deltatime.name="deltatime";
        deltatime.offset=3;
        deltatime.dof=1;
        deltatime.interpolation="";
        configurationSpecification.AddGroup(deltatime);

        OpenRAVE::ConfigurationSpecification::Group iswaypoint;
        iswaypoint.name="iswaypoint";
        iswaypoint.offset=4;
        iswaypoint.dof=1;
        iswaypoint.interpolation="next";
        configurationSpecification.AddGroup(iswaypoint);

        //-- Console output of the manually adjusted ConfigurationSpecification
        //for (size_t i = 0; i < configurationSpecification._vgroups.size(); i++)
        //{
        //    OpenRAVE::ConfigurationSpecification::Group g = configurationSpecification._vgroups[i];
        //    yCDebug(YORRM, "[%d] %s, %d, %d, %s",i,g.name.c_str(), g.offset, g.dof, g.interpolation.c_str());
        //}

        OpenRAVE::TrajectoryBasePtr ptraj = OpenRAVE::RaveCreateTrajectory(penv,"");

        ptraj->Init(configurationSpecification);

        //-- ptraj[0] with positions it has now, with: 0 deltatime, 1 iswaypoint
        std::vector<OpenRAVE::dReal> dofCurrentFull(5);
        dofCurrentFull[0] = H_0_src.trans.x;  // joint_values
        dofCurrentFull[1] = H_0_src.trans.y;  // joint_values
        dofCurrentFull[2] = 2.0*asin(H_0_src.rot.w); // bad sign: 2.0*acos(H_0_src.rot.x);  // joint_values
        dofCurrentFull[3] = 0;           // deltatime
        dofCurrentFull[4] = 1;           // iswaypoint
        ptraj->Insert(0,dofCurrentFull);

        //-- ptraj[1] with position targets, with: 1 deltatime, 1 iswaypoint
        std::vector<OpenRAVE::dReal> dofTargetFull(5);
        dofTargetFull[0] = H_0_dst.trans.x;  // joint_values
        dofTargetFull[1] = H_0_dst.trans.y;  // joint_values
        dofTargetFull[2] = 2.0*asin(H_0_dst.rot.w); // bad sign: 2.0*acos(H_0_dst.rot.x);  // joint_values
        dofTargetFull[3] = dofTime;    // deltatime
        dofTargetFull[4] = 1;          // iswaypoint
        ptraj->Insert(1,dofTargetFull);

        //-- SetPath makes the controller perform the trajectory
        pcontrol->SetPath(ptraj);

        break;
    }
    case FOUR_WHEEL_IDEALVELOCITYCONTROLLER:
    {
        std::vector<OpenRAVE::dReal> values(4, value);
        pcontrol->SetDesired(values);
        break;
    }
    default:
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::turnLeft(double value)
{
    yCTrace(YORRM) << value;

    switch (mode)
    {
    case TRANSFORM_IDEALCONTROLLER:
    {
        //pcontrol->SetDesired(values,transform);

        OpenRAVE::dReal refVelocity = 90.0;  // [deg/s]
        OpenRAVE::dReal dofTime = std::abs( OpenRAVE::dReal(value)  / refVelocity ); // Time in seconds
        yCDebug(YORRM, "abs(increment/vel) = abs(%f/%f) = %f [s]",value,refVelocity,dofTime);

        OpenRAVE::Transform H_0_src = probot->GetTransform();
        yCDebug(YORRM) << "H_0_src:" << H_0_src.trans.x << H_0_src.trans.y << H_0_src.trans.z << "|" << H_0_src.rot.x << H_0_src.rot.y << H_0_src.rot.z << H_0_src.rot.w;
        //OpenRAVE::Vector H_0_src_axisAngle = OpenRAVE::geometry::axisAngleFromQuat(H_0_src.rot);
        //yCDebug(YORRM, "H_0_src_aa: %f %f %f %f", H_0_src_axisAngle.x, H_0_src_axisAngle.y, H_0_src_axisAngle.z, H_0_src_axisAngle.w);

        OpenRAVE::dReal relAngleRad = OpenRAVE::dReal(value) * M_PI / 180.0;

        OpenRAVE::Transform H_src_dst;
        H_src_dst.rot.x = cos( relAngleRad / 2.0);
        H_src_dst.rot.y = 0.0;
        H_src_dst.rot.z = 0.0;
        H_src_dst.rot.w = sin( relAngleRad / 2.0);

        OpenRAVE::Transform H_0_dst = H_0_src * H_src_dst;
        yCDebug(YORRM) << "H_0_dst:" << H_0_dst.trans.x << H_0_dst.trans.y << H_0_dst.trans.z << "|" << H_0_dst.rot.x << H_0_dst.rot.y << H_0_dst.rot.z << H_0_dst.rot.w;
        //OpenRAVE::Vector H_0_dst_axisAngle = OpenRAVE::geometry::axisAngleFromQuat(H_0_dst.rot);
        //yCDebug(YORRM, "H_0_dst_aa: %f %f %f %f", H_0_dst_axisAngle.x, H_0_dst_axisAngle.y, H_0_dst_axisAngle.z, H_0_dst_axisAngle.w);


        //-- Our own ConfigurationSpecification
        //Alternative: OpenRAVE::ConfigurationSpecification sc = OpenRAVE::RaveGetAffineConfigurationSpecification(OpenRAVE::DOF_X);
        OpenRAVE::ConfigurationSpecification configurationSpecification;

        //-- Add the linear interpolation tag to the joint_values group
        OpenRAVE::ConfigurationSpecification::Group joint_values;
        std::string joint_valuesName("affine_transform  ");  // Analogous to "joint_values ".
        joint_valuesName.append(robotName);  // Checked that this is required even when examples put "__dummy__".
        joint_valuesName.append(" ");
        std::stringstream ss;
        //ss << manipulatorIDs[ j ];
        //ss << "0 ";
        ss << (OpenRAVE::DOF_X|OpenRAVE::DOF_Y|OpenRAVE::DOF_RotationAxis);
        joint_valuesName.append(ss.str());
        joint_values.name = joint_valuesName;
        joint_values.offset = 0;
        joint_values.dof = 3;
        joint_values.interpolation = "linear";
        configurationSpecification.AddGroup(joint_values);

        //-- Add a required deltatime group
        //-- Perhaps also could be done via: int timeoffset = spec.AddDeltaTimeGroup();
        OpenRAVE::ConfigurationSpecification::Group deltatime;
        deltatime.name="deltatime";
        deltatime.offset=3;
        deltatime.dof=1;
        deltatime.interpolation="";
        configurationSpecification.AddGroup(deltatime);

        OpenRAVE::ConfigurationSpecification::Group iswaypoint;
        iswaypoint.name="iswaypoint";
        iswaypoint.offset=4;
        iswaypoint.dof=1;
        iswaypoint.interpolation="next";
        configurationSpecification.AddGroup(iswaypoint);

        //-- Console output of the manually adjusted ConfigurationSpecification
        //for (size_t i = 0; i < configurationSpecification._vgroups.size(); i++)
        //{
        //    OpenRAVE::ConfigurationSpecification::Group g = configurationSpecification._vgroups[i];
        //    yCDebug(YORRM, "[%d] %s, %d, %d, %s",i,g.name.c_str(), g.offset, g.dof, g.interpolation.c_str());
        //}

        OpenRAVE::TrajectoryBasePtr ptraj = OpenRAVE::RaveCreateTrajectory(penv,"");

        ptraj->Init(configurationSpecification);

        //-- ptraj[0] with positions it has now, with: 0 deltatime, 1 iswaypoint
        std::vector<OpenRAVE::dReal> dofCurrentFull(5);
        dofCurrentFull[0] = H_0_src.trans.x;  // joint_values
        dofCurrentFull[1] = H_0_src.trans.y;  // joint_values
        dofCurrentFull[2] = 2.0*asin(H_0_src.rot.w); // bad sign: 2.0*acos(H_0_src.rot.x);  // joint_values
        dofCurrentFull[3] = 0;           // deltatime
        dofCurrentFull[4] = 1;           // iswaypoint
        ptraj->Insert(0,dofCurrentFull);

        //-- ptraj[1] with position targets, with: 1 deltatime, 1 iswaypoint
        std::vector<OpenRAVE::dReal> dofTargetFull(5);
        dofTargetFull[0] = H_0_dst.trans.x;  // joint_values
        dofTargetFull[1] = H_0_dst.trans.y;  // joint_values
        dofTargetFull[2] = 2.0*asin(H_0_dst.rot.w); // bad sign: 2.0*acos(H_0_dst.rot.x);  // joint_values
        dofTargetFull[3] = dofTime;    // deltatime
        dofTargetFull[4] = 1;          // iswaypoint
        ptraj->Insert(1,dofTargetFull);

        //-- SetPath makes the controller perform the trajectory
        pcontrol->SetPath(ptraj);

        break;
    }
    case FOUR_WHEEL_IDEALVELOCITYCONTROLLER:
    {
        std::vector<OpenRAVE::dReal> values(4);
        values[0] = -value;
        values[1] = value;
        values[2] = -value;
        values[3] = value;
        pcontrol->SetDesired(values);
        break;
    }
    default:
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::stopMovement()
{
    yCTrace(YORRM);
    switch (mode)
    {
    case TRANSFORM_IDEALCONTROLLER:
    {
        moveForward(0);
        break;
    }
    case FOUR_WHEEL_IDEALVELOCITYCONTROLLER:
    {
        std::vector<OpenRAVE::dReal> values(4, 0.0);
        pcontrol->SetDesired(values);
        break;
    }
    default:
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::tiltDown(double value)
{
    yCTrace(YORRM) << value;
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::panLeft(double value)
{
    yCTrace(YORRM) << value;
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::stopCameraMovement()
{
    yCTrace(YORRM);
    return true;
}

// -----------------------------------------------------------------------------
