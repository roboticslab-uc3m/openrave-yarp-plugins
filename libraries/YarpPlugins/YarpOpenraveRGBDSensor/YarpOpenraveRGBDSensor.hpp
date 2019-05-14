// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_RGBDSENSOR_HPP__
#define __YARP_OPENRAVE_RGBDSENSOR_HPP__

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IRGBDSensor.h>

#ifdef OR_RGBDSENSOR_IFRAMEGRABBERCONTROLS_IMPL
# include <yarp/dev/FrameGrabberControl2.h>
#endif // OR_RGBDSENSOR_IFRAMEGRABBERCONTROLS_IMPL

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
class YarpOpenraveRGBDSensor :
#ifdef OR_RGBDSENSOR_IFRAMEGRABBERCONTROLS_IMPL
                               public yarp::dev::IFrameGrabberControls2,
#endif // OR_RGBDSENSOR_IFRAMEGRABBERCONTROLS_IMPL
                               YarpOpenraveBase,
                               public yarp::dev::DeviceDriver,
                               public yarp::dev::IRGBDSensor
{
public:

    // Set the Thread Rate in the class constructor
    YarpOpenraveRGBDSensor() : rgb(true) {}

    // ------- DeviceDriver declarations. Implementation in DeviceDriverImageImpl.cpp -------
    /**
     * Open the DeviceDriver.
     * @param config is a list of parameters for the device.
     * Which parameters are effective for your device can vary.
     * See \ref dev_examples "device invocation examples".
     * If there is no example for your device,
     * you can run the "yarpdev" program with the verbose flag
     * set to probe what parameters the device is checking.
     * If that fails too,
     * you'll need to read the source code (please nag one of the
     * yarp developers to add documentation for your device).
     * @return true/false upon success/failure
     */
    virtual bool open(yarp::os::Searchable& config);

    /**
     * Close the DeviceDriver.
     * @return true/false on success/failure.
     */
    virtual bool close();

    // ------- IRGBDSensor declarations. Implementation in IRGBDSensorImpl.cpp -------
    /*
     *  IRgbVisualParams interface. Look at IVisualParams.h for documentation
     */
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

    /*
     * IDepthVisualParams interface. Look at IVisualParams.h for documentation
     */
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

    /*
     * IRGBDSensor specific interface methods
     */

    /**
     * Get the extrinsic parameters ofrom the device
     * @param  extrinsic  return a rototranslation matrix describing the position
     *         of the depth optical frame with respect to the rgb frame
     * @return true if success
     */
    virtual bool getExtrinsicParam(yarp::sig::Matrix &extrinsic)  {}

    /**
     * Return an error message in case of error. For debugging purpose and user notification.
     * Error message will be reset after any succesful command
     * @return A string explaining the last error occurred.
     */
    virtual std::string getLastErrorMsg(yarp::os::Stamp *timeStamp = NULL)  {}

    /**
     * Get the rgb frame from the device.
     * The pixel type of the source image will usually be set as a VOCAB_PIXEL_RGB,
     * but the user can call the function with the pixel type of his/her choise. The convertion
     * if possible, will be done automatically on client side (TO BO VERIFIED).
     * Note: this will consume CPU power because it will not use GPU optimization.
     * Use VOCAB_PIXEL_RGB for best performances.
     *
     * @param rgbImage the image to be filled.
     * @param timeStamp time in which the image was acquired. Optional, the user must provide memory allocation
     * @return True on success
     */
    virtual bool getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp = NULL);

    /**
     * Get the depth frame from the device.
     * The pixel type of the source image will usually be set as a VOCAB_PIXEL_RGB,
     * but the user can call the function with the pixel type of his/her choise. The convertion
     * if possible, will be done automatically on client side.
     * Note: this will consume CPU power because it will not use GPU optimization.
     * Use VOCAB_PIXEL_RGB for best performances.
     *
     * @param rgbImage the image to be filled.
     * @param timeStamp time in which the image was acquired. Optional, the user must provide memory allocation
     * @return True on success
     */
    virtual bool getDepthImage(yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthImage, yarp::os::Stamp *timeStamp = NULL);

    /**
    * Get the both the color and depth frame in a single call. Implementation should assure the best possible synchronization
    * is achieved accordingly to synch policy set by the user.
    * TimeStamps are referred to acquisition time of the corresponding piece of information.
    * If the device is not providing TimeStamps, then 'timeStamp' field should be set to '-1'.
    * @param colorFrame pointer to FlexImage data to hold the color frame from the sensor
    * @param depthFrame pointer to FlexImage data to hold the depth frame from the sensor
    * @param colorStamp pointer to memory to hold the Stamp of the color frame
    * @param depthStamp pointer to memory to hold the Stamp of the depth frame
    * @return true if able to get both data.
    */
    virtual bool getImages(yarp::sig::FlexImage &colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthFrame, yarp::os::Stamp *colorStamp=NULL, yarp::os::Stamp *depthStamp=NULL);

    /**
     * Get the surrent status of the sensor, using enum type
     *
     * @return an enum representing the status of the robot or an error code
     * if any error is present
     */
    virtual RGBDSensor_status getSensorStatus();

