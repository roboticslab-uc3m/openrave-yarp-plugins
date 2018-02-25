// Authors: see AUTHORS.md at project root.
// CopyPolicy: released under the terms of the LGPLv2.1, see LICENSE at project root.
// URL: https://github.com/asrob-uc3m/yarp-devices

#ifndef __I_ROBOT_MANAGER_HPP__
#define __I_ROBOT_MANAGER_HPP__

#include <yarp/os/Vocab.h>

#define VOCAB_MOVE_FORWARD VOCAB4('m','o','v','f')
#define VOCAB_TURN_LEFT VOCAB4('t','r','n','l')
#define VOCAB_STOP_MOVEMENT VOCAB4('s','t','p','m')

#define VOCAB_TILT_DOWN VOCAB4('t','l','t','d')
#define VOCAB_PAN_LEFT VOCAB4('p','a','n','l')
#define VOCAB_STOP_CAMERA_MOVEMENT VOCAB4('s','t','p','c')

namespace asrob{

/**
 * @ingroup yarp_devices_libraries
 *
 * \defgroup YarpPlugins
 *
  * @brief Base class for Robot Managers. Original copy of this file at https://github.com/asrob-uc3m/yarp-devices/blob/develop/libraries/YarpPlugins/IRobotManager.hpp
 */
class IRobotManager
{
    public:
        /// @brief Robot: Move forward (use negative value for move backward). position mode [m]. Velocity mode [m/s].
        virtual bool moveForward(double value) = 0;

        /// @brief Robot: Turn left (use negative value for turn left). position mode [deg]. Velocity mode [deg/s].
        virtual bool turnLeft(double value) = 0;

        /// @brief Robot: Stop movement.
        virtual bool stopMovement() = 0;

        /// @brief Robot camera: Tilt down (use negative value for tilt up). position mode [deg]. Velocity mode [deg/s].
        virtual bool tiltDown(double value) = 0;

        /// @brief Robot camera: Pan left (use negative value for pan right). position mode [deg]. Velocity mode [deg/s].
        virtual bool panLeft(double value) = 0;

        /// @brief Robot camera: Stop movement.
        virtual bool stopCameraMovement() = 0;
        
        //-- Robot connection related functions
        /// @brief Connect to the remote robot
        virtual bool connect() = 0;

        /// @brief Disconnect from the remote robot
        virtual bool disconnect() = 0;

        /// @brief Test connection (not in used yet)
        virtual bool test() = 0;

        /// @brief Enable/disable sending commands through the manager
        virtual void setEnabled(bool enabled) = 0;

        //-- Other
        virtual void onDestroy() = 0;
        virtual ~IRobotManager() {}
};

}

#endif //-- __I_ROBOT_MANAGER_HPP__
