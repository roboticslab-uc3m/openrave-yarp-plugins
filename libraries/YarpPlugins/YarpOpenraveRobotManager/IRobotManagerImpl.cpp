// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveRobotManager.hpp"

namespace roboticslab
{

// ------------------- DeviceDriver Related ------------------------------------

bool YarpOpenraveRobotManager::moveForward(int velocity)
{
    CD_DEBUG("\n");

    switch (mode)
    {
    case TRANSFORM_IDEALCONTROLLER:
    {
        std::vector<OpenRAVE::dReal> values(2,0.0);
        values[0] = velocity;
        pcontrol->SetDesired(values);
        break;
    }
    case FOUR_WHEEL_IDEALVELOCITYCONTROLLER:
    {
        std::vector<OpenRAVE::dReal> values(4, velocity);
        pcontrol->SetDesired(values);
        break;
    }
    default:
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::moveBackwards(int velocity)
{
    CD_DEBUG("\n");
    std::vector<OpenRAVE::dReal> values(4, -velocity);
    pcontrol->SetDesired(values);
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::turnLeft(int velocity)
{
    CD_DEBUG("\n");
    std::vector<OpenRAVE::dReal> values(4);
    values[0] = -velocity;
    values[1] = velocity;
    values[2] = -velocity;
    values[3] = velocity;
    pcontrol->SetDesired(values);
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::turnRight(int velocity)
{
    CD_DEBUG("\n");
    std::vector<OpenRAVE::dReal> values(4);
    values[0] = velocity;
    values[1] = -velocity;
    values[2] = velocity;
    values[3] = -velocity;
    pcontrol->SetDesired(values);
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::stopMovement()
{
    CD_DEBUG("\n");
    std::vector<OpenRAVE::dReal> values(4, 0.0);
    pcontrol->SetDesired(values);
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::tiltUp(int velocity)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::tiltDown(int velocity)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::panLeft(int velocity)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::panRight(int velocity)
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::stopCameraMovement()
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::connect()
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::disconnect()
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

bool YarpOpenraveRobotManager::test()
{
    CD_DEBUG("\n");
    return true;
}

// -----------------------------------------------------------------------------

void YarpOpenraveRobotManager::setEnabled(bool enabled)
{
    CD_DEBUG("\n");
    return;
}

// -----------------------------------------------------------------------------

void YarpOpenraveRobotManager::onDestroy()
{
    CD_DEBUG("\n");
    return;
}

// -----------------------------------------------------------------------------

}  // namespace roboticslab
