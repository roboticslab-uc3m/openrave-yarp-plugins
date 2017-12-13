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

#include "YarpOpenraveBase.hpp"

#include "IRobotManager.hpp"

#include "ColorDebug.hpp"

#define DEFAULT_GEN_REF_SPEED 7.5  // Exposed.

namespace roboticslab
{

/**
 * @ingroup TeoYarp
 * \defgroup YarpOpenraveRobotManager
 *
 * @brief Contains teo::YarpOpenraveRobotManager.
 *
 * @section YarpOpenraveRobotManager_install Installation
 *
 * The plugin is compiled when ENABLE_TeoYarp_YarpOpenraveRobotManager is activated (not default). For further
 * installation steps refer to <a class="el" href="pages.html">your own system installation guidelines</a>.
 */

/**
 * @ingroup YarpOpenraveRobotManager
 * @brief Implements the YARP_dev DeviceDriver, and IRobotManager.
 * interface class member functions.
 */
class YarpOpenraveRobotManager : YarpOpenraveBase, public yarp::dev::DeviceDriver, public rd::IRobotManager {
public:

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

    // ------- IRobotManager declarations. Implementation in IRobotManager.cpp -------

    //-- Robot movement related functions
    virtual bool moveForward(int velocity) { return true; }
    virtual bool moveBackwards(int velocity) { return true; }
    virtual bool turnLeft(int velocity) { return true; }
    virtual bool turnRight(int velocity) { return true; }
    virtual bool stopMovement() { return true; }

    //-- Robot camera related functions
    virtual bool tiltUp(int velocity) { return true; }
    virtual bool tiltDown(int velocity) { return true; }
    virtual bool panLeft(int velocity) { return true; }
    virtual bool panRight(int velocity) { return true; }
    virtual bool stopCameraMovement() { return true; }

    //-- Robot connection related functions
    /// @brief Connect to the remote robot
    virtual bool connect() { return true; }

    /// @brief Disconnect from the remote robot
    virtual bool disconnect() { return true; }

    /// @brief Test connection (not in used yet)
    virtual bool test() { return true; }

    /// @brief Enable/disable sending commands through the manager
    virtual void setEnabled(bool enabled) { return; }

    //-- Other
    virtual void onDestroy() { return; }

    // ------------------------------- Private -------------------------------------

private:

    // General Joint Motion Controller parameters //
    unsigned int axes;
    std::vector< int > controlModes;
    std::vector<double> refSpeeds;  // Exposed.

    //OpenRAVE//
    OpenRAVE::MultiControllerBasePtr multi;
    std::vector< OpenRAVE::ControllerBasePtr > pcontrols;
    std::vector< int > manipulatorIDs;
    std::vector<OpenRAVE::dReal> manipulatorTargetRads;
    std::vector<OpenRAVE::RobotBase::JointPtr> vectorOfJointPtr;

    // Specific for --env parameter with --view
    boost::thread_group openraveThreads;
};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_CONTROLBOARD_HPP__
