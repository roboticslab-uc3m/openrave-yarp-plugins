// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboardCollision.hpp"

// ------------------- IPositionControl Related --------------------------------

bool teo::YarpOpenraveControlboardCollision::getAxes(int *ax) {
    CD_INFO("\n");
    *ax = axes;
    CD_INFO("Reporting %d axes are present\n", *ax);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setPositionMode() {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::positionMove(int j, double ref) {  // encExposed = ref;
    CD_INFO("\n");
    if ((unsigned int)j>axes) {
        fprintf(stderr,"[FakeControlboardOR] error: axis index more than axes.\n");
        return false;
    }

    //Keep the actual position of the joint (before move)
    OpenRAVE::dReal dEncRawKeep;
    dEncRawKeep=dEncRaw[ manipulatorIDs[j] ];

    //Update new position
    dEncRaw[ manipulatorIDs[j] ] = ref;

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

    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::positionMove(const double *refs) {  // encExposed = refs;
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::relativeMove(int j, double delta) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::relativeMove(const double *deltas) {  // encExposed = deltas + encExposed
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::checkMotionDone(int j, bool *flag) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::checkMotionDone(bool *flag) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setRefSpeed(int j, double sp) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setRefSpeeds(const double *spds) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setRefSpeed(i,spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setRefAcceleration(int j, double acc) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::setRefAccelerations(const double *accs) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setRefAcceleration(i,accs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getRefSpeed(int j, double *ref) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getRefSpeeds(double *spds) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getRefSpeed(i,&spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getRefAcceleration(int j, double *acc) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::getRefAccelerations(double *accs) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getRefAcceleration(i,&accs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::stop(int j) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::YarpOpenraveControlboardCollision::stop() {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= stop(i);
    return ok;
}

// -----------------------------------------------------------------------------

