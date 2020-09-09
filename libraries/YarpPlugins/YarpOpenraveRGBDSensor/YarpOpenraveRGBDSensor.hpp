// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_RGBDSENSOR_HPP__
#define __YARP_OPENRAVE_RGBDSENSOR_HPP__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IRGBDSensor.h>

#include <openrave/openrave.h>

#include <boost/smart_ptr/shared_ptr.hpp>

#include "YarpOpenraveBase.hpp"

namespace roboticslab
{

/**
 * @ingroup YarpPlugins
 * \defgroup YarpOpenraveRGBDSensor
 *
 * @brief Contains roboticslab::YarpOpenraveRGBDSensor.
 */

/**
 * @ingroup YarpOpenraveRGBDSensor
 * @brief Implements the YARP_dev IRGBDSensor, etc.
 * interface class member functions.
 */
class YarpOpenraveRGBDSensor : YarpOpenraveBase,
                               public yarp::dev::DeviceDriver,
                               public yarp::dev::IRGBDSensor
{
public:

    YarpOpenraveRGBDSensor() : rgb(true), rgbReady(false), depthReady(false) {}

    // ------- DeviceDriver declarations. Implementation in DeviceDriverImageImpl.cpp -------

    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    // ------- IRGBDSensor declarations. Implementation in IRGBDSensorImpl.cpp -------
    // IRgbVisualParams interface. Look at IVisualParams.h for documentation
    virtual int  getRgbHeight() {}
    virtual int  getRgbWidth() {}
    virtual bool getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig> &configurations) {}
    virtual bool getRgbResolution(int &width, int &height) {}
    virtual bool setRgbResolution(int width, int height) {}
    virtual bool getRgbFOV(double &horizontalFov, double &verticalFov) {}
    virtual bool setRgbFOV(double horizontalFov, double verticalFov) {}
    virtual bool getRgbIntrinsicParam(yarp::os::Property &intrinsic) {}
    virtual bool getRgbMirroring(bool &mirror) {}
    virtual bool setRgbMirroring(bool mirror) {}

    // IDepthVisualParams interface. Look at IVisualParams.h for documentation
    virtual int    getDepthHeight();
    virtual int    getDepthWidth();
    virtual bool   setDepthResolution(int width, int height);
    virtual bool   getDepthFOV(double &horizontalFov, double &verticalFov) {}
    virtual bool   setDepthFOV(double horizontalFov, double verticalFov) {}
    virtual double getDepthAccuracy() {}
    virtual bool   setDepthAccuracy(double accuracy) {}
    virtual bool   getDepthClipPlanes(double &nearPlane, double &farPlane) {}
    virtual bool   setDepthClipPlanes(double nearPlane, double farPlane) {}
    virtual bool   getDepthIntrinsicParam(yarp::os::Property &intrinsic) {}
    virtual bool   getDepthMirroring(bool &mirror) {}
    virtual bool   setDepthMirroring(bool mirror) {}

    // IRGBDSensor specific interface methods
    virtual bool getExtrinsicParam(yarp::sig::Matrix &extrinsic)  {}
    virtual std::string getLastErrorMsg(yarp::os::Stamp *timeStamp = NULL)  {}
    virtual bool getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp = NULL) override;
    virtual bool getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp = NULL) override;
    virtual bool getImages(yarp::sig::FlexImage &colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthFrame, yarp::os::Stamp *colorStamp=NULL, yarp::os::Stamp *depthStamp=NULL)  override;
    virtual RGBDSensor_status getSensorStatus() override;

private:

    // General RGBDSensor parameters
    int rgbHeight, rgbWidth, depthHeight, depthWidth;
    bool rgb;
    bool rgbReady;
    bool depthReady;

    // OpenRAVE
    OpenRAVE::SensorBasePtr depthSensorBasePtr;
    OpenRAVE::SensorBasePtr rgbSensorBasePtr;
    boost::shared_ptr<OpenRAVE::SensorBase::LaserSensorData> depthSensorDataPtr;
    boost::shared_ptr<OpenRAVE::SensorBase::CameraSensorData> rgbSensorDataPtr;

    static const int NOT_SET;
};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_RGBDSENSOR_HPP__
