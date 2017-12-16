// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "FakeControlboard.hpp"

#include <ColorDebug.hpp>

// ------------------- IPositionControl Related --------------------------------

bool roboticslab::FakeControlboard::getAxes(int *ax) {
    *ax = axes;
    CD_INFO("Reporting %d axes are present\n", *ax);
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::positionMove(int j, double ref) {  // encExposed = ref;
    if ((unsigned int)j>axes) {
        CD_ERROR("axis index more than axes.\n");
        return false;
    }
    if(modePosVel!=0) {  // Check if we are in position mode.
        CD_WARNING("will not positionMove as not in positionMode\n");
        return false;
    }
    CD_INFO("positionMove(%d,%f) f[begin]\n",j,ref);
    // Set all the private parameters of the Rave class that correspond to this kind of movement!
    targetExposed[j] = ref;
    double encExposed = getEncExposed(j);
    if (fabs(targetExposed[j]- encExposed)<jointTol[j]) {
        stop(j);  // puts jointStatus[j]=0;
        CD_INFO("Joint q%d reached target.\n",j+1);
        return true;
    } else if ( ref > encExposed) {
        //if(!velocityMove(j, refSpeed[j])) return false;
        velRaw[j] = (refSpeed[j] * velRawExposed[j]);
    } else {
        //if(!velocityMove(j, -refSpeed[j])) return false;
        velRaw[j] = -(refSpeed[j] * velRawExposed[j]);
    }
    jointStatus[j] = 1;
    CD_INFO("positionMove(%d,%f) f[end]\n",j,ref);
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::positionMove(const double *refs) {  // encExposed = refs;
    if(modePosVel!=0) {  // Check if we are in position mode.
        CD_ERROR("Will not positionMove as not in positionMode\n");
        return false;
    }
    CD_INFO("positionMove() f[begin]\n");
    // Find out the maximum time to move
    double max_time = 0;
    std::vector<double> encsExposed = getEncsExposed();
    for(unsigned int motor=0;motor<axes;motor++) {
        CD_INFO("dist[%d]: %f\n",motor,fabs(refs[motor]- encsExposed[motor]));
        CD_INFO("refSpeed[%d]: %f\n",motor,refSpeed[motor]);
        if (fabs((refs[motor]- encsExposed[motor])/refSpeed[motor])>max_time) {
            max_time = fabs((refs[motor]- encsExposed[motor])/refSpeed[motor]);
            CD_INFO(" -->candidate: %f\n",max_time);
        }
    }
    CD_INFO("max_time[final]: %f\n",max_time);
    // Set all the private parameters of the Rave class that correspond to this kind of movement!
    for(unsigned int motor=0;motor<axes;motor++) {
        targetExposed[motor]=refs[motor];
        velRaw[motor] = ((refs[motor]- encsExposed[motor])/max_time)*velRawExposed[motor];
        if(velRaw[motor] != velRaw[motor]) velRaw[motor] = 0;  // protect against NaN
        CD_INFO("velRaw[%d]: %f\n",motor,velRaw[motor]);
        jointStatus[motor]=1;
        if (fabs(targetExposed[motor]- encsExposed[motor])<jointTol[motor]) {
            stop(motor);  // puts jointStatus[motor]=0;
            CD_INFO("Joint q%d reached target.\n",motor+1);
        }
    }
    CD_INFO("positionMove() f[end]\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::relativeMove(int j, double delta) {
    if ((unsigned int)j>axes) return false;
    if(modePosVel!=0) {  // Check if we are in position mode.
        CD_ERROR("FakeControlboard will not relativeMove as not in positionMode\n");
        return false;
    }
    CD_INFO("relativeMove(%d,%f) f[begin]\n",j,delta);
    // Set all the private parameters of the Rave class that correspond to this kind of movement!
    double encExposed = getEncExposed(j);
    targetExposed[j]= encExposed +delta;
    if (fabs(targetExposed[j]- encExposed)<jointTol[j]) {
        stop(j);  // puts jointStatus[j]=0;
        CD_INFO("Joint q%d already at target.\n",j+1);
        return true;
    } else if ( targetExposed[j] > encExposed) {
        // if(!velocityMove(j, refSpeed[j])) return false;
        velRaw[j] = (refSpeed[j] * velRawExposed[j]);
    } else {
        // if(!velocityMove(j, -refSpeed[j])) return false;
        velRaw[j] = -(refSpeed[j] * velRawExposed[j]);
    }
    jointStatus[j]=2;
    CD_INFO("relativeMove(%d,%f) f[end]\n",j,delta);
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::relativeMove(const double *deltas) {  // encExposed = deltas + encExposed
    if(modePosVel!=0) {  // Check if we are in position mode.
        CD_ERROR("will not relativeMove as not in positionMode\n");
        return false;
    }
    CD_INFO("relativeMove() f[begin]\n");
    // Find out the maximum angle to move
    double max_dist = 0;
    double time_max_dist = 0;
    for(unsigned int motor=0;motor<axes;motor++)
        if (fabs(deltas[motor])>max_dist) {
            max_dist = fabs(deltas[motor]);
            time_max_dist = max_dist/refSpeed[motor];  // the max_dist motor will be at refSpeed
        }
    // Set all the private parameters of the Rave class that correspond to this kind of movement!
    std::vector<double> encsExposed = getEncsExposed();
    for(unsigned int motor=0; motor<axes; motor++) {
      targetExposed[motor]= encsExposed[motor] +deltas[motor];
      velRaw[motor] = ((deltas[motor])/time_max_dist)*velRawExposed[motor];
      CD_INFO("velRaw[%d]: %f\n",motor,velRaw[motor]);
      jointStatus[motor]=2;
    }
    CD_INFO("relativeMove() f[end]\n");
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::checkMotionDone(int j, bool *flag) {
    if ((unsigned int)j>axes) return false;
    bool done = true;
    if (jointStatus[j]>0) done=false;
    *flag = done;
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::checkMotionDone(bool *flag) {
    bool done = true;
    for (unsigned int i=0; i<axes; i++) {
        if (jointStatus[i]>0) done = false;
    }
    *flag = done;
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::setRefSpeed(int j, double sp) {
    if ((unsigned int)j>axes) return false;
    refSpeed[j]=sp;
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::setRefSpeeds(const double *spds) {
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setRefSpeed(i,spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::setRefAcceleration(int j, double acc) {
    if ((unsigned int)j>axes) return false;
    refAcc[j]=acc;
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::setRefAccelerations(const double *accs) {
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= setRefAcceleration(i,accs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::getRefSpeed(int j, double *ref) {
    if ((unsigned int)j>axes) return false;
    *ref=refSpeed[j];
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::getRefSpeeds(double *spds) {
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getRefSpeed(i,&spds[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::getRefAcceleration(int j, double *acc) {
    if ((unsigned int)j>axes) return false;
    *acc=refAcc[j];
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::getRefAccelerations(double *accs) {
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= getRefAcceleration(i,&accs[i]);
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::stop(int j) {
    if ((unsigned int)j>axes) return false;
    CD_DEBUG("stop(%d)\n",j);
    velRaw[j]=0.0;
    jointStatus[j]=0;
    return true;
}

// -----------------------------------------------------------------------------

bool roboticslab::FakeControlboard::stop() {
    bool ok = true;
    for(unsigned int i=0;i<axes;i++)
        ok &= stop(i);
    return ok;
}

// -----------------------------------------------------------------------------

