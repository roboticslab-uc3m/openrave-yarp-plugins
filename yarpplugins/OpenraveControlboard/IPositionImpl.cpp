// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "OpenraveControlboard.hpp"

// ------------------- IPositionControl Related --------------------------------

bool teo::OpenraveControlboard::getAxes(int *ax) {
    CD_INFO("\n");
    *ax = axes;
    CD_INFO("Reporting %d axes are present\n", *ax);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setPositionMode() {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::positionMove(int j, double ref) {  // encExposed = ref;
    CD_INFO("\n");
    if ((unsigned int)j>axes) {
        fprintf(stderr,"[FakeControlboardOR] error: axis index more than axes.\n");
        return false;
    }
//    if(modePosVel!=0) {  // Check if we are in position mode.
//        fprintf(stderr,"[FakeControlboardOR] warning: will not positionMove as not in positionMode\n");
//        return false;
//    }

    //OpenRAVE::RobotBasePtr probot;
    //std::vector< int > manipulatorIDs;

    dEncRaw[ manipulatorIDs[j] ] = ref;

    probot->SetJointValues(dEncRaw);  // More compatible with physics??

    penv->StepSimulation(0.1);  // StepSimulation must be given in seconds

    {
        OpenRAVE::EnvironmentMutex::scoped_lock lock(penv->GetMutex()); // lock environment
        if(penv->CheckSelfCollision(probot)) {  // Check if we collide.
            CD_WARNING("Collision!!!\n");
            //return false;  // Bad strategy: we get trapped when we are already in collision!!
        }
    }

    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::positionMove(const double *refs) {  // encExposed = refs;
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::relativeMove(int j, double delta) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::relativeMove(const double *deltas) {  // encExposed = deltas + encExposed
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::checkMotionDone(int j, bool *flag) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::checkMotionDone(bool *flag) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setRefSpeed(int j, double sp) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setRefSpeeds(const double *spds) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setRefSpeed(i,spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setRefAcceleration(int j, double acc) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setRefAccelerations(const double *accs) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setRefAcceleration(i,accs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getRefSpeed(int j, double *ref) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getRefSpeeds(double *spds) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getRefSpeed(i,&spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getRefAcceleration(int j, double *acc) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::getRefAccelerations(double *accs) {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getRefAcceleration(i,&accs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::stop(int j) {
    CD_INFO("NOTHING TO DO\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::stop() {
    CD_INFO("\n");
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= stop(i);
    return ok;
}

// -----------------------------------------------------------------------------

