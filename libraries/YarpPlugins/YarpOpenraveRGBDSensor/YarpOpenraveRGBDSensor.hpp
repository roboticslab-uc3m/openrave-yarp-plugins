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

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // ------- IRGBDSensor declarations. Implementation in IRGBDSensorImpl.cpp -------
    // IRgbVisualParams interface. Look at IVisualParams.h for documentation
    int  getRgbHeight() override { return 0; }
    int  getRgbWidth() override { return 0; }
    bool getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig> &configurations) override { return false; }
    bool getRgbResolution(int &width, int &height) override { return false; }
    bool setRgbResolution(int width, int height) override { return false; }
    bool getRgbFOV(double &horizontalFov, double &verticalFov) override { return false; }
    bool setRgbFOV(double horizontalFov, double verticalFov) override { return false; }
    bool getRgbIntrinsicParam(yarp::os::Property &intrinsic) override { return false; }
    bool getRgbMirroring(bool &mirror) override { return false; }
    bool setRgbMirroring(bool mirror) override { return false; }

    // IDepthVisualParams interface. Look at IVisualParams.h for documentation
    int getDepthHeight() override;
    int getDepthWidth() override;
    bool setDepthResolution(int width, int height) override;
    bool getDepthFOV(double &horizontalFov, double &verticalFov) override { return false; }
    bool setDepthFOV(double horizontalFov, double verticalFov) override { return false; }
    double getDepthAccuracy() override { return 0.0; }
    bool setDepthAccuracy(double accuracy) override { return false; }
    bool getDepthClipPlanes(double &nearPlane, double &farPlane) override { return false; }
    bool setDepthClipPlanes(double nearPlane, double farPlane) override { return false; }
    bool getDepthIntrinsicParam(yarp::os::Property &intrinsic) override { return false; }
    bool getDepthMirroring(bool &mirror) override { return false; }
    bool setDepthMirroring(bool mirror) override { return false; }

    // IRGBDSensor specific interface methods
    bool getExtrinsicParam(yarp::sig::Matrix &extrinsic) override { return false; }
    std::string getLastErrorMsg(yarp::os::Stamp *timeStamp = NULL) override { return ""; }
    bool getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp = NULL) override;
    bool getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp = NULL) override;
    bool getImages(yarp::sig::FlexImage &colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthFrame, yarp::os::Stamp *colorStamp=NULL, yarp::os::Stamp *depthStamp=NULL) override;
    RGBDSensor_status getSensorStatus() override;

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
