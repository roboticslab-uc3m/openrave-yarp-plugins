// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARP_OPENRAVE_ROBOT_MANAGER_HPP__
#define __YARP_OPENRAVE_ROBOT_MANAGER_HPP__

#include <yarp/dev/DeviceDriver.h>

#include <openrave/openrave.h>

#include <IRobotManager.hpp>

#include "YarpOpenraveBase.hpp"

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

    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    // ------- IRobotManager declarations. Implementation in IRobotManagerImpl.cpp -------

    virtual bool moveForward(double value);
    virtual bool turnLeft(double value);
    virtual bool stopMovement();
    virtual bool tiltDown(double value);
    virtual bool panLeft(double value);
    virtual bool stopCameraMovement();

private:

    robot_mode mode;

    // OpenRAVE
    OpenRAVE::ControllerBasePtr pcontrol;

};

}  // namespace roboticslab

#endif  // __YARP_OPENRAVE_ROBOT_MANAGER_HPP__
