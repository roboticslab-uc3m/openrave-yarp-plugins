// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

// ------------------ IVelocity Related ----------------------------------------

bool roboticslab::YarpOpenraveControlboard::velocityMove(int j, double sp) {  // velExposed = sp;

    //-- Check if we are in position mode.
    if( controlModes[j] != VOCAB_CM_VELOCITY )
    {
        CD_ERROR("Will not velocityMove as not in velocityMode (%d)\n",j);
        return false;
    }

    //-- Code from getLimits
    std::vector<OpenRAVE::dReal> vLowerLimit;
    std::vector<OpenRAVE::dReal> vUpperLimit;
    vectorOfJointPtr[j]->GetLimits(vLowerLimit,vUpperLimit);

    //-- Our joints always have ony 1 DoF, therefore safe to use only [0].
    //double min = radToDegIfNotPrismatic(j,vLowerLimit[0]);
    //double max = radToDegIfNotPrismatic(j,vUpperLimit[0]);
    double min = vLowerLimit[0];
    double max = vUpperLimit[0];

    //CD_DEBUG("Limits %d: [%f, %f]\n",j,min,max);

    OpenRAVE::dReal dofTargetRads;
    if( sp > 0 )
        dofTargetRads = max;
    else if ( sp < 0 )
        dofTargetRads = min;
    else
    {
        this->stop(j);
        //CD_INFO("Done 0 vel (%d).\n",j);
        return true;
    }

    //CD_DEBUG("Target %d: [%f]\n",j,dofTargetRads);

    {
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment

        //-- Check and do immediate movement if appropriate.
        //-- In fact, OpenRAVE would actually do the extra-fast movement but warn at all times.
        double velMin,velMax;
        this->getVelLimits( j, &velMin, &velMax );
        if( sp > velMax )
        {
            CD_WARNING("Command exceeds joint speed limits (%f > %f)).\n",sp,velMax);
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

        OpenRAVE::dReal dofTime = abs( ( dofTargetRads - dofCurrentRads ) / ( degToRadIfNotPrismatic(j,sp) ) ); // Time in seconds

        CD_DEBUG("[%d] abs(target-current)/vel = abs(%f-%f)/%f = %f [s]\n",j,radToDegIfNotPrismatic(j,dofTargetRads),radToDegIfNotPrismatic(j,dofCurrentRads),sp,dofTime);

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

bool roboticslab::YarpOpenraveControlboard::velocityMove(const double *sp) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= velocityMove(i,sp[i]);
    return ok;
}

// ----------------------------------------------------------------------------

