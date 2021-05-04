// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlboard.hpp"

#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

// ------------------ IEncodersTimed Related -----------------------------------------

bool roboticslab::YarpOpenraveControlboard::getEncodersTimed(double *encs, double *time)
{
    yTrace();
    bool ok = true;
    for(unsigned int i=0; i < axes; i++)
        ok &= getEncoderTimed(i,&(encs[i]),&(time[i]));
    return ok;
}

// -----------------------------------------------------------------------------

bool roboticslab::YarpOpenraveControlboard::getEncoderTimed(int j, double *encs, double *time)
{
    yTrace() << j;

    getEncoder(j, encs);
    *time = yarp::os::Time::now();

    return true;
}

// -----------------------------------------------------------------------------
