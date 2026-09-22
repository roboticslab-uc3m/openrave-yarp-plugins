// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "YarpOpenraveControlBoard.hpp"

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"

using namespace roboticslab;

// ------------------- IControlMode Related ------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getAvailableControlModes(int j, std::vector<yarp::dev::SelectableControlModeEnum> & avail)
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "getAvailableControlModes: axis" << j << "is out of bounds";
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
    }

    avail = {
        yarp::dev::SelectableControlModeEnum::VOCAB_CM_POSITION,
        yarp::dev::SelectableControlModeEnum::VOCAB_CM_POSITION_DIRECT,
        yarp::dev::SelectableControlModeEnum::VOCAB_CM_VELOCITY
    };

    return yarp::dev::ReturnValue::return_code::return_value_ok;
}
#endif

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getControlMode(int j, yarp::dev::ControlModeEnum & mode)
#else
bool YarpOpenraveControlBoard::getControlMode(int j, int * mode)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "getControlMode: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    mode = static_cast<yarp::dev::ControlModeEnum>(controlModes[j]);
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    *mode = controlModes[j];
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getControlModes(std::vector<yarp::dev::ControlModeEnum> & modes)
#else
bool YarpOpenraveControlBoard::getControlModes(int * modes)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        ok &= getControlMode(i, modes[i]);
#else
        ok &= getControlMode(i, &(modes[i]));
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return ok;
#endif
}

// ---------------------- IControlMode2 Related  ---------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::getControlModes(const std::vector<int> & joints, std::vector<yarp::dev::ControlModeEnum> & modes)
#else
bool YarpOpenraveControlBoard::getControlModes(int n_joint, const int * joints, int * modes)
#endif
{
    bool ok = true;

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    for (unsigned int i = 0; i < joints.size(); i++)
#else
    for (unsigned int i = 0; i < n_joint; i++)
#endif
    {
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        ok &= getControlMode(joints[i], modes[i]);
#else
        ok &= getControlMode(joints[i], &modes[i]);
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setControlMode(int j, yarp::dev::SelectableControlModeEnum mode)
#else
bool YarpOpenraveControlBoard::setControlMode(int j, int mode)
#endif
{
    if (j < 0 || (unsigned int)j > axes)
    {
        yCError(YORCB) << "setControlMode: axis" << j << "is out of bounds";
#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
        return yarp::dev::ReturnValue::return_code::return_value_error_input_out_of_bounds;
#else
        return false;
#endif
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    controlModes[j] = static_cast<yarp::conf::vocab32_t>(mode);
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    controlModes[j] = mode;
    return true;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setControlModes(const std::vector<yarp::dev::SelectableControlModeEnum> & modes)
#else
bool YarpOpenraveControlBoard::setControlModes(int * modes)
#endif
{
    bool ok = true;

    for (unsigned int i = 0; i < axes; i++)
    {
        ok &= setControlMode(i, modes[i]);
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
yarp::dev::ReturnValue YarpOpenraveControlBoard::setControlModes(const std::vector<int> & joints, const std::vector<yarp::dev::SelectableControlModeEnum> & modes)
#else
bool YarpOpenraveControlBoard::setControlModes(int n_joint, const int * joints, int * modes)
#endif
{
    bool ok = true;

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    for (unsigned int j = 0; j < joints.size(); j++)
#else
    for (int j = 0; j < n_joint; j++)
#endif
    {
        ok &= this->setControlMode(joints[j], modes[j]);
    }

#if YARP_VERSION_COMPARE(>=, 4, 0, 0)
    return yarp::dev::ReturnValue::return_code::return_value_ok;
#else
    return ok;
#endif
}

// -----------------------------------------------------------------------------
