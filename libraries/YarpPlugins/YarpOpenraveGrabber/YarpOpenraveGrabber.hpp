// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_CONTROLBOARD_HPP__
#define __YARP_OPENRAVE_CONTROLBOARD_HPP__

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#include <openrave-core.h>

#include <boost/make_shared.hpp>

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <vector>

#include "ColorDebug.hpp"

#define DEFAULT_GEN_REF_SPEED 7.5  // Exposed.

namespace roboticslab
{


// Specific for --env parameter
void SetViewer(OpenRAVE::EnvironmentBasePtr penv, const std::string& viewername);

/**
 * @ingroup TeoYarp
 * \defgroup YarpOpenraveGrabber
 *
 * @brief Contains teo::YarpOpenraveGrabber.
 *
 * @section YarpOpenraveGrabber_install Installation
 *
 * The plugin is compiled when ENABLE_TeoYarp_YarpOpenraveGrabber is activated (not default). For further
 * installation steps refer to <a class="el" href="pages.html">your own system installation guidelines</a>.
 */

/**
 * @ingroup YarpOpenraveGrabber
 * @brief Implements the YARP_dev IFrameGrabberImage, etc.
 * interface class member functions.
 */
class YarpOpenraveGrabber : public yarp::dev::DeviceDriver, public yarp::dev::IFrameGrabberImage {
public:

    // Set the Thread Rate in the class constructor
    YarpOpenraveGrabber() {}

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

    // ------- IFrameGrabberImage declarations. Implementation in IFrameGrabberImageImpl.cpp -------
    /**
     * Get an rgb image from the frame grabber, if required
     * demosaicking/color reconstruction is applied
     *
     * @param image the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

    /**
     * Return the height of each frame.
     * @return image height
     */
    virtual int height() const;

    /**
     * Return the width of each frame.
     * @return image width
     */
    virtual int width() const;

private:

    // General Grabber parameters //

    //OpenRAVE//
    OpenRAVE::EnvironmentBasePtr penv;
    OpenRAVE::RobotBasePtr probot;
    std::string robotName;

    // Specific for --env parameter with --view
    boost::thread_group openraveThreads;
};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_CONTROLBOARD_HPP__
