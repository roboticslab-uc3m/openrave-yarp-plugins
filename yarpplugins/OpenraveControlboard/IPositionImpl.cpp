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
    CD_INFO("\n");
    if (modePosVel==0) return true;  // Simply return true if we were already in pos mode.
    // Do anything additional before setting flag to pos...
    if(!stop()) {
        fprintf(stderr,"[OpenraveControlboard] warning: setPositionMode() return false; failed to stop\n");
        return false;
    }
    modePosVel = 0;
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::positionMove(int j, double ref) {  // encExposed = ref;
    CD_INFO("\n");
    if ((unsigned int)j>axes) {
        fprintf(stderr,"[OpenraveControlboard] error: axis index more than axes.\n");
        return false;
    }
    if(modePosVel!=0) {  // Check if we are in position mode.
        fprintf(stderr,"[OpenraveControlboard] warning: will not positionMove as not in positionMode\n");
        return false;
    }

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
    CD_INFO("\n");
    if(modePosVel!=0) {  // Check if we are in position mode.
        fprintf(stderr,"[OpenraveControlboard] error: Will not positionMove as not in positionMode\n");
        return false;
    }

    printf("[OpenraveControlboard] positionMove() f[end]\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::relativeMove(int j, double delta) {
    CD_INFO("\n");
    if ((unsigned int)j>axes) return false;
    if(modePosVel!=0) {  // Check if we are in position mode.
        printf("[fail] OpenraveControlboard will not relativeMove as not in positionMode\n");
        return false;
    }
    printf("[OpenraveControlboard] relativeMove(%d,%f) f[begin]\n",j,delta);
    jointStatus[j]=2;
    printf("[OpenraveControlboard] relativeMove(%d,%f) f[end]\n",j,delta);
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::relativeMove(const double *deltas) {  // encExposed = deltas + encExposed
    CD_INFO("\n");
    if(modePosVel!=0) {  // Check if we are in position mode.
        fprintf(stderr,"[OpenraveControlboard] warning: will not relativeMove as not in positionMode\n");
        return false;
    }
    printf("[OpenraveControlboard] relativeMove() f[begin]\n");
    // Find out the maximum angle to move
    double max_dist = 0;
    double time_max_dist = 0;
    for(unsigned int motor=0;motor<axes;motor++)
        if (fabs(deltas[motor])>max_dist) {
            max_dist = fabs(deltas[motor]);
            time_max_dist = max_dist/refSpeed[motor];  // the max_dist motor will be at refSpeed
        }
    // Set all the private parameters of the Rave class that correspond to this kind of movement!
    printf("[OpenraveControlboard] relativeMove() f[end]\n");
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::checkMotionDone(int j, bool *flag) {
    CD_INFO("\n");
    if ((unsigned int)j>axes) return false;
    bool done = true;
    if (jointStatus[j]>0) done=false;
    *flag = done;
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::checkMotionDone(bool *flag) {
    CD_INFO("\n");
    bool done = true;
    for (unsigned int i=0; i<axes; i++) {
        if (jointStatus[i]>0) done = false;
    }
    *flag = done;
    return true;
}

// -----------------------------------------------------------------------------

bool teo::OpenraveControlboard::setRefSpeed(int j, double sp) {
    CD_INFO("\n");
    if ((unsigned int)j>axes) return false;
    refSpeed[j]=sp;
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
    CD_INFO("\n");
    if ((unsigned int)j>axes) return false;
    refAcc[j]=acc;
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
    CD_INFO("\n");
    if ((unsigned int)j>axes) return false;
    *ref=refSpeed[j];
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
    CD_INFO("\n");
    if ((unsigned int)j>axes) return false;
    *acc=refAcc[j];
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
    CD_INFO("\n");
    if ((unsigned int)j>axes) return false;
    printf("[OpenraveControlboard] stop(%d)\n",j);
    velRaw[j]=0.0;
    jointStatus[j]=0;
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

