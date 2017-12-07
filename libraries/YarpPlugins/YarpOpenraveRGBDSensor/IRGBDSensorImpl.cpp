// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRGBDSensor.hpp"

namespace roboticslab
{

// ------------------ IRGBDSensor Related ----------------------------------------

bool YarpOpenraveRGBDSensor::getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp)
{
    sensorBasePtr->GetSensorData(sensorDataPtr);

    //std::vector< OpenRAVE::RaveVector< OpenRAVE::dReal > > sensorRanges = ptrVectorOfLaserSensorDataPtr->at(laserIter)->ranges;
    //std::vector< OpenRAVE::RaveVector< OpenRAVE::dReal > > sensorPositions = ptrVectorOfLaserSensorDataPtr->at(laserIter)->positions;
    //OpenRAVE::Transform tinv = ptrVectorOfLaserSensorDataPtr->at(laserIter)->__trans.inverse();

    /*
        if(sensorRanges.size()==3072) i_depth.resize(64,48);  // Tamaño de la pantalla (64,48)
        else if(sensorRanges.size()==12288) i_depth.resize(128,96);
        else if(sensorRanges.size()==49152) i_depth.resize(256,192);
        else if(sensorRanges.size()==307200) i_depth.resize(640,480);
        else if(sensorRanges.size()==4) i_depth.resize(2,2);
        //else printf("[warning] unrecognized laser sensor data size.\n");
        else i_depth.resize(sensorRanges.size(),1);
        for (int i_y = 0; i_y < i_depth.height(); ++i_y) {  // was y in x before
            for (int i_x = 0; i_x < i_depth.width(); ++i_x) {
                //double p = sensorRanges[i_y+(i_x*i_depth.height())].z;
                double p;
                if( sensorPositions.size() > 0 ) {
                    OpenRAVE::Vector v = tinv*(sensorRanges[i_y+(i_x*i_depth.height())] + sensorPositions[0]);
                    p = (float)v.z;
                } else {
                    OpenRAVE::Vector v = tinv*(sensorRanges[i_y+(i_x*i_depth.height())]);
                    p = (float)v.z;
                }
                i_depth(i_x,i_y) = p*1000.0;  // give mm
            }
        }
*/
    return true;
}

// ----------------------------------------------------------------------------

}
