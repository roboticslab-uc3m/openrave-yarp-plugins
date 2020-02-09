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
    // Instead we could
    // - http://www.openrave.org/docs/latest_stable/interface_types/sensor/basecamera/
    //     - setintrinsic
    //         - https://github.com/rdiankov/openrave/blob/2656da7b573004e3e12109b9831797a758a86981/plugins/basesensors/basecamera.h#L177
    //         - https://github.com/personalrobotics/offscreen_render/issues/2#issuecomment-91033096
    //             - sensor.SendCommand('setintrinsic 529 525 328 267 0.01 10')
    //     - setdims
    // - Clone() (???)
    boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData const> origGeomDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraGeomData const>(sensorBasePtr->GetSensorGeometry(OpenRAVE::SensorBase::ST_Camera));
    modGeomDataPtr.reset(new OpenRAVE::SensorBase::CameraGeomData);
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
        CD_INFO("Gain set to %f (no visual effect observed at time of writing)\n", value);
        return true;
    }
    else if(YARP_FEATURE_ZOOM == feature)
    {
        modGeomDataPtr->intrinsics.focal_length = value;
        modGeomDataPtr->intrinsics.fx = value * origGeomDataPtr->width;
        modGeomDataPtr->intrinsics.fy = value * origGeomDataPtr->height;
        boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData const> constModGeomDataPtr(modGeomDataPtr);
        sensorBasePtr->SetSensorGeometry(constModGeomDataPtr);
        CD_INFO("Zoom set to %f\n", value);
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