#ifdef OR_RGBDSENSOR_IFRAMEGRABBERCONTROLS_IMPL
    // ------- IFrameGrabberControls declarations. -------

    virtual bool setBrightness(double v) { return false; }
    virtual bool setExposure(double v) { return false; }
    virtual bool setSharpness(double v) { return false; }
    virtual bool setWhiteBalance(double blue, double red) { return false; }
    virtual bool setHue(double v) { return false; }
    virtual bool setSaturation(double v) { return false; }
    virtual bool setGamma(double v) { return false; }
    virtual bool setShutter(double v) { return false; }
    virtual bool setGain(double v) { return false; }
    virtual bool setIris(double v) { return false; }
    virtual double getBrightness() { return 0.0; }
    virtual double getExposure() { return 0.0; }
    virtual double getSharpness() { return 0.0; }
    virtual bool getWhiteBalance(double &blue, double &red) { return false; }
    virtual double getHue() { return 0.0; }
    virtual double getSaturation() { return 0.0; }
    virtual double getGamma() { return 0.0; }
    virtual double getShutter() { return 0.0; }
    virtual double getGain() { return 0.0; }
    virtual double getIris() { return 0.0; }
    virtual bool getCameraDescription(CameraDescriptor *camera) { return false; }
    virtual bool hasFeature(int feature, bool *hasFeature) { return false; }
    virtual bool setFeature(int feature, double value) { return false; }
    virtual bool getFeature(int feature, double *value) { return false; }
    virtual bool setFeature(int feature, double value1, double value2) { return false; }
    virtual bool getFeature(int feature, double *value1, double *value2) { return false; }
    virtual bool hasOnOff(int feature, bool *HasOnOff) { return false; }
    virtual bool setActive(int feature, bool onoff) { return false; }
    virtual bool getActive(int feature, bool *isActive) { return false; }
    virtual bool hasAuto(int feature, bool *hasAuto) { return false; }
    virtual bool hasManual(int feature, bool *hasManual) { return false; }
    virtual bool hasOnePush(int feature, bool *hasOnePush) { return false; }
    virtual bool setMode(int feature, FeatureMode mode) { return false; }
    virtual bool getMode(int feature, FeatureMode *mode) { return false; }
    virtual bool setOnePush(int feature) { return false; }
#endif // OR_RGBDSENSOR_IFRAMEGRABBERCONTROLS_IMPL

private:

    // General Grabber parameters //
    int rgbHeight, rgbWidth, depthHeight, depthWidth;
    bool rgb;

    //OpenRAVE//
    OpenRAVE::SensorBasePtr depthSensorBasePtr;
    OpenRAVE::SensorBasePtr rgbSensorBasePtr;
    boost::shared_ptr<OpenRAVE::SensorBase::LaserSensorData> depthSensorDataPtr;
    boost::shared_ptr<OpenRAVE::SensorBase::CameraSensorData> rgbSensorDataPtr;

    static const int NOT_SET;
};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_RGBDSENSOR_HPP__
