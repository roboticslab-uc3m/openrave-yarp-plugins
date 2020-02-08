// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_GRABBER_HPP__
#define __YARP_OPENRAVE_GRABBER_HPP__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/IVisualParams.h>

#include <openrave/openrave.h>

#include <boost/smart_ptr/shared_ptr.hpp>

#include "YarpOpenraveBase.hpp"

namespace roboticslab
{

/**
 * @ingroup YarpPlugins
 * \defgroup YarpOpenraveGrabber
 *
 * @brief Contains roboticslab::YarpOpenraveGrabber.
 */

/**
 * @ingroup YarpOpenraveGrabber
 * @brief Implements the YARP_dev IFrameGrabberImage, etc.
 * interface class member functions.
 */
class YarpOpenraveGrabber : YarpOpenraveBase,
                            public yarp::dev::DeviceDriver,
                            public yarp::dev::IFrameGrabberControls,
                            public yarp::dev::IFrameGrabberImage,
                            public yarp::dev::IRgbVisualParams
{
public:

    // Set the Thread Rate in the class constructor
    YarpOpenraveGrabber() {}

    // ------- DeviceDriver declarations. Implementation in DeviceDriverImageImpl.cpp -------

    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    // ------- IFrameGrabberControls declarations. Implementation in IFrameGrabberControlsImpl.cpp -------

    bool getCameraDescription(CameraDescriptor *camera) { return false; }
    bool hasFeature(int feature, bool *hasFeature) { return false; }
    bool setFeature(int feature, double value) { return false; }
    bool getFeature(int feature, double *value) { return false; }
    bool setFeature(int feature, double  value1, double  value2) { return false; }
    bool getFeature(int feature, double *value1, double *value2) { return false; }
    bool hasOnOff(int feature, bool *HasOnOff) { return false; }
    bool setActive(int feature, bool onoff) { return false; }
    bool getActive(int feature, bool *isActive) { return false; }
    bool hasAuto(int feature, bool *hasAuto) { return false; }
    bool hasManual(int feature, bool *hasManual) { return false; }
    bool hasOnePush(int feature, bool *hasOnePush) { return false; }
    bool setMode(int feature, FeatureMode mode) { return false; }
    bool getMode(int feature, FeatureMode *mode) { return false; }
    bool setOnePush(int feature) { return false; }

    // ------- IFrameGrabberImage declarations. Implementation in IFrameGrabberImageImpl.cpp -------

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);
    virtual int height() const;
    virtual int width() const;

    // ------- IRgbVisualParams declarations. Look at IVisualParams.h for documentation. Implementation in IFrameGrabberImageImpl.cpp -------

    virtual int  getRgbHeight();
    virtual int  getRgbWidth();
    virtual bool getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig> &configurations) { return true; }
    virtual bool getRgbResolution(int &width, int &height) { return true; }
    virtual bool setRgbResolution(int width, int height) { return true; }
    virtual bool getRgbFOV(double &horizontalFov, double &verticalFov) { return true; }
    virtual bool setRgbFOV(double horizontalFov, double verticalFov) { return true; }
    virtual bool getRgbIntrinsicParam(yarp::os::Property &intrinsic) { return true; }
    virtual bool getRgbMirroring(bool &mirror) { return true; }
    virtual bool setRgbMirroring(bool mirror) { return true; }

private:

    // General Grabber parameters //
    int _height, _width;

    //OpenRAVE//
    OpenRAVE::SensorBasePtr sensorBasePtr;
    boost::shared_ptr<OpenRAVE::SensorBase::CameraSensorData> sensorDataPtr;

    static const int NOT_SET;
};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_GRABBER_HPP__
