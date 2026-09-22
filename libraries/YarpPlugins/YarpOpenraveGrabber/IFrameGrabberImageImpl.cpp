// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveGrabber.hpp"

#include <stdint.h> // uint8_t

#include <vector>

#include <yarp/sig/Image.h>

using namespace roboticslab;

// ------------------ IFrameGrabberImage Related ----------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image)
#else
bool YarpOpenraveGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image)
#endif
{
    sensorBasePtr->GetSensorData(sensorDataPtr);

    std::vector<uint8_t> currentFrame = sensorDataPtr->vimagedata;

    if (currentFrame.empty())
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

    //-- The following code works but provides a glitchy image on some machines, guess not thread safe.
    // image.setExternal(currentFrame.data(),_width,_height);

    //-- So the safe way seems to be using an intermediate image and making a copy.
    yarp::sig::ImageOf<yarp::sig::PixelRgb> tmpImage;
    tmpImage.setExternal(currentFrame.data(), _width, _height);
    image.copy(tmpImage);

    //-- Here's the old inefficient pixel-by-pixel copy in case efficient code ever stops working.
    /*yarp::sig::PixelRgb p;
    image.resize(_width,_height);
    for (int i_x = 0; i_x < image.width(); ++i_x)
    {
        for (int i_y = 0; i_y < image.height(); ++i_y)
        {
            p.r = sensorDataPtr->vimagedata[3*(i_x+(i_y*image.width()))];
            p.g = sensorDataPtr->vimagedata[1+3*(i_x+(i_y*image.width()))];
            p.b = sensorDataPtr->vimagedata[2+3*(i_x+(i_y*image.width()))];
            image.safePixel(i_x,i_y) = p;
        }
    }*/

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// ----------------------------------------------------------------------------

int YarpOpenraveGrabber::height() const
{
    return _height;
}

// ----------------------------------------------------------------------------

int YarpOpenraveGrabber::width() const
{
    return _width;
}

// ----------------------------------------------------------------------------

int YarpOpenraveGrabber::getRgbHeight()
{
    return _height;
}

// ----------------------------------------------------------------------------

int YarpOpenraveGrabber::getRgbWidth()
{
    return _width;
}

// ----------------------------------------------------------------------------
