// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_ROBOT_MANAGER_HPP__
#define __YARP_OPENRAVE_ROBOT_MANAGER_HPP__

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#include <openrave-core.h>

#include <boost/make_shared.hpp>

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <vector>

#include "YarpOpenraveBase.hpp"

#include "IRobotManager.hpp"

#include "ColorDebug.hpp"

namespace roboticslab
{

/**
 * @ingroup YarpPlugins
 * \defgroup YarpOpenraveRobotManager
 *
 * @brief Contains roboticslab::YarpOpenraveRobotManager.
 */

/**
 * @ingroup YarpOpenraveRobotManager
 * @brief Implements the YARP_dev DeviceDriver, and IRobotManager.
 * interface class member functions.
 */
class YarpOpenraveRobotManager : YarpOpenraveBase, public yarp::dev::DeviceDriver, public asrob::IRobotManager
{
public:
    //! Lists available translational representations.
    enum robot_mode
    {
        TRANSFORM_IDEALCONTROLLER,
        FOUR_WHEEL_IDEALVELOCITYCONTROLLER
    };

    YarpOpenraveRobotManager() {}


    // -------- DeviceDriver declarations. Implementation in DeviceDriverImpl.cpp --------

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

    // ------- IRobotManager declarations. Implementation in IRobotManagerImpl.cpp -------

    /// @brief Robot: Move forward (use negative value for move backward). position mode [m]. Velocity mode [m/s].
    virtual bool moveForward(double value);

    /// @brief Robot: Turn left (use negative value for turn left). position mode [deg]. Velocity mode [deg/s].
    virtual bool turnLeft(double value);

    /// @brief Robot: Stop movement.
    virtual bool stopMovement();

    /// @brief Robot camera: Tilt down (use negative value for tilt up). position mode [deg]. Velocity mode [deg/s].
    virtual bool tiltDown(double value);

    /// @brief Robot camera: Pan left (use negative value for pan right). position mode [deg]. Velocity mode [deg/s].
    virtual bool panLeft(double value);

    /// @brief Robot camera: Stop movement.
    virtual bool stopCameraMovement();

private:

    robot_mode mode;

    //OpenRAVE//
    OpenRAVE::ControllerBasePtr pcontrol;

};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_ROBOT_MANAGER_HPP__
