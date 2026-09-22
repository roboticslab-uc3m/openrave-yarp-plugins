// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRGBDSensor.hpp"

#include <stdint.h> // uint8_t

#include <cmath>

#include <vector>

#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Image.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------ IRGBDSensor Related ----------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveRGBDSensor::getRgbImage(yarp::sig::FlexImage & rgbImage, yarp::os::Stamp * timeStamp)
#else
bool YarpOpenraveRGBDSensor::getRgbImage(yarp::sig::FlexImage & rgbImage, yarp::os::Stamp * timeStamp)
#endif
{
    timeStamp->update(yarp::os::Time::now());

    if (!rgb) //-- treat special no RGB case
    {
        yarp::sig::ImageOf<yarp::sig::PixelRgb> tmpImage;
        rgbImage.copy(tmpImage);
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
        return true;
#endif
    }

    if (!rgbSensorBasePtr->GetSensorData(rgbSensorDataPtr))
    {
        yCDebug(YORRS) << "RGB: GetSensorData() failed";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

    std::vector<uint8_t> currentFrame = rgbSensorDataPtr->vimagedata;

    if (currentFrame.size() == 0)
    {
        yCDebug(YORRS) << "RGB: currentFrame.size() == 0";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

    yarp::sig::ImageOf<yarp::sig::PixelRgb> tmpImage;
    tmpImage.setExternal(currentFrame.data(), rgbWidth, rgbHeight);

    //-- Similar to YarpOpenraveGrabber IFrameGrabberImageImpl.cpp, make copy to avoid glitch.
    rgbImage.copy(tmpImage);

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveRGBDSensor::getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> & depthImage, yarp::os::Stamp * timeStamp)
#else
bool YarpOpenraveRGBDSensor::getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> & depthImage, yarp::os::Stamp * timeStamp)
#endif
{
    timeStamp->update(yarp::os::Time::now());

    if (!depthSensorBasePtr->GetSensorData(depthSensorDataPtr))
    {
        yCDebug(YORRS) << "Depth: GetSensorData() failed";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

    std::vector<OpenRAVE::Vector> sensorRanges = depthSensorDataPtr->ranges;

    if (depthHeight == 0 || depthWidth == 0)
    {
        if (sensorRanges.size() == 3072)
        {
            depthWidth  = 64;
            depthHeight = 48;
        }
        else if (sensorRanges.size() == 12288)
        {
            depthWidth  = 128;
            depthHeight = 96;
        }
        else if (sensorRanges.size() == 49152)
        {
            depthWidth  = 256;
            depthHeight = 192;
        }
        else if (sensorRanges.size() == 307200)
        {
            depthWidth  = 640;
            depthHeight = 480;
        }
        else if (sensorRanges.size() == 4)
        {
            depthWidth  = 2;
            depthHeight = 2;
        }
        else if (sensorRanges.size() == 0)
        {
            yCWarning(YORRS) << "sensorRanges.size() == 0";
        }
        else
        {
            depthWidth  = sensorRanges.size();
            depthHeight = 1;
        }
    }

    depthImage.resize(depthWidth, depthHeight);

    for (int i_y = 0; i_y < depthImage.height(); ++i_y)
    {
        for (int i_x = 0; i_x < depthImage.width(); ++i_x)
        {
            OpenRAVE::Vector ranges = sensorRanges[i_y + (i_x * depthImage.height())];
            double distance = std::sqrt(ranges.lengthsqr3());
            depthImage(i_x, i_y) = distance; // [meters]
        }
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveRGBDSensor::getImages(yarp::sig::FlexImage & colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> & depthFrame, yarp::os::Stamp * colorStamp, yarp::os::Stamp * depthStamp)
#else
bool YarpOpenraveRGBDSensor::getImages(yarp::sig::FlexImage & colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> & depthFrame, yarp::os::Stamp * colorStamp, yarp::os::Stamp * depthStamp)
#endif
{
    bool ok = true;

    ok &= getRgbImage(colorFrame, colorStamp);
    ok &= getDepthImage(depthFrame, depthStamp);

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return ok
        ? yarp::dev::ReturnValue::return_code::return_value_ok
        : yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
    return ok;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveRGBDSensor::getSensorStatus(yarp::dev::IRGBDSensor::RGBDSensor_status & status)
#else
yarp::dev::IRGBDSensor::RGBDSensor_status YarpOpenraveRGBDSensor::getSensorStatus()
#endif
{
    if (!rgbReady || !depthReady)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        status = yarp::dev::IRGBDSensor::RGBDSensor_status::RGBD_SENSOR_NOT_READY;
#else
        return RGBDSensor_status::RGBD_SENSOR_NOT_READY;
#endif
    }
    else
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        status = yarp::dev::IRGBDSensor::RGBDSensor_status::RGBD_SENSOR_OK_IN_USE;
#else
        return RGBDSensor_status::RGBD_SENSOR_OK_IN_USE;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#endif
}

// ----------------------------------------------------------------------------
