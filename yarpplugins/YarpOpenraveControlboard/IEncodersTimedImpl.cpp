// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

// ------------------ IEncodersTimed Related -----------------------------------------

bool roboticslab::YarpOpenraveControlboard::getEncodersTimed(double *encs, double *time) {
    //CD_INFO("\n");  //-- Way too verbose
    bool ok = true;
    for(unsigned int i=0; i < axes; i++)
        ok &= getEncoderTimed(i,&(encs[i]),&(time[i]));
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getEncoderTimed(int j, double *encs, double *time) {
    //CD_INFO("(%d)\n",j);  //-- Way too verbose

    getEncoder(j, encs);
    *time = yarp::os::Time::now();

    return true;
}

// -----------------------------------------------------------------------------
