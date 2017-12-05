// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_GRABBER_HPP__
#define __YARP_OPENRAVE_GRABBER_HPP__

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#include <openrave-core.h>

#include <boost/make_shared.hpp>

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <vector>

#include "ColorDebug.hpp"

namespace roboticslab
{


// Specific for --env parameter
void SetViewer(OpenRAVE::EnvironmentBasePtr penv, const std::string& viewername);

/**
 * @ingroup TeoYarp
 * \defgroup YarpOpenraveRGBDSensor
 *
 * @brief Contains teo::YarpOpenraveRGBDSensor.
 *
 * @section YarpOpenraveRGBDSensor_install Installation
 *
 * The plugin is compiled when ENABLE_TeoYarp_YarpOpenraveRGBDSensor is activated (not default). For further
 * installation steps refer to <a class="el" href="pages.html">your own system installation guidelines</a>.
 */

/**
 * @ingroup YarpOpenraveRGBDSensor
 * @brief Implements the YARP_dev IFrameGrabberImage, etc.
 * interface class member functions.
 */
class YarpOpenraveRGBDSensor : public yarp::dev::DeviceDriver, public yarp::dev::IRGBDSensor {
public:

    // Set the Thread Rate in the class constructor
    YarpOpenraveRGBDSensor() {}

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
    virtual int    getDepthHeight() {}
    virtual int    getDepthWidth() {}
    virtual bool   setDepthResolution(int width, int height) {}
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
    virtual yarp::os::ConstString getLastErrorMsg(yarp::os::Stamp *timeStamp = NULL)  {}

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
    virtual bool getRgbImage(yarp::sig::FlexImage &rgbImage, yarp::os::Stamp *timeStamp = NULL)  {}

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
    virtual bool getImages(yarp::sig::FlexImage &colorFrame, yarp::sig::ImageOf<yarp::sig::PixelFloat> &depthFrame, yarp::os::Stamp *colorStamp=NULL, yarp::os::Stamp *depthStamp=NULL) {}

    /**
     * Get the surrent status of the sensor, using enum type
     *
     * @return an enum representing the status of the robot or an error code
     * if any error is present
     */
    virtual RGBDSensor_status getSensorStatus() {}

private:

    // General Grabber parameters //
    int _height, _width;

    //OpenRAVE//
    OpenRAVE::EnvironmentBasePtr penv;
    OpenRAVE::RobotBasePtr probot;
    std::string robotName;

    OpenRAVE::SensorBasePtr sensorBasePtr;
    boost::shared_ptr<OpenRAVE::SensorBase::CameraSensorData> sensorDataPtr;

    // Specific for --env parameter with --view
    boost::thread_group openraveThreads;
};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_GRABBER_HPP__
