// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_GRABBER_HPP__
#define __YARP_OPENRAVE_GRABBER_HPP__

#include <yarp/conf/version.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IRgbVisualParams.h>

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
class YarpOpenraveGrabber : private YarpOpenraveBase,
                            public yarp::dev::DeviceDriver,
                            public yarp::dev::IFrameGrabberControls,
                            public yarp::dev::IFrameGrabberImage,
                            public yarp::dev::IRgbVisualParams
{
public:
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    constexpr static auto ret_ok = yarp::dev::ReturnValue::return_code::return_value_ok;
#endif

    // ------- DeviceDriver declarations. Implementation in DeviceDriverImageImpl.cpp -------
    bool open(yarp::os::Searchable & config) override;
    bool close() override;

    // ------- IFrameGrabberControls declarations. Implementation in IFrameGrabberControlsImpl.cpp -------
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue getCameraDescription(yarp::dev::CameraDescriptor & camera) override;
    yarp::dev::ReturnValue hasFeature(yarp::dev::cameraFeature_id_t feature, bool & hasFeature) override;
    yarp::dev::ReturnValue setFeature(yarp::dev::cameraFeature_id_t feature, double value) override;
    yarp::dev::ReturnValue getFeature(yarp::dev::cameraFeature_id_t feature, double & value) override;
    yarp::dev::ReturnValue setFeature(yarp::dev::cameraFeature_id_t feature, double  value1, double  value2) override;
    yarp::dev::ReturnValue getFeature(yarp::dev::cameraFeature_id_t feature, double & value1, double & value2) override;
    yarp::dev::ReturnValue hasOnOff(yarp::dev::cameraFeature_id_t feature, bool & hasOnOff) override;
    yarp::dev::ReturnValue setActive(yarp::dev::cameraFeature_id_t feature, bool onoff) override;
    yarp::dev::ReturnValue getActive(yarp::dev::cameraFeature_id_t feature, bool & isActive) override;
    yarp::dev::ReturnValue hasAuto(yarp::dev::cameraFeature_id_t feature, bool & hasAuto) override;
    yarp::dev::ReturnValue hasManual(yarp::dev::cameraFeature_id_t feature, bool & hasManual) override;
    yarp::dev::ReturnValue hasOnePush(yarp::dev::cameraFeature_id_t feature, bool & hasOnePush) override;
    yarp::dev::ReturnValue setMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode mode) override;
    yarp::dev::ReturnValue getMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode & mode) override;
    yarp::dev::ReturnValue setOnePush(yarp::dev::cameraFeature_id_t feature) override;
#else
    bool getCameraDescription(yarp::dev::CameraDescriptor * camera) override;
    bool hasFeature(int feature, bool * hasFeature) override;
    bool setFeature(int feature, double value) override;
    bool getFeature(int feature, double * value) override;
    bool setFeature(int feature, double  value1, double value2) override;
    bool getFeature(int feature, double * value1, double * value2) override;
    bool hasOnOff(int feature, bool * HasOnOff) override;
    bool setActive(int feature, bool onoff) override;
    bool getActive(int feature, bool * isActive) override;
    bool hasAuto(int feature, bool * hasAuto) override;
    bool hasManual(int feature, bool * hasManual) override;
    bool hasOnePush(int feature, bool * hasOnePush) override;
    bool setMode(int feature, yarp::dev::FeatureMode mode) override;
    bool getMode(int feature, yarp::dev::FeatureMode * mode) override;
    bool setOnePush(int feature) override;
#endif

    // ------- IFrameGrabberImage declarations. Implementation in IFrameGrabberImageImpl.cpp -------
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image) override;
#else
    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;
#endif
    int height() const override;
    int width() const override;

    // ------- IRgbVisualParams declarations. Look at IVisualParams.h for documentation. Implementation in IFrameGrabberImageImpl.cpp -------
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    yarp::dev::ReturnValue getRgbSupportedConfigurations(std::vector<yarp::dev::CameraConfig> & configurations) override { return ret_ok; }
    yarp::dev::ReturnValue getRgbResolution(int & width, int & height) override { return ret_ok; }
    yarp::dev::ReturnValue setRgbResolution(int width, int height) override { return ret_ok; }
    yarp::dev::ReturnValue getRgbFOV(double & horizontalFov, double & verticalFov) override { return ret_ok; }
    yarp::dev::ReturnValue setRgbFOV(double horizontalFov, double verticalFov) override { return ret_ok; }
    yarp::dev::ReturnValue getRgbIntrinsicParam(yarp::os::Property & intrinsic) override { return ret_ok; }
    yarp::dev::ReturnValue getRgbMirroring(bool & mirror) override { return ret_ok; }
    yarp::dev::ReturnValue setRgbMirroring(bool mirror) override { return ret_ok; }
#else
    bool getRgbSupportedConfigurations(yarp::sig::VectorOf<yarp::dev::CameraConfig> & configurations) override { return true; }
    bool getRgbResolution(int & width, int & height) override { return true; }
    bool setRgbResolution(int width, int height) override { return true; }
    bool getRgbFOV(double & horizontalFov, double & verticalFov) override { return true; }
    bool setRgbFOV(double horizontalFov, double verticalFov) override { return true; }
    bool getRgbIntrinsicParam(yarp::os::Property & intrinsic) override { return true; }
    bool getRgbMirroring(bool & mirror) override { return true; }
    bool setRgbMirroring(bool mirror) override { return true; }
#endif
    int getRgbHeight() override;
    int getRgbWidth() override;

private:
    // General Grabber parameters
    int _height, _width;

    // OpenRAVE
    OpenRAVE::SensorBasePtr sensorBasePtr;
    boost::shared_ptr<OpenRAVE::SensorBase::CameraSensorData> sensorDataPtr;
    boost::shared_ptr<OpenRAVE::SensorBase::CameraGeomData> modGeomDataPtr;

    // YARP
    yarp::dev::CameraDescriptor cameraDescriptor;
};

} // namespace roboticslab

#endif // __YARP_OPENRAVE_GRABBER_HPP__
