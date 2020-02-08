// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveGrabber.hpp"

#include <ColorDebug.h>

namespace roboticslab
{

// ------------------ IFrameGrabberControls Related ----------------------------------------

bool YarpOpenraveGrabber::getCameraDescription(CameraDescriptor *camera)
{
    return false;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::hasFeature(int feature, bool *hasFeature)
{
    *hasFeature = false;
    if((YARP_FEATURE_GAIN == feature)||(YARP_FEATURE_ZOOM == feature))
        *hasFeature = true;

    return true;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::setFeature(int feature, double value)
{
    boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData const> origGeomDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraGeomData const>(sensorBasePtr->GetSensorGeometry(OpenRAVE::SensorBase::ST_Camera));
    OpenRAVE::SensorBase::CameraGeomData* modGeomDataPtr = new OpenRAVE::SensorBase::CameraGeomData;
    modGeomDataPtr->intrinsics = origGeomDataPtr->intrinsics;
    modGeomDataPtr->width = origGeomDataPtr->width;
    modGeomDataPtr->height = origGeomDataPtr->height;
    modGeomDataPtr->sensor_reference = origGeomDataPtr->sensor_reference;
    modGeomDataPtr->target_region = origGeomDataPtr->target_region;
    modGeomDataPtr->measurement_time = origGeomDataPtr->measurement_time;
    modGeomDataPtr->gain = origGeomDataPtr->gain;

    if(YARP_FEATURE_GAIN == feature)
    {
        modGeomDataPtr->gain = value;
        boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData const> constModGeomDataPtr(modGeomDataPtr);
        sensorBasePtr->SetSensorGeometry(constModGeomDataPtr);
        return true;
    }
    else if(YARP_FEATURE_ZOOM == feature)
    {
        modGeomDataPtr->intrinsics.focal_length = value;
        boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData const> constModGeomDataPtr(modGeomDataPtr);
        sensorBasePtr->SetSensorGeometry(constModGeomDataPtr);
        return true;
    }

    // else...
    return false;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::getFeature(int feature, double *value)
{
    boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData const> geomDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraGeomData const>(sensorBasePtr->GetSensorGeometry(OpenRAVE::SensorBase::ST_Camera));

    if(YARP_FEATURE_GAIN == feature)
    {
        *value = geomDataPtr->gain;
        return true;
    }
    else if(YARP_FEATURE_ZOOM == feature)
    {
        *value = geomDataPtr->intrinsics.focal_length;
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::setFeature(int feature, double  value1, double  value2)
{
    return false;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::getFeature(int feature, double *value1, double *value2)
{
    return false;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::hasOnOff(int feature, bool *HasOnOff)
{
    return false;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::setActive(int feature, bool onoff)
{
    return false;
}

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::getActive(int feature, bool *isActive) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::hasAuto(int feature, bool *hasAuto) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::hasManual(int feature, bool *hasManual) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::hasOnePush(int feature, bool *hasOnePush) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::setMode(int feature, FeatureMode mode) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::getMode(int feature, FeatureMode *mode) { return false; }

// ----------------------------------------------------------------------------

bool YarpOpenraveGrabber::setOnePush(int feature) { return false; }

// ----------------------------------------------------------------------------

}
