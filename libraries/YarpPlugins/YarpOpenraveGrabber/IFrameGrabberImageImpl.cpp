// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveGrabber.hpp"

#include <stdint.h> // uint8_t

#include <vector>

#include <yarp/sig/Image.h>

#include <ColorDebug.h>

namespace roboticslab
{

// ------------------ IFrameGrabberImage Related ----------------------------------------

bool YarpOpenraveGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    sensorBasePtr->GetSensorData(sensorDataPtr);

    std::vector<uint8_t> currentFrame = sensorDataPtr->vimagedata;
    //CD_DEBUG("Vector size: %d\n",currentFrame.size()); // i.e. 480 * 640 * 3 = 921600;
    if(0 == currentFrame.size())
    {
        //CD_DEBUG("Waiting for camera...\n");
        return false;
    }

    //-- The following code works but provides a glitchy image on some machines, guess not thread safe.
    // image.setExternal(currentFrame.data(),_width,_height);

    //-- So the safe way seems to be using an intermediate image and making a copy.
    yarp::sig::ImageOf<yarp::sig::PixelRgb> tmpImage;
    tmpImage.setExternal(currentFrame.data(),_width,_height);
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

    return true;
}

// ----------------------------------------------------------------------------

int YarpOpenraveGrabber::height() const
{
    CD_DEBUG("%d\n", _height);
    return _height;
}

// ----------------------------------------------------------------------------

int YarpOpenraveGrabber::width() const
{
    CD_DEBUG("%d\n", _width);
    return _width;
}

// ----------------------------------------------------------------------------

int YarpOpenraveGrabber::getRgbHeight()
{
    CD_DEBUG("%d\n", _height);
    return _height;
}

// ----------------------------------------------------------------------------

int YarpOpenraveGrabber::getRgbWidth()
{
    CD_DEBUG("%d\n", _width);
    return _width;
}

// ----------------------------------------------------------------------------

}
