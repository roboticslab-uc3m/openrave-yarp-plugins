// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <openrave/planningutils.h>

// ------------------- IPositionControl Related --------------------------------

bool roboticslab::YarpOpenraveControlboard::getAxes(int *ax) {
    CD_INFO("\n");
    *ax = axes;
    CD_INFO("Reporting %d axes are present\n", *ax);
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::positionMove(int j, double ref) {  // encExposed = ref;
    CD_INFO("\n");

    //-- Check if we are in position mode.
    if( controlModes[j] != VOCAB_CM_POSITION )
    {
        CD_ERROR("Will not positionMove as not in positionMode\n");
        return false;
    }

    {
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment

        manipulatorTargets[ j ] = ref * M_PI / 180.0;

        OpenRAVE::ConfigurationSpecification activeConfigurationSpecification = probot->GetActiveConfigurationSpecification();
        for (size_t i = 0; i < activeConfigurationSpecification._vgroups.size(); i++)
        {
            CD_DEBUG("%d, %s, %s\n",i,activeConfigurationSpecification._vgroups[i].name.c_str(), activeConfigurationSpecification._vgroups[i].interpolation.c_str());
        }
        std::vector<int> activeDOFIndices = probot->GetActiveDOFIndices();
        for(size_t i=0; i<manipulatorIDs.size(); i++)
        {
            CD_DEBUG("activeDOFIndices[%d]: %d\n",i,activeDOFIndices[i]);
        }

        OpenRAVE::TrajectoryBasePtr ptraj = OpenRAVE::RaveCreateTrajectory(penv,"");
        ptraj->Init(activeConfigurationSpecification);
        std::vector<OpenRAVE::dReal> manipulatorNow;
        probot->GetActiveDOFValues(manipulatorNow); // get current values
        ptraj->Insert(0,manipulatorNow);
        ptraj->Insert(1,manipulatorTargets);
        CD_DEBUG("ptraj-prev\n");
        ptraj->serialize(std::cout);
        CD_DEBUG("ptraj-prev\n");
        OpenRAVE::planningutils::SmoothActiveDOFTrajectory(ptraj,probot);
        CD_DEBUG("ptraj-post\n");
        ptraj->serialize(std::cout);
        CD_DEBUG("ptraj-post\n");
        //CD_DEBUG("ptraj-post: %s\n",ptraj->GetDescription().c_str());
        pcontrol->SetPath(ptraj);
        /*
        int timeoffset = spec.AddDeltaTimeGroup();
        std::vector<OpenRAVE::dReal> q;
        probot->GetActiveDOFValues(q); // get current values
        q[j] = ref * M_PI / 180.0;
        ptraj->Insert(0,q);
        pcontrol->SetPath(ptraj);
        */
        //OpenRAVE::planningutils::SmoothActiveDOFTrajectory(ptraj,probot);
        //probot->GetController()->SetPath(ptraj);
        //pcontrol->SetDesired(manipulatorTargets);

    }
    //penv->StepSimulation(0.1);  // StepSimulation must be given in seconds

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
    CD_WARNING("Always returning true.\n");
    *flag = true;
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::checkMotionDone(bool *flag) {
    CD_WARNING("Always returning true.\n");
    *flag = true;
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::setRefSpeed(int j, double sp) {
    CD_INFO("Doing nothing.\n");
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
    CD_INFO("Doing nothing.\n");
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
    CD_INFO("Doing nothing.\n");
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

