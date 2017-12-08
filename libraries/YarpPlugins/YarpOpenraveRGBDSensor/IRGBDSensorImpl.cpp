// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRGBDSensor.hpp"

namespace roboticslab
{

// ------------------ IRGBDSensor Related ----------------------------------------

int YarpOpenraveRGBDSensor::getDepthHeight()
{
    return 48;
}

// ----------------------------------------------------------------------------

int YarpOpenraveRGBDSensor::getDepthWidth()
{
    return 64;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveRGBDSensor::setDepthResolution(int width, int height)
{
    return false;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveRGBDSensor::getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp)
{
    //CD_DEBUG("\n");

    sensorBasePtr->GetSensorData(sensorDataPtr);

    std::vector< OpenRAVE::RaveVector< OpenRAVE::dReal > > sensorRanges = sensorDataPtr->ranges;
    std::vector< OpenRAVE::RaveVector< OpenRAVE::dReal > > sensorPositions = sensorDataPtr->positions;
    OpenRAVE::Transform tinv = sensorDataPtr->__trans.inverse();

    if(sensorRanges.size()==3072) depthImage.resize(64,48);  // Tamaño de la pantalla (64,48)
    else if(sensorRanges.size()==12288) depthImage.resize(128,96);
    else if(sensorRanges.size()==49152) depthImage.resize(256,192);
    else if(sensorRanges.size()==307200) depthImage.resize(640,480);
    else if(sensorRanges.size()==4) depthImage.resize(2,2);
    //else printf("[warning] unrecognized laser sensor data size.\n");
    else depthImage.resize(sensorRanges.size(),1);
    for (int i_y = 0; i_y < depthImage.height(); ++i_y) {  // was y in x before
        for (int i_x = 0; i_x < depthImage.width(); ++i_x) {
            //double p = sensorRanges[i_y+(i_x*depthImage.height())].z;
            double p;
            if( sensorPositions.size() > 0 ) {
                OpenRAVE::Vector v = tinv*(sensorRanges[i_y+(i_x*depthImage.height())] + sensorPositions[0]);
                p = (float)v.z;
            } else {
                OpenRAVE::Vector v = tinv*(sensorRanges[i_y+(i_x*depthImage.height())]);
                p = (float)v.z;
            }
            depthImage(i_x,i_y) = p*1000.0;  // give mm
        }
    }

    timeStamp->update( yarp::os::Time::now() );

    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveRGBDSensor::getImages(yarp::sig::FlexImage &colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthFrame, yarp::os::Stamp *colorStamp, yarp::os::Stamp *depthStamp)
{
    bool ok = true;
    ok &= getDepthImage(depthFrame,depthStamp);
    return ok;
}

// ----------------------------------------------------------------------------

yarp::dev::IRGBDSensor::RGBDSensor_status YarpOpenraveRGBDSensor::getSensorStatus()
{
    return yarp::dev::IRGBDSensor::RGBD_SENSOR_OK_IN_USE;
}

// ----------------------------------------------------------------------------

}
