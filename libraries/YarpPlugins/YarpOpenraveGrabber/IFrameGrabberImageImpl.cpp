// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveGrabber.hpp"

namespace roboticslab
{

// ------------------ IFrameGrabberImage Related ----------------------------------------

bool YarpOpenraveGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{

    return true;
}

// ----------------------------------------------------------------------------

int YarpOpenraveGrabber::height() const
{

    return 0;
}

// ----------------------------------------------------------------------------

int YarpOpenraveGrabber::width() const
{

    return 0;
}

// ----------------------------------------------------------------------------

}
