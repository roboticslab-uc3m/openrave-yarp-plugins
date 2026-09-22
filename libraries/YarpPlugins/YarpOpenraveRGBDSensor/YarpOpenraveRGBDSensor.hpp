// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_RGBDSENSOR_HPP__
#define __YARP_OPENRAVE_RGBDSENSOR_HPP__

#include <yarp/conf/version.h>

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
class YarpOpenraveRGBDSensor : private YarpOpenraveBase,
                               public yarp::dev::DeviceDriver,
                               public yarp::dev::IRGBDSensor
{
public:
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    constexpr static auto ret_ok = yarp::dev::ReturnValue::return_code::return_value_ok;
    constexpr static auto ret_not_ok = yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device;
#endif

    // ------- DeviceDriver declarations. Implementation in DeviceDriverImageImpl.cpp -------
    bool open(yarp::os::Searchable & config) override;
    bool close() override;

    // ------- IRGBDSensor declarations. Implementation in IRGBDSensorImpl.cpp -------
    // IRgbVisualParams interface. Look at IVisualParams.h for documentation
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue getRgbSupportedConfigurations(std::vector<yarp::dev::CameraConfig> & configurations) override { return ret_not_ok; }
    yarp::dev::ReturnValue getRgbResolution(int & width, int & height) override { return ret_not_ok; }
    yarp::dev::ReturnValue setRgbResolution(int width, int height) override { return ret_not_ok; }
    yarp::dev::ReturnValue getRgbFOV(double & horizontalFov, double & verticalFov) override { return ret_not_ok; }
    yarp::dev::ReturnValue setRgbFOV(double horizontalFov, double verticalFov) override { return ret_not_ok; }
    yarp::dev::ReturnValue getRgbIntrinsicParam(yarp::os::Property & intrinsic) override { return ret_not_ok; }
    yarp::dev::ReturnValue getRgbMirroring(bool & mirror) override { return ret_not_ok; }
    yarp::dev::ReturnValue setRgbMirroring(bool mirror) override { return ret_not_ok; }
#else
    bool getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig> & configurations) override { return false; }
    bool getRgbResolution(int & width, int & height) override { return false; }
    bool setRgbResolution(int width, int height) override { return false; }
    bool getRgbFOV(double & horizontalFov, double & verticalFov) override { return false; }
    bool setRgbFOV(double horizontalFov, double verticalFov) override { return false; }
    bool getRgbIntrinsicParam(yarp::os::Property & intrinsic) override { return false; }
    bool getRgbMirroring(bool & mirror) override { return false; }
    bool setRgbMirroring(bool mirror) override { return false; }
#endif
    int getRgbHeight() override { return 0; };
    int getRgbWidth() override { return 0; };

    // IDepthVisualParams interface. Look at IVisualParams.h for documentation
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue getDepthResolution(int & width, int & height) override { return ret_not_ok; }
    yarp::dev::ReturnValue setDepthResolution(int width, int height) override { return ret_not_ok; }
    yarp::dev::ReturnValue getDepthFOV(double & horizontalFov, double & verticalFov) override { return ret_not_ok; }
    yarp::dev::ReturnValue setDepthFOV(double horizontalFov, double verticalFov) override { return ret_not_ok; }
    yarp::dev::ReturnValue getDepthAccuracy(double & accuracy) override { accuracy = 0.0; return ret_ok; }
    yarp::dev::ReturnValue setDepthAccuracy(double accuracy) override { return ret_not_ok; }
    yarp::dev::ReturnValue getDepthClipPlanes(double & nearPlane, double & farPlane) override { return ret_not_ok; }
    yarp::dev::ReturnValue setDepthClipPlanes(double nearPlane, double farPlane) override { return ret_not_ok; }
    yarp::dev::ReturnValue getDepthIntrinsicParam(yarp::os::Property & intrinsic) override { return ret_not_ok; }
    yarp::dev::ReturnValue getDepthMirroring(bool & mirror) override { return ret_not_ok; }
    yarp::dev::ReturnValue setDepthMirroring(bool mirror) override { return ret_not_ok; }
#else
    bool setDepthResolution(int width, int height) override { return false; }
    bool getDepthFOV(double & horizontalFov, double & verticalFov) override { return false; }
    bool setDepthFOV(double horizontalFov, double verticalFov) override { return false; }
    double getDepthAccuracy() override { return 0.0; }
    bool setDepthAccuracy(double accuracy) override { return false; }
    bool getDepthClipPlanes(double & nearPlane, double & farPlane) override { return false; }
    bool setDepthClipPlanes(double nearPlane, double farPlane) override { return false; }
    bool getDepthIntrinsicParam(yarp::os::Property & intrinsic) override { return false; }
    bool getDepthMirroring(bool & mirror) override { return false; }
    bool setDepthMirroring(bool mirror) override { return false; }
#endif
    int getDepthHeight() override { return depthHeight; };
    int getDepthWidth() override { return depthWidth; };

    // IRGBDSensor specific interface methods
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue getExtrinsicParam(yarp::sig::Matrix & extrinsic) override { return ret_not_ok; }
    yarp::dev::ReturnValue getLastErrorMsg(std::string & message, yarp::os::Stamp * timeStamp = nullptr) override { message = ""; return ret_ok; }
    yarp::dev::ReturnValue getRgbImage(yarp::sig::FlexImage & rgbImage, yarp::os::Stamp * timeStamp = nullptr) override;
    yarp::dev::ReturnValue getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> & depthImage, yarp::os::Stamp * timeStamp = nullptr) override;
    yarp::dev::ReturnValue getImages(yarp::sig::FlexImage & colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> & depthFrame, yarp::os::Stamp * colorStamp = nullptr, yarp::os::Stamp * depthStamp = nullptr) override;
    yarp::dev::ReturnValue getSensorStatus(yarp::dev::IRGBDSensor::RGBDSensor_status & status) override;
#else
    bool getExtrinsicParam(yarp::sig::Matrix & extrinsic) override { return false; }
    std::string getLastErrorMsg(yarp::os::Stamp * timeStamp = nullptr) override { return ""; }
    bool getRgbImage(yarp::sig::FlexImage & rgbImage, yarp::os::Stamp * timeStamp = nullptr) override;
    bool getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> & depthImage, yarp::os::Stamp * timeStamp = nullptr) override;
    bool getImages(yarp::sig::FlexImage & colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> & depthFrame, yarp::os::Stamp * colorStamp = nullptr, yarp::os::Stamp * depthStamp = nullptr) override;
    yarp::dev::IRGBDSensor::RGBDSensor_status getSensorStatus() override;
#endif

private:
    int rgbHeight, rgbWidth, depthHeight, depthWidth;

    bool rgb {true};
    bool rgbReady {false};
    bool depthReady {false};

    // OpenRAVE
    OpenRAVE::SensorBasePtr depthSensorBasePtr;
    OpenRAVE::SensorBasePtr rgbSensorBasePtr;

    boost::shared_ptr<OpenRAVE::SensorBase::LaserSensorData> depthSensorDataPtr;
    boost::shared_ptr<OpenRAVE::SensorBase::CameraSensorData> rgbSensorDataPtr;
};

} // namespace roboticslab

#endif // __YARP_OPENRAVE_RGBDSENSOR_HPP__
