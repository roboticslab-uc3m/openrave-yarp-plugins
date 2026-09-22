// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveGrabber.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------ IFrameGrabberControls Related ----------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::getCameraDescription(yarp::dev::CameraDescriptor & camera)
{
    camera = cameraDescriptor;
    return yarp::dev::ReturnValue::return_code::return_value_ok;
}
#else
bool YarpOpenraveGrabber::getCameraDescription(yarp::dev::CameraDescriptor * camera)
{
    *camera = cameraDescriptor;
    return true;
}
#endif

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::hasFeature(yarp::dev::cameraFeature_id_t feature, bool & hasFeature)
#else
bool YarpOpenraveGrabber::hasFeature(int feature, bool * hasFeature)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (feature == yarp::dev::cameraFeature_id_t::YARP_FEATURE_GAIN || feature == yarp::dev::cameraFeature_id_t::YARP_FEATURE_ZOOM)
#else
    if (feature == YARP_FEATURE_GAIN || feature == YARP_FEATURE_ZOOM)
#endif
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        hasFeature = true;
#else
        *hasFeature = true;
#endif
    }
    else
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        hasFeature = false;
#else
        *hasFeature = false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::setFeature(yarp::dev::cameraFeature_id_t feature, double value)
#else
bool YarpOpenraveGrabber::setFeature(int feature, double value)
#endif
{
    // Instead we could
    // - http://www.openrave.org/docs/latest_stable/interface_types/sensor/basecamera/
    //     - setintrinsic
    //         - https://github.com/rdiankov/openrave/blob/2656da7b573004e3e12109b9831797a758a86981/plugins/basesensors/basecamera.h#L177
    //         - https://github.com/personalrobotics/offscreen_render/issues/2#issuecomment-91033096
    //             - sensor.SendCommand('setintrinsic 529 525 328 267 0.01 10')
    //     - setdims
    // - Clone() (???)
    auto origGeomDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraGeomData const>(sensorBasePtr->GetSensorGeometry(OpenRAVE::SensorBase::ST_Camera));
    modGeomDataPtr.reset(new OpenRAVE::SensorBase::CameraGeomData);
    modGeomDataPtr->intrinsics = origGeomDataPtr->intrinsics;
    modGeomDataPtr->width = origGeomDataPtr->width;
    modGeomDataPtr->height = origGeomDataPtr->height;
    modGeomDataPtr->target_region = origGeomDataPtr->target_region;
    modGeomDataPtr->measurement_time = origGeomDataPtr->measurement_time;
    modGeomDataPtr->gain = origGeomDataPtr->gain;

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (feature == yarp::dev::cameraFeature_id_t::YARP_FEATURE_GAIN)
#else
    if (feature == YARP_FEATURE_GAIN)
#endif
    {
        modGeomDataPtr->gain = value;
        boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData const> constModGeomDataPtr(modGeomDataPtr);
        sensorBasePtr->SetSensorGeometry(constModGeomDataPtr);
        yCInfo(YORG) << "Gain set to" << value << "(no visual effect observed at time of writing)";
    }
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    else if (feature == yarp::dev::cameraFeature_id_t::YARP_FEATURE_ZOOM)
#else
    else if (feature == YARP_FEATURE_ZOOM)
#endif
    {
        modGeomDataPtr->intrinsics.focal_length = value;
        modGeomDataPtr->intrinsics.fx = value * origGeomDataPtr->width;
        modGeomDataPtr->intrinsics.fy = value * origGeomDataPtr->height;
        boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData const> constModGeomDataPtr(modGeomDataPtr);
        sensorBasePtr->SetSensorGeometry(constModGeomDataPtr);
        yCInfo(YORG) << "Zoom set to" << value;
    }
    else
    {
        yCError(YORG) << "Feature not recognized";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::getFeature(yarp::dev::cameraFeature_id_t feature, double & value)
#else
bool YarpOpenraveGrabber::getFeature(int feature, double * value)
#endif
{
    auto geomDataPtr = boost::dynamic_pointer_cast<OpenRAVE::SensorBase::CameraGeomData const>(sensorBasePtr->GetSensorGeometry(OpenRAVE::SensorBase::ST_Camera));

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    if (feature == yarp::dev::cameraFeature_id_t::YARP_FEATURE_GAIN)
#else
    if (feature == YARP_FEATURE_GAIN)
#endif
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        value = geomDataPtr->gain;
#else
        *value = geomDataPtr->gain;
#endif
    }
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    else if (feature == yarp::dev::cameraFeature_id_t::YARP_FEATURE_ZOOM)
#else
    else if (feature == YARP_FEATURE_ZOOM)
#endif
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        value = geomDataPtr->intrinsics.focal_length;
#else
        *value = geomDataPtr->intrinsics.focal_length;
#endif
    }
    else
    {
        yCError(YORG) << "Feature not recognized";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_method_failed;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return true;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::setFeature(yarp::dev::cameraFeature_id_t feature, double value1, double value2)
#else
bool YarpOpenraveGrabber::setFeature(int feature, double value1, double value2)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::getFeature(yarp::dev::cameraFeature_id_t feature, double & value1, double & value2)
#else
bool YarpOpenraveGrabber::getFeature(int feature, double * value1, double * value2)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::hasOnOff(yarp::dev::cameraFeature_id_t feature, bool & hasOnOff)
#else
bool YarpOpenraveGrabber::hasOnOff(int feature, bool * hasOnOff)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::setActive(yarp::dev::cameraFeature_id_t feature, bool onoff)
#else
bool YarpOpenraveGrabber::setActive(int feature, bool onoff)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::getActive(yarp::dev::cameraFeature_id_t feature, bool & isActive)
#else
bool YarpOpenraveGrabber::getActive(int feature, bool * isActive)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::hasAuto(yarp::dev::cameraFeature_id_t feature, bool & hasAuto)
#else
bool YarpOpenraveGrabber::hasAuto(int feature, bool * hasAuto)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::hasManual(yarp::dev::cameraFeature_id_t feature, bool & hasManual)
#else
bool YarpOpenraveGrabber::hasManual(int feature, bool * hasManual)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::hasOnePush(yarp::dev::cameraFeature_id_t feature, bool & hasOnePush)
#else
bool YarpOpenraveGrabber::hasOnePush(int feature, bool * hasOnePush)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::setMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode mode)
#else
bool YarpOpenraveGrabber::setMode(int feature, yarp::dev::FeatureMode mode)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::getMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode & mode)
#else
bool YarpOpenraveGrabber::getMode(int feature, yarp::dev::FeatureMode * mode)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveGrabber::setOnePush(yarp::dev::cameraFeature_id_t feature)
#else
bool YarpOpenraveGrabber::setOnePush(int feature)
#endif
{
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#else
    return false;
#endif
}

// ----------------------------------------------------------------------------
