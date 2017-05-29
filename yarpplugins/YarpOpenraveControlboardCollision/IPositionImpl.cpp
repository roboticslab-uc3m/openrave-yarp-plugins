// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboardCollision.hpp"

// ------------------- IPositionControl Related --------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getAxes(int *ax) {
    CD_INFO("\n");
    return iPositionControl->getAxes(ax);
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::positionMove(int j, double ref) {  // encExposed = ref;
    CD_INFO("\n");
    if ((unsigned int)j>axes) {
        CD_ERROR("axis index more than axes.\n");
        return false;
    }

    //Keep the actual position of the joint (before move)
    OpenRAVE::dReal dEncRawKeep;
    dEncRawKeep=dEncRaw[ manipulatorIDs[j] ];

    //Update new position
    dEncRaw[ manipulatorIDs[j] ] = ref * M_PI / 180.0;

    probot->SetJointValues(dEncRaw);  // More compatible with physics??

    penv->StepSimulation(0.1);  // StepSimulation must be given in seconds

    {
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment
        if(penv->CheckSelfCollision(probot)) {  // Check if we collide.
            CD_WARNING("Collision!!! Invalid position. Going back to the initial position\n");

            //Move back to initial position
            dEncRaw[ manipulatorIDs[j] ]=dEncRawKeep;

            probot->SetJointValues(dEncRaw);  // More compatible with physics??

            penv->StepSimulation(0.1);  // StepSimulation must be given in seconds

            return false;  // Bad strategy: we get trapped when we are already in collision!!
        }
    }

    return iPositionControl->positionMove(j,ref);
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::positionMove(const double *refs) {  // encExposed = refs;
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::relativeMove(int j, double delta) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::relativeMove(const double *deltas) {  // encExposed = deltas + encExposed
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::checkMotionDone(int j, bool *flag) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::checkMotionDone(bool *flag) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setRefSpeed(int j, double sp) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setRefSpeeds(const double *spds) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setRefSpeed(i,spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setRefAcceleration(int j, double acc) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::setRefAccelerations(const double *accs) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setRefAcceleration(i,accs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getRefSpeed(int j, double *ref) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getRefSpeeds(double *spds) {
    CD_INFO("\n");
    return iPositionControl->getRefSpeeds(spds);
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getRefAcceleration(int j, double *acc) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::getRefAccelerations(double *accs) {
    CD_INFO("\n");
    return iPositionControl->getRefAccelerations(accs);
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::stop(int j) {
    CD_INFO("Doing nothing.\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboardCollision::stop() {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= stop(i);
    return ok;
}

// -----------------------------------------------------------------------------

